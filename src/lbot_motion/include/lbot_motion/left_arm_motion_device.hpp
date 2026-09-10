#ifndef LBOT_MOTION__LEFT_ARM_MOTION_DEVICE_HPP_
#define LBOT_MOTION__LEFT_ARM_MOTION_DEVICE_HPP_

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include "lbot_arm_interfaces/srv/inverse_kinematics.hpp"
#include "lbot_arm_interfaces/srv/move_j.hpp"
#include "lbot_arm_interfaces/srv/move_jp.hpp"
#include "lbot_arm_interfaces/srv/move_l.hpp"
#include "lbot_arm_interfaces/srv/set_emergency.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

namespace lbot_motion {

struct CartesianPose
{
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double roll{0.0};
  double pitch{0.0};
  double yaw{0.0};
};

struct ArmMotionOptions
{
  double speed{0.1};
  double acceleration{0.1};
  bool block{true};
};

struct DeviceResult
{
  bool success{false};
  std::string message;

  static DeviceResult ok(const std::string &message = "") {return {true, message};}
  static DeviceResult fail(const std::string &message) {return {false, message};}
};

// Narrow ROS adapter for this competition task:
// - commands only the left arm and left O6 hand;
// - contains no task state machine or task-specific waypoint logic.
class LeftArmMotionDevice
{
public:
  LeftArmMotionDevice(
    const rclcpp::Node::SharedPtr &node,
    std::string robot_namespace = "/robot1",
    std::chrono::milliseconds state_timeout = std::chrono::milliseconds(3000),
    std::chrono::milliseconds service_timeout = std::chrono::milliseconds(30000));

  bool wait_for_state();
  std::array<double, 7> left_joints() const;
  bool wait_until_left_joints(
    const std::array<double, 7> &target,
    double tolerance_rad,
    std::chrono::milliseconds timeout,
    std::size_t stable_samples = 3);

  DeviceResult move_joints(
    const std::array<double, 7> &target,
    const ArmMotionOptions &options,
    const std::string &label = "");
  DeviceResult move_pose(
    const CartesianPose &target,
    const ArmMotionOptions &options,
    const std::string &label = "");
  DeviceResult move_linear(
    const CartesianPose &target,
    const ArmMotionOptions &options,
    const std::string &label = "");
  DeviceResult inverse_kinematics(
    const CartesianPose &target,
    const std::array<double, 7> &seed,
    std::array<double, 7> &solution);
  DeviceResult set_hand(
    const std::array<uint8_t, 6> &positions,
    uint8_t speed,
    uint8_t force,
    int repeat = 3);
  DeviceResult set_emergency_stop(bool emergency);

private:
  using MoveJ = lbot_arm_interfaces::srv::MoveJ;
  using MoveJP = lbot_arm_interfaces::srv::MoveJP;
  using MoveL = lbot_arm_interfaces::srv::MoveL;
  using InverseKinematics = lbot_arm_interfaces::srv::InverseKinematics;
  using SetEmergency = lbot_arm_interfaces::srv::SetEmergency;

  std::string endpoint(const std::string &suffix) const;
  void on_left_state(const sensor_msgs::msg::JointState::SharedPtr message);

  rclcpp::Node::SharedPtr node_;
  std::string robot_namespace_;
  std::chrono::milliseconds state_timeout_;
  std::chrono::milliseconds service_timeout_;

  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_state_sub_;
  rclcpp::Client<MoveJ>::SharedPtr move_j_client_;
  rclcpp::Client<MoveJP>::SharedPtr move_jp_client_;
  rclcpp::Client<MoveL>::SharedPtr move_l_client_;
  rclcpp::Client<InverseKinematics>::SharedPtr ik_client_;
  rclcpp::Client<SetEmergency>::SharedPtr emergency_client_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr hand_speed_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr hand_force_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr hand_position_pub_;

  mutable std::mutex state_mutex_;
  std::condition_variable state_cv_;
  std::array<double, 7> left_joints_{};
  bool left_state_ready_{false};
  std::uint64_t left_state_sequence_{0};
};

}  // namespace lbot_motion

#endif  // LBOT_MOTION__LEFT_ARM_MOTION_DEVICE_HPP_
