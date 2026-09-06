#ifndef LBOT_MOTION__MOTION_DEVICE_HPP_
#define LBOT_MOTION__MOTION_DEVICE_HPP_

#include <array>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "lbot_arm_interfaces/srv/move_j.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

namespace lbot_motion {

enum class Side { Left, Right };

Side side_from_string(const std::string &value);
std::string side_to_string(Side side);

struct ArmMotionOptions {
  double speed{0.2};
  double acceleration{0.2};
  bool block{true};
};

class ArmController {
public:
  ArmController(const rclcpp::Node::SharedPtr &node, Side side,
                double state_timeout_sec = 15.0,
                double service_timeout_sec = 10.0);

  bool wait_for_state();
  bool has_state() const { return state_ready_; }
  std::array<double, 7> current_joints() const { return joints_; }
  bool move_joints(const std::array<double, 7> &target,
                   const ArmMotionOptions &options,
                   const std::string &label = "");

private:
  void on_joint_state(const sensor_msgs::msg::JointState::SharedPtr msg);

  rclcpp::Node::SharedPtr node_;
  Side side_;
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr state_sub_;
  rclcpp::Client<lbot_arm_interfaces::srv::MoveJ>::SharedPtr move_client_;
  std::array<double, 7> joints_{};
  bool state_ready_{false};
  double state_timeout_sec_{15.0};
  double service_timeout_sec_{10.0};
};

class HandController {
public:
  HandController(const rclcpp::Node::SharedPtr &node, Side side);

  bool set_positions(const std::array<uint8_t, 6> &positions,
                    uint8_t speed = 250, uint8_t force = 250,
                    int repeat = 3);

private:
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr speed_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr force_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr position_pub_;
};

class MotionDevice {
public:
  explicit MotionDevice(const rclcpp::Node::SharedPtr &node,
                        double state_timeout_sec = 15.0,
                        double service_timeout_sec = 10.0);

  ArmController &arm(Side side);
  HandController &hand(Side side);
  bool wait_for_state(Side side);
  bool wait_for_states();

private:
  ArmController left_arm_;
  ArmController right_arm_;
  HandController left_hand_;
  HandController right_hand_;
};

}  // namespace lbot_motion

#endif
