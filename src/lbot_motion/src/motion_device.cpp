#include "lbot_motion/motion_device.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace lbot_motion {

namespace {
std::string arm_topic(Side side) { return side == Side::Left ? "left_arm" : "right_arm"; }
std::string hand_topic(Side side) { return side == Side::Left ? "left_hand" : "right_hand"; }
}  // namespace

Side side_from_string(const std::string &value)
{
  if (value == "left" || value == "l" || value == "left_arm") return Side::Left;
  return Side::Right;
}

std::string side_to_string(Side side) { return side == Side::Left ? "left" : "right"; }

ArmController::ArmController(const rclcpp::Node::SharedPtr &node, Side side,
                             double state_timeout_sec, double service_timeout_sec)
: node_(node), side_(side), state_timeout_sec_(state_timeout_sec),
  service_timeout_sec_(service_timeout_sec)
{
  const auto prefix = arm_topic(side_);
  state_sub_ = node_->create_subscription<sensor_msgs::msg::JointState>(
    prefix + "/joint_states", 10,
    [this](const sensor_msgs::msg::JointState::SharedPtr msg) { on_joint_state(msg); });
  move_client_ = node_->create_client<lbot_arm_interfaces::srv::MoveJ>(prefix + "/move_joint");
}

void ArmController::on_joint_state(const sensor_msgs::msg::JointState::SharedPtr msg)
{
  if (msg->position.size() < joints_.size()) return;
  std::copy_n(msg->position.begin(), joints_.size(), joints_.begin());
  state_ready_ = true;
}

bool ArmController::wait_for_state()
{
  const auto deadline = std::chrono::steady_clock::now() +
    std::chrono::duration_cast<std::chrono::steady_clock::duration>(
      std::chrono::duration<double>(state_timeout_sec_));
  while (rclcpp::ok() && !state_ready_ && std::chrono::steady_clock::now() < deadline) {
    rclcpp::spin_some(node_->get_node_base_interface());
    rclcpp::sleep_for(std::chrono::milliseconds(20));
  }
  return state_ready_;
}

bool ArmController::move_joints(const std::array<double, 7> &target,
                                const ArmMotionOptions &options,
                                const std::string &label)
{
  if (!move_client_->wait_for_service(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<double>(service_timeout_sec_)))) {
    RCLCPP_ERROR(node_->get_logger(), "%s arm MoveJ service is unavailable",
                 side_to_string(side_).c_str());
    return false;
  }
  auto request = std::make_shared<lbot_arm_interfaces::srv::MoveJ::Request>();
  request->joints.assign(target.begin(), target.end());
  request->speed = static_cast<float>(std::max(0.0, options.speed));
  request->acce = static_cast<float>(std::max(0.0, options.acceleration));
  request->block = options.block;
  RCLCPP_INFO(node_->get_logger(),
              "Sending %s MoveJ (speed=%.3f, acce=%.3f, block=%s)%s",
              side_to_string(side_).c_str(), options.speed, options.acceleration,
              options.block ? "true" : "false", label.empty() ? "" : " [timeline]");
  auto future = move_client_->async_send_request(request);
  if (!options.block) {
    // DDS sends the request immediately. The planner deliberately uses this
    // mode so events on the two arms and hand can share the same timestamp.
    return true;
  }
  const auto result = rclcpp::spin_until_future_complete(
    node_->get_node_base_interface(), future,
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<double>(service_timeout_sec_ + 30.0)));
  if (result != rclcpp::FutureReturnCode::SUCCESS || !future.get()->success) {
    RCLCPP_ERROR(node_->get_logger(), "MoveJ failed%s%s", label.empty() ? "" : ": ", label.c_str());
    return false;
  }
  RCLCPP_INFO(node_->get_logger(), "%s MoveJ accepted and completed", side_to_string(side_).c_str());
  return true;
}

HandController::HandController(const rclcpp::Node::SharedPtr &node, Side side)
{
  const auto prefix = hand_topic(side);
  speed_pub_ = node->create_publisher<std_msgs::msg::UInt8MultiArray>(prefix + "/set_l6_speed", 10);
  force_pub_ = node->create_publisher<std_msgs::msg::UInt8MultiArray>(prefix + "/set_l6_force", 10);
  position_pub_ = node->create_publisher<std_msgs::msg::UInt8MultiArray>(prefix + "/set_l6_joint", 10);
}

bool HandController::set_positions(const std::array<uint8_t, 6> &positions,
                                   uint8_t speed, uint8_t force, int repeat)
{
  std_msgs::msg::UInt8MultiArray speed_msg, force_msg, position_msg;
  speed_msg.data.assign(6, speed);
  force_msg.data.assign(6, force);
  position_msg.data.assign(positions.begin(), positions.end());
  for (int i = 0; i < std::max(1, repeat) && rclcpp::ok(); ++i) {
    speed_pub_->publish(speed_msg);
    force_pub_->publish(force_msg);
    position_pub_->publish(position_msg);
    if (i + 1 < std::max(1, repeat)) rclcpp::sleep_for(std::chrono::milliseconds(20));
  }
  return true;
}

MotionDevice::MotionDevice(const rclcpp::Node::SharedPtr &node,
                           double state_timeout_sec, double service_timeout_sec)
: left_arm_(node, Side::Left, state_timeout_sec, service_timeout_sec),
  right_arm_(node, Side::Right, state_timeout_sec, service_timeout_sec),
  left_hand_(node, Side::Left), right_hand_(node, Side::Right)
{}

ArmController &MotionDevice::arm(Side side) { return side == Side::Left ? left_arm_ : right_arm_; }
HandController &MotionDevice::hand(Side side) { return side == Side::Left ? left_hand_ : right_hand_; }
bool MotionDevice::wait_for_state(Side side) { return arm(side).wait_for_state(); }

bool MotionDevice::wait_for_states()
{
  return left_arm_.wait_for_state() && right_arm_.wait_for_state();
}

}  // namespace lbot_motion
