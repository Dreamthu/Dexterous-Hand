#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "lbot_arm_interfaces/srv/move_j.hpp"

using namespace std::chrono_literals;
using MoveJ = lbot_arm_interfaces::srv::MoveJ;

class MotionTestNode : public rclcpp::Node
{
public:
  MotionTestNode() : Node("motion_test_node")
  {
    left_state_sub_ = create_subscription<sensor_msgs::msg::JointState>(
      "left_arm/joint_states", 10,
      [this](sensor_msgs::msg::JointState::SharedPtr msg) {
        if (msg->position.size() == 7) {
          left_joints_ = msg->position;
          left_state_ready_ = true;
        }
      });

    right_state_sub_ = create_subscription<sensor_msgs::msg::JointState>(
      "right_arm/joint_states", 10,
      [this](sensor_msgs::msg::JointState::SharedPtr msg) {
        if (msg->position.size() == 7) {
          right_joints_ = msg->position;
          right_state_ready_ = true;
        }
      });

    left_movej_ = create_client<MoveJ>("left_arm/move_joint");
    right_movej_ = create_client<MoveJ>("right_arm/move_joint");
  }

  bool run()
  {
    RCLCPP_INFO(get_logger(), "Waiting for current joint states...");
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    while (rclcpp::ok() && (!left_state_ready_ || !right_state_ready_)) {
      rclcpp::spin_some(get_node_base_interface());
      if (std::chrono::steady_clock::now() >= deadline) {
        RCLCPP_ERROR(get_logger(), "Timed out waiting for joint states");
        return false;
      }
      rclcpp::sleep_for(100ms);
    }

    RCLCPP_INFO(get_logger(), "Waiting for MoveJ services...");
    if (!left_movej_->wait_for_service(10s) || !right_movej_->wait_for_service(10s)) {
      RCLCPP_ERROR(get_logger(), "MoveJ service is not available");
      return false;
    }

    // Move only 0.1 rad on joint 1, then restore the measured pose.
    const auto left_home = left_joints_;
    const auto right_home = right_joints_;
    auto left_target = left_home;
    auto right_target = right_home;
    left_target[0] += 0.1;
    right_target[0] += 0.1;

    RCLCPP_WARN(get_logger(), "Starting low-speed motion test: joint 1 +/- 0.1 rad");
    if (!move(left_movej_, left_target, "left arm forward") ||
        !move(left_movej_, left_home, "left arm restore") ||
        !move(right_movej_, right_target, "right arm forward") ||
        !move(right_movej_, right_home, "right arm restore")) {
      return false;
    }

    RCLCPP_INFO(get_logger(), "Motion test completed successfully");
    return true;
  }

private:
  bool move(
    const rclcpp::Client<MoveJ>::SharedPtr &client,
    const std::vector<double> &joints,
    const std::string &label)
  {
    auto request = std::make_shared<MoveJ::Request>();
    request->joints.assign(joints.begin(), joints.end());
    request->speed = 0.1f;
    request->acce = 0.1f;
    request->block = true;

    RCLCPP_INFO(get_logger(), "Executing %s", label.c_str());
    auto future = client->async_send_request(request);
    const auto ret = rclcpp::spin_until_future_complete(
      get_node_base_interface(), future, 30s);
    if (ret != rclcpp::FutureReturnCode::SUCCESS || !future.get()->success) {
      RCLCPP_ERROR(get_logger(), "%s failed", label.c_str());
      return false;
    }
    return true;
  }

  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_state_sub_;
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr right_state_sub_;
  rclcpp::Client<MoveJ>::SharedPtr left_movej_;
  rclcpp::Client<MoveJ>::SharedPtr right_movej_;
  std::vector<double> left_joints_;
  std::vector<double> right_joints_;
  bool left_state_ready_{false};
  bool right_state_ready_{false};
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MotionTestNode>();
  const bool success = node->run();
  rclcpp::shutdown();
  return success ? 0 : 1;
}
