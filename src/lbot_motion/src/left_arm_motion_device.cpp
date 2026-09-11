#include "lbot_motion/left_arm_motion_device.hpp"

#include <algorithm>
#include <cmath>
#include <future>
#include <sstream>
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
  left_pose_sub_ = node_->create_subscription<geometry_msgs::msg::PoseStamped>(
    endpoint("left_arm/pose_states"), 10,
    [this](geometry_msgs::msg::PoseStamped::ConstSharedPtr message) {
      {
        std::lock_guard<std::mutex> lock(state_mutex_);
        left_pose_ = std::move(message);
        ++left_pose_sequence_;
      }
      state_cv_.notify_all();
    });
  move_j_client_ = node_->create_client<MoveJ>(endpoint("left_arm/move_joint"));
  joint_follow_pub_ = node_->create_publisher<lbot_arm_interfaces::msg::FollowJoint>(
    endpoint("left_arm/joint_follow"), rclcpp::QoS(1).reliable().durability_volatile());
  move_jp_client_ = node_->create_client<MoveJP>(endpoint("left_arm/move_pose"));
  move_l_client_ = node_->create_client<MoveL>(endpoint("left_arm/move_linear"));
  ik_client_ = node_->create_client<InverseKinematics>(endpoint("left_arm/inverse_kinematics"));
  fk_client_ = node_->create_client<ForwardKinematics>(endpoint("left_arm/forward_kinematics"));
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
  if (message->position.size() != left_joints_.size() ||
      !std::all_of(message->position.begin(), message->position.end(),
        [](double value) {return std::isfinite(value);})) return;
  {
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::copy_n(message->position.begin(), left_joints_.size(), left_joints_.begin());
    left_state_ready_ = true;
    left_state_received_ = std::chrono::steady_clock::now();
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

LeftJointFeedback LeftArmMotionDevice::left_joint_feedback() const
{
  std::lock_guard<std::mutex> lock(state_mutex_);
  return {left_state_ready_, left_joints_, left_state_sequence_, left_state_ready_ ?
    std::chrono::steady_clock::now() - left_state_received_ : std::chrono::steady_clock::duration::zero()};
}

bool LeftArmMotionDevice::fresh_left_joints(
  std::array<double, 7> &joints, std::chrono::milliseconds max_age, std::uint64_t *sequence) const
{
  const auto feedback = left_joint_feedback();
  if (!feedback.available || feedback.age > max_age) return false;
  joints = feedback.joints;
  if (sequence) *sequence = feedback.sequence;
  return true;
}

bool LeftArmMotionDevice::wait_for_joint_follow_subscriber()
{
  const auto deadline = std::chrono::steady_clock::now() + service_timeout_;
  while (rclcpp::ok() && std::chrono::steady_clock::now() < deadline) {
    if (joint_follow_pub_->get_subscription_count() > 0) return true;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  return false;
}

DeviceResult LeftArmMotionDevice::joint_follow(const std::array<double, 7> &target)
{
  if (!std::all_of(target.begin(), target.end(), [](double value) {return std::isfinite(value);})) {
    return DeviceResult::fail("non-finite joint_follow target");
  }
  if (!rclcpp::ok() || joint_follow_pub_->get_subscription_count() == 0) {
    return DeviceResult::fail("left joint_follow subscriber unavailable");
  }
  lbot_arm_interfaces::msg::FollowJoint message;
  message.joints.assign(target.begin(), target.end());
  message.follow = true;
  joint_follow_pub_->publish(message);
  // Publication is not an SDK acknowledgement; the task monitors measured joints.
  return DeviceResult::ok();
}

bool LeftArmMotionDevice::wait_for_fresh_state()
{
  std::unique_lock<std::mutex> lock(state_mutex_);
  const auto previous = left_state_sequence_;
  return state_cv_.wait_for(lock, state_timeout_,
    [this, previous]() {return left_state_ready_ && left_state_sequence_ != previous;});
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

DeviceResult LeftArmMotionDevice::wait_until_left_pose(
  const CartesianPose &target, const std::string &frame,
  double position_tolerance_m, double orientation_tolerance_rad,
  std::chrono::milliseconds timeout, std::size_t stable_samples, CartesianPose *reached_pose)
{
  const std::array<double, 6> values{target.x, target.y, target.z, target.roll, target.pitch, target.yaw};
  if (!std::all_of(values.begin(), values.end(), [](double v) {return std::isfinite(v);}) ||
      frame.empty() || !std::isfinite(position_tolerance_m) || position_tolerance_m <= 0.0 ||
      !std::isfinite(orientation_tolerance_rad) || orientation_tolerance_rad <= 0.0 ||
      timeout.count() <= 0 || stable_samples == 0) {
    return DeviceResult::fail("invalid pose-arrival check parameters");
  }
  const double cr = std::cos(target.roll / 2), sr = std::sin(target.roll / 2);
  const double cp = std::cos(target.pitch / 2), sp = std::sin(target.pitch / 2);
  const double cy = std::cos(target.yaw / 2), sy = std::sin(target.yaw / 2);
  const std::array<double, 4> desired{
    sr*cp*cy-cr*sp*sy, cr*sp*cy+sr*cp*sy, cr*cp*sy-sr*sp*cy, cr*cp*cy+sr*sp*sy};
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  std::unique_lock<std::mutex> lock(state_mutex_);
  auto sequence = left_pose_sequence_;
  const auto minimum_stamp = node_->now().nanoseconds();
  std::size_t stable = 0;
  std::int64_t last_stamp = -1;
  std::string last_status = "no fresh pose feedback";
  while (std::chrono::steady_clock::now() < deadline) {
    state_cv_.wait_until(lock, deadline, [this, sequence]() {return left_pose_sequence_ != sequence;});
    if (left_pose_sequence_ == sequence) break;
    sequence = left_pose_sequence_;
    const auto message = left_pose_;
    if (!message) continue;
    const auto stamp = static_cast<std::int64_t>(message->header.stamp.sec) * 1000000000LL +
      message->header.stamp.nanosec;
    const auto age = node_->now().nanoseconds() - stamp;
    if (message->header.frame_id != frame || stamp < minimum_stamp || stamp <= last_stamp || age < 0 ||
        age > std::chrono::duration_cast<std::chrono::nanoseconds>(state_timeout_).count()) {
      stable = 0;
      last_status = "pose feedback has wrong frame or stale timestamp";
      continue;
    }
    last_stamp = stamp;
    const auto &p = message->pose.position;
    const auto &q = message->pose.orientation;
    const double norm = std::hypot(std::hypot(q.x, q.y), std::hypot(q.z, q.w));
    if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z) ||
        !std::isfinite(norm) || norm < 1e-12) {
      stable = 0;
      last_status = "pose feedback contains invalid position/quaternion";
      continue;
    }
    const double position_error = std::hypot(p.x-target.x, p.y-target.y, p.z-target.z);
    const double dot = std::abs((q.x*desired[0] + q.y*desired[1] + q.z*desired[2] + q.w*desired[3]) / norm);
    const double orientation_error = 2 * std::acos(std::clamp(dot, 0.0, 1.0));
    std::ostringstream status;
    status << "actual xyz=[" << p.x << ',' << p.y << ',' << p.z << "] target xyz=["
           << target.x << ',' << target.y << ',' << target.z << "] errors="
           << position_error*1000 << " mm," << orientation_error << " rad";
    last_status = status.str();
    stable = position_error <= position_tolerance_m && orientation_error <= orientation_tolerance_rad ?
      stable + 1 : 0;
    if (stable >= stable_samples) {
      if (reached_pose) {
        const double x = q.x/norm, y = q.y/norm, z = q.z/norm, w = q.w/norm;
        *reached_pose = {p.x, p.y, p.z,
          std::atan2(2*(w*x+y*z), 1-2*(x*x+y*y)),
          std::asin(std::clamp(2*(w*y-z*x), -1.0, 1.0)),
          std::atan2(2*(w*z+x*y), 1-2*(y*y+z*z))};
      }
      return DeviceResult::ok("pose arrival confirmed: " + last_status);
    }
  }
  return DeviceResult::fail("pose arrival timeout: " + last_status);
}

DeviceResult LeftArmMotionDevice::move_joints(
  const std::array<double, 7> &target,
  const ArmMotionOptions &options,
  const std::string &label)
{
  RCLCPP_INFO(
    node_->get_logger(), "waiting for left MoveJ service '%s' (timeout=%ld ms)",
    endpoint("left_arm/move_joint").c_str(),
    static_cast<long>(service_timeout_.count()));
  if (!move_j_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail(
      "left MoveJ service unavailable at " + endpoint("left_arm/move_joint") +
      suffix_label(label));
  }
  auto request = std::make_shared<MoveJ::Request>();
  request->joints.assign(target.begin(), target.end());
  request->speed = static_cast<float>(std::max(0.0, options.speed));
  request->acce = static_cast<float>(std::max(0.0, options.acceleration));
  request->block = options.block;
  auto future = move_j_client_->async_send_request(request);
  // Some driver/firmware combinations acknowledge a non-blocking command
  // only after the trajectory request has been accepted.  Keep the response
  // window long enough for that acknowledgement; joint-state settling is
  // checked separately by the control layer.
  const auto response_timeout = service_timeout_ + std::chrono::seconds(30);
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

DeviceResult LeftArmMotionDevice::forward_kinematics(
  const std::array<double, 7> &joints, CartesianPose &pose)
{
  if (!std::all_of(joints.begin(), joints.end(), [](double v) {return std::isfinite(v);})) {
    return DeviceResult::fail("non-finite left FK joint input");
  }
  if (!fk_client_->wait_for_service(service_timeout_)) {
    return DeviceResult::fail("left forward-kinematics service unavailable");
  }
  auto request = std::make_shared<ForwardKinematics::Request>();
  request->joints.assign(joints.begin(), joints.end());
  auto future = fk_client_->async_send_request(request);
  if (!response_ready(future, service_timeout_)) {
    fk_client_->remove_pending_request(future);
    return DeviceResult::fail("left forward-kinematics response timeout");
  }
  const auto response = future.get();
  if (!response->success) return DeviceResult::fail("left forward-kinematics request failed");
  const auto &p = response->position;
  const auto &r = response->euler;
  const std::array<double, 6> values{p.x, p.y, p.z, r.x, r.y, r.z};
  if (!std::all_of(values.begin(), values.end(), [](double v) {return std::isfinite(v);})) {
    return DeviceResult::fail("non-finite left forward-kinematics result");
  }
  pose = {p.x, p.y, p.z, r.x, r.y, r.z};
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
