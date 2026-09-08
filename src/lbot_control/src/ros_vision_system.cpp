#include "lbot_control/ros_vision_system.hpp"

#include <algorithm>
#include <cmath>
#include <future>

namespace lbot_control {
namespace {

std::int64_t stamp_ns(const std_msgs::msg::Header &header)
{
  return static_cast<std::int64_t>(header.stamp.sec) * 1000000000LL + header.stamp.nanosec;
}

}  // namespace

RosVisionSystem::RosVisionSystem(
  const rclcpp::Node::SharedPtr &node, RosVisionConfig config)
: node_(node), config_(std::move(config))
{
  sequence_sub_ = node_->create_subscription<Sequence>(
    config_.sequence_topic, rclcpp::QoS(10),
    [this](Sequence::ConstSharedPtr message) {
      std::lock_guard<std::mutex> lock(mutex_);
      sequence_ = std::move(message);
      condition_.notify_all();
    });
  slots_sub_ = node_->create_subscription<geometry_msgs::msg::PoseArray>(
    config_.slots_topic, rclcpp::QoS(10),
    [this](geometry_msgs::msg::PoseArray::ConstSharedPtr message) {
      std::lock_guard<std::mutex> lock(mutex_);
      slots_ = std::move(message);
      condition_.notify_all();
    });
  event_client_ = node_->create_client<SetNutState>(config_.event_service);
}

bool RosVisionSystem::valid_frame(const std::string &frame) const
{
  return frame == config_.base_frame;
}

Pose6 RosVisionSystem::pose_from_point(double x, double y, double z)
{
  Pose6 pose;
  pose.x = x; pose.y = y; pose.z = z;
  return pose;
}

bool RosVisionSystem::target_pose(const Sequence &message, std::size_t index, Pose6 &pose) const
{
  if (index >= message.targets.size()) return false;
  const auto &target = message.targets[index];
  if (!target.visible || !target.position_valid || !valid_frame(target.position.header.frame_id)) {
    return false;
  }
  pose = pose_from_point(
    target.position.point.x, target.position.point.y, target.position.point.z);
  return std::isfinite(pose.x) && std::isfinite(pose.y) && std::isfinite(pose.z);
}

bool RosVisionSystem::slot_pose(
  const geometry_msgs::msg::PoseArray &message, std::size_t index, Pose6 &pose) const
{
  if (index >= message.poses.size() || !valid_frame(message.header.frame_id)) return false;
  const auto &position = message.poses[index].position;
  pose = pose_from_point(position.x, position.y, position.z);
  return std::isfinite(pose.x) && std::isfinite(pose.y) && std::isfinite(pose.z);
}

SceneResult RosVisionSystem::initial_scene()
{
  std::unique_lock<std::mutex> lock(mutex_);
  const bool ready = condition_.wait_for(lock, config_.wait_timeout, [this]() {
    if (!sequence_ || !slots_) return false;
    return sequence_->initialized && sequence_->observation_valid &&
           sequence_->expected_count == 3 && sequence_->current_target_id == 1 &&
           slots_->poses.size() == 3;
  });
  if (!ready) return SceneResult::fail("timed out waiting for initial vision scene");

  SceneObservation scene;
  scene.frame_id = config_.base_frame;
  for (std::size_t index = 0; index < 3; ++index) {
    Pose6 nut, slot;
    if (!target_pose(*sequence_, index, nut) || !slot_pose(*slots_, index, slot)) {
      return SceneResult::fail("initial scene contains an invalid target or slot pose");
    }
    scene.targets[index].size = static_cast<NutSize>(index);
    scene.targets[index].nut = nut;
    scene.targets[index].slot = slot;
  }
  return SceneResult::ok(scene);
}

VisionCommandResult RosVisionSystem::send_event(NutSize size, const std::string &action)
{
  if (!event_client_->wait_for_service(config_.service_timeout)) {
    return VisionCommandResult::fail("vision event service unavailable");
  }
  std::uint64_t round = 0;
  std::string session;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!sequence_) return VisionCommandResult::fail("no vision sequence received");
    round = sequence_->round_id;
    session = sequence_->session_id;
  }
  auto request = std::make_shared<SetNutState::Request>();
  request->session_id = session;
  request->round_id = round;
  request->event_sequence = ++event_sequence_;
  request->target_id = static_cast<std::uint8_t>(size) + 1;
  request->action = action;
  auto future = event_client_->async_send_request(request);
  if (future.wait_for(config_.service_timeout) != std::future_status::ready) {
    return VisionCommandResult::fail("vision event service response timeout");
  }
  const auto response = future.get();
  if (!response->accepted) return VisionCommandResult::fail(response->reason);
  return VisionCommandResult::ok(response->reason);
}

VisionCommandResult RosVisionSystem::start_target(NutSize size)
{
  const auto target_id = static_cast<std::uint8_t>(size) + 1;
  std::unique_lock<std::mutex> lock(mutex_);
  const bool ready = condition_.wait_for(lock, config_.wait_timeout, [this, target_id]() {
    return sequence_ && sequence_->observation_valid &&
      sequence_->current_target_id == target_id &&
      static_cast<std::size_t>(target_id) <= sequence_->targets.size() &&
      sequence_->targets[target_id - 1].visible &&
      sequence_->targets[target_id - 1].position_valid;
  });
  if (!ready) {
    return VisionCommandResult::fail("timed out waiting for a stable visual target");
  }
  target_start_stamp_ns_ = stamp_ns(sequence_->header);
  lock.unlock();
  return send_event(size, "start");
}

bool RosVisionSystem::wait_for_observation(
  std::int64_t previous_stamp_ns, Sequence &sequence,
  geometry_msgs::msg::PoseArray &slots)
{
  std::unique_lock<std::mutex> lock(mutex_);
  const bool ready = condition_.wait_for(lock, config_.wait_timeout, [this, previous_stamp_ns]() {
    return sequence_ && stamp_ns(sequence_->header) > previous_stamp_ns;
  });
  if (!ready) return false;
  sequence = *sequence_;
  if (slots_) slots = *slots_;
  return true;
}

PickCheckResult RosVisionSystem::check_nut_in_source(NutSize size)
{
  const auto deadline = std::chrono::steady_clock::now() + config_.wait_timeout;
  auto previous_stamp = target_start_stamp_ns_;
  while (std::chrono::steady_clock::now() < deadline) {
    Sequence sequence;
    geometry_msgs::msg::PoseArray slots;
    if (!wait_for_observation(previous_stamp, sequence, slots)) break;
    previous_stamp = stamp_ns(sequence.header);
    const auto expected = sequence.expected_count;
    const auto observed = sequence.observed_count;
    const auto target_id = static_cast<std::uint8_t>(size) + 1;
    const bool present = sequence.observation_valid && sequence.current_target_id == target_id &&
      observed == expected && static_cast<std::size_t>(target_id) <= sequence.targets.size() &&
      sequence.targets[target_id - 1].visible;
    const bool removed = !present && expected > 0 && observed + 1 == expected &&
      sequence.status == "observation_count_mismatch";
    if (!present && !removed) continue;  // transient frame/TF/depth failure

    const auto event = send_event(size, present ? "retry" : "complete");
    if (!event.success) return PickCheckResult::fail(event.message);
    return present ? PickCheckResult::present(event.message) : PickCheckResult::removed(event.message);
  }
  return PickCheckResult::fail("timed out waiting for a clear post-action vision observation");
}

}  // namespace lbot_control
