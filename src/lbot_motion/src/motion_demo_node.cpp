#include <memory>
#include <string>

#include "lbot_motion/camera_motion_plan.hpp"
#include "rclcpp/rclcpp.hpp"

class MotionDemoNode : public rclcpp::Node {
public:
  MotionDemoNode() : Node("motion_demo_node")
  {
    arm_name_ = declare_parameter("arm", std::string("right"));
    motion_ = declare_parameter("motion", std::string("extend"));
    speed_ = declare_parameter("speed", 0.2);
    acceleration_ = declare_parameter("acce", 0.2);
    state_timeout_ = declare_parameter("state_timeout", 15.0);
    service_timeout_ = declare_parameter("service_timeout", 10.0);
    hand_enabled_ = declare_parameter("hand_enabled", true);
    hand_speed_ = declare_parameter("hand_speed", 250);
    hand_force_ = declare_parameter("hand_force", 250);
    hand_close_ = declare_parameter("hand_close_position", 0);
    hand_open_ = declare_parameter("hand_open_position", 255);
  }

  bool run()
  {
    if (arm_name_ != "left" && arm_name_ != "right" && arm_name_ != "l" &&
        arm_name_ != "r" && arm_name_ != "left_arm" && arm_name_ != "right_arm") {
      RCLCPP_ERROR(get_logger(), "arm must be left or right");
      return false;
    }
    const auto side = lbot_motion::side_from_string(arm_name_);
    auto node = shared_from_this();
    auto device = std::make_shared<lbot_motion::MotionDevice>(node, state_timeout_, service_timeout_);
    lbot_motion::MotionPlanner planner(device);
    lbot_motion::CameraMotionOptions options;
    options.speed = speed_;
    options.acceleration = acceleration_;
    options.hand_enabled = hand_enabled_;
    options.hand_speed = hand_speed_;
    options.hand_force = hand_force_;
    options.hand_close_position = hand_close_;
    options.hand_open_position = hand_open_;
    try {
      planner.add(lbot_motion::make_camera_motion_plan(
        side, lbot_motion::camera_motion_mode_from_string(motion_), options));
    } catch (const std::exception &error) {
      RCLCPP_ERROR(get_logger(), "%s", error.what());
      return false;
    }
    RCLCPP_INFO(get_logger(), "Running %s timeline on %s arm", motion_.c_str(),
                lbot_motion::side_to_string(side).c_str());
    return planner.execute();
  }

private:
  std::string arm_name_, motion_;
  double speed_{0.2}, acceleration_{0.2}, state_timeout_{15.0}, service_timeout_{10.0};
  int hand_speed_{250}, hand_force_{250}, hand_close_{0}, hand_open_{255};
  bool hand_enabled_{true};
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MotionDemoNode>();
  const bool success = node->run();
  rclcpp::shutdown();
  return success ? 0 : 1;
}
