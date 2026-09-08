#include "lbot_motion/left_arm_motion_device.hpp"

#include <algorithm>
#include <cmath>
#include <future>
#include <thread>
#include <utility>

#include "geometry_msgs/msg/vector3.hpp"

namespace lbot_motion {
namespace {

geometry_msgs::msg::Vector3 position_of(const CartesianPose &pose)
{
  geometry_msgs::msg::Vector3 value;
  value.x = pose.x;
  value.y = pose.y;
  value.z = pose.z;
  return value;
}

geometry_msgs::msg::Vector3 euler_of(const CartesianPose &pose)
{
  geometry_msgs::msg::Vector3 value;
  value.x = pose.roll;
  value.y = pose.pitch;
  value.z = pose.yaw;
  return value;
}

template<typename FutureT>
bool response_ready(FutureT &future, std::chrono::milliseconds timeout)
{
  return future.wait_for(timeout) == std::future_status::ready;
}

std::string suffix_label(const std::string &label)
{
  return label.empty() ? std::string() : " for " + label;
}

}  // namespace

LeftArmMotionDevice::LeftArmMotionDevice(
  const rclcpp::Node::SharedPtr &node,
  std::string robot_namespace,
  std::chrono::milliseconds state_timeout,
  std::chrono::milliseconds service_timeout)
: node_(node), robot_namespace_(std::move(robot_namespace)),
  state_timeout_(state_timeout), service_timeout_(service_timeout)
{
  left_state_sub_ = node_->create_subscription<sensor_msgs::msg::JointState>(
    endpoint("left_arm/joint_states"), 10,
    [this](sensor_msgs::msg::JointState::SharedPtr message) {on_left_state(message);});
  move_j_client_ = node_->create_client<MoveJ>(endpoint("left_arm/move_joint"));
  move_jp_client_ = node_->create_client<MoveJP>(endpoint("left_arm/move_pose"));
  move_l_client_ = node_->create_client<MoveL>(endpoint("left_arm/move_linear"));
  ik_client_ = node_->create_client<InverseKinematics>(endpoint("left_arm/inverse_kinematics"));
  emergency_client_ =
    node_->create_client<SetEmergency>(endpoint("left_arm/set_emergency_stop"));
  hand_speed_pub_ = node_->create_publisher<std_msgs::msg::UInt8MultiArray>(
    endpoint("left_hand/set_l6_speed"), 10);
  hand_force_pub_ = node_->create_publisher<std_msgs::msg::UInt8MultiArray>(
    endpoint("left_hand/set_l6_force"), 10);
  hand_position_pub_ = node_->create_publisher<std_msgs::msg::UInt8MultiArray>(
    endpoint("left_hand/set_l6_joint"), 10);
}

std::string LeftArmMotionDevice::endpoint(const std::string &suffix) const
{
  std::string prefix = robot_namespace_;
  if (prefix.empty() || prefix == "/") return "/" + suffix;
  if (prefix.front() != '/') prefix.insert(prefix.begin(), '/');
  while (!prefix.empty() && prefix.back() == '/') prefix.pop_back();
  return prefix + "/" + suffix;
}

void LeftArmMotionDevice::on_left_state(
  const sensor_msgs::msg::JointState::SharedPtr message)
{
  if (message->position.size() < left_joints_.size()) return;
  {
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::copy_n(message->position.begin(), left_joints_.size(), left_joints_.begin());
    left_state_ready_ = true;
    ++left_state_sequence_;
  }
  state_cv_.notify_all();
}

bool LeftArmMotionDevice::wait_for_state()
{
  std::unique_lock<std::mutex> lock(state_mutex_);
  return state_cv_.wait_for(
    lock, state_timeout_,
    [this]() {return left_state_ready_;});
}

std::array<double, 7> LeftArmMotionDevice::left_joints() const
{
  std::lock_guard<std::mutex> lock(state_mutex_);
  return left_joints_;
}

bool LeftArmMotionDevice::wait_until_left_joints(
  const std::array<double, 7> &target,
  double tolerance_rad,
  std::chrono::milliseconds timeout,
  std::size_t stable_samples)
{
  if (!(tolerance_rad > 0.0) || stable_samples == 0) return false;
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  std::unique_lock<std::mutex> lock(state_mutex_);
  std::uint64_t observed_sequence = left_state_sequence_;
  std::size_t stable_count = 0;
  while (std::chrono::steady_clock::now() < deadline) {
    state_cv_.wait_until(
      lock, deadline,
      [this, observed_sequence]() {return left_state_sequence_ != observed_sequence;});
    if (left_state_sequence_ == observed_sequence) break;
    observed_sequence = left_state_sequence_;
    bool reached = left_state_ready_;
    for (std::size_t index = 0; index < target.size() && reached; ++index) {
      reached = std::abs(left_joints_[index] - target[index]) <= tolerance_rad;
    }
    stable_count = reached ? stable_count + 1 : 0;
    if (stable_count >= stable_samples) return true;
  }
  return false;
}

DeviceResult LeftArmMotionDevice::move_joints(
  const std::array<double, 7> &target,
  const ArmMotionOptions &options,
  const std::string &label)
{
  if (!move_j_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left MoveJ service unavailable" + suffix_label(label));
  }
  auto request = std::make_shared<MoveJ::Request>();
  request->joints.assign(target.begin(), target.end());
  request->speed = static_cast<float>(std::max(0.0, options.speed));
  request->acce = static_cast<float>(std::max(0.0, options.acceleration));
  request->block = options.block;
  auto future = move_j_client_->async_send_request(request);
  const auto response_timeout = options.block ?
    service_timeout_ + std::chrono::seconds(30) : service_timeout_;
  if (!response_ready(future, response_timeout)) {
    return DeviceResult::fail("left MoveJ response timeout" + suffix_label(label));
  }
  if (!future.get()->success) {
    return DeviceResult::fail("left MoveJ rejected" + suffix_label(label));
  }
  return DeviceResult::ok(label);
}

DeviceResult LeftArmMotionDevice::move_pose(
  const CartesianPose &target,
  const ArmMotionOptions &options,
  const std::string &label)
{
  if (!move_jp_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left MoveJP service unavailable" + suffix_label(label));
  }
  auto request = std::make_shared<MoveJP::Request>();
  request->position = position_of(target);
  request->euler = euler_of(target);
  request->speed = static_cast<float>(std::max(0.0, options.speed));
  request->acce = static_cast<float>(std::max(0.0, options.acceleration));
  request->block = options.block;
  auto future = move_jp_client_->async_send_request(request);
  const auto response_timeout = options.block ?
    service_timeout_ + std::chrono::seconds(30) : service_timeout_;
  if (!response_ready(future, response_timeout)) {
    return DeviceResult::fail("left MoveJP response timeout" + suffix_label(label));
  }
  if (!future.get()->success) {
    return DeviceResult::fail("left MoveJP rejected" + suffix_label(label));
  }
  return DeviceResult::ok(label);
}

DeviceResult LeftArmMotionDevice::move_linear(
  const CartesianPose &target,
  const ArmMotionOptions &options,
  const std::string &label)
{
  if (!move_l_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left MoveL service unavailable" + suffix_label(label));
  }
  auto request = std::make_shared<MoveL::Request>();
  request->position = position_of(target);
  request->euler = euler_of(target);
  request->speed = static_cast<float>(std::max(0.0, options.speed));
  request->acce = static_cast<float>(std::max(0.0, options.acceleration));
  request->block = options.block;
  auto future = move_l_client_->async_send_request(request);
  const auto response_timeout = options.block ?
    service_timeout_ + std::chrono::seconds(30) : service_timeout_;
  if (!response_ready(future, response_timeout)) {
    return DeviceResult::fail("left MoveL response timeout" + suffix_label(label));
  }
  if (!future.get()->success) {
    return DeviceResult::fail("left MoveL rejected" + suffix_label(label));
  }
  return DeviceResult::ok(label);
}

DeviceResult LeftArmMotionDevice::inverse_kinematics(
  const CartesianPose &target,
  const std::array<double, 7> &seed,
  std::array<double, 7> &solution)
{
  if (!ik_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left inverse-kinematics service unavailable");
  }
  auto request = std::make_shared<InverseKinematics::Request>();
  request->joints.assign(seed.begin(), seed.end());
  request->position = position_of(target);
  request->euler = euler_of(target);
  auto future = ik_client_->async_send_request(request);
  if (!response_ready(future, service_timeout_)) {
    return DeviceResult::fail("left inverse-kinematics response timeout");
  }
  const auto response = future.get();
  if (!response->success || response->joints.size() != solution.size()) {
    return DeviceResult::fail("left inverse-kinematics request failed");
  }
  std::copy_n(response->joints.begin(), solution.size(), solution.begin());
  return DeviceResult::ok();
}

DeviceResult LeftArmMotionDevice::set_hand(
  const std::array<uint8_t, 6> &positions,
  uint8_t speed_value,
  uint8_t force_value,
  int repeat)
{
  std_msgs::msg::UInt8MultiArray speed;
  std_msgs::msg::UInt8MultiArray force;
  std_msgs::msg::UInt8MultiArray position;
  speed.data.assign(6, speed_value);
  force.data.assign(6, force_value);
  position.data.assign(positions.begin(), positions.end());
  for (int index = 0; index < std::max(1, repeat); ++index) {
    hand_speed_pub_->publish(speed);
    hand_force_pub_->publish(force);
    hand_position_pub_->publish(position);
    if (index + 1 < std::max(1, repeat)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }
  return DeviceResult::ok();
}

DeviceResult LeftArmMotionDevice::set_emergency_stop(bool emergency)
{
  if (!emergency_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left emergency-stop service unavailable");
  }
  auto request = std::make_shared<SetEmergency::Request>();
  request->emergency = emergency;
  auto future = emergency_client_->async_send_request(request);
  if (!response_ready(future, service_timeout_)) {
    return DeviceResult::fail("left emergency-stop response timeout");
  }
  if (!future.get()->success) return DeviceResult::fail("left emergency-stop request failed");
  return DeviceResult::ok();
}

}  // namespace lbot_motion
