#ifndef LBOT_CONTROL__ROS_LEFT_ARM_MOTION_SYSTEM_HPP_
#define LBOT_CONTROL__ROS_LEFT_ARM_MOTION_SYSTEM_HPP_

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "lbot_control/motion_system.hpp"
#include "lbot_control/table_route.hpp"
#include "lbot_motion/left_arm_motion_device.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lbot_control {

struct RosLeftArmMotionConfig
{
  std::string robot_namespace{"/robot1"};
  std::string base_frame{"base_torso_root"};

  // Keeping this false prevents placeholder arrays from becoming executable.
  bool table_route_calibrated{false};
  std::vector<JointWaypoint> table_waypoints;

  std::array<double, 7> left_joint_min{
    -2.91, -3.20, -2.69, -2.05, -2.69, -1.59, -1.59};
  std::array<double, 7> left_joint_max{
    2.91, 0.07, 2.69, 2.05, 2.69, 1.59, 1.59};
  double joint_limit_margin_rad{0.05};

  double joint_speed{0.08};
  double joint_acceleration{0.08};
  double waypoint_tolerance_rad{0.03};
  std::size_t stable_joint_samples{3};
  std::chrono::milliseconds waypoint_timeout{35000};
  std::chrono::milliseconds waypoint_settle{200};
  double cartesian_speed{0.05};
  double cartesian_acceleration{0.05};
  std::chrono::milliseconds state_timeout{3000};
  std::chrono::milliseconds service_timeout{5000};
  std::chrono::milliseconds grip_settle{500};

  std::array<uint8_t, 6> hand_open{{128, 128, 128, 128, 128, 128}};
  std::array<std::array<uint8_t, 6>, 3> hand_closed{};
  uint8_t hand_speed{80};
  uint8_t hand_force{60};
};

// Converts task-level phases into calls to the task-agnostic lbot_motion
// adapter. This is the only lbot_control class that depends on lbot_motion.
class RosLeftArmMotionSystem final : public MotionSystem
{
public:
  RosLeftArmMotionSystem(
    const rclcpp::Node::SharedPtr &node,
    RosLeftArmMotionConfig config);

  MotionResult prepare_table_route();
  MotionResult prepare(const MotionPlan &plan) override;
  MotionResult execute(MotionStage stage, const PlannedTarget *target = nullptr) override;
  void stop() noexcept override;

private:
  bool joints_within_limits(const std::array<double, 7> &joints) const;
  bool finite_pose(const Pose6 &pose) const;
  MotionResult validate_target(const PlannedTarget &target, std::array<double, 7> &seed);
  MotionResult execute_joint_segment(const JointPathSegment &segment);
  MotionResult execute_joint_route(const std::vector<JointPathSegment> &segments);

  MotionResult move_joints(const std::array<double, 7> &joints, const std::string &label);
  MotionResult move_pose(const Pose6 &pose, const std::string &label);
  MotionResult move_linear(const Pose6 &pose, const std::string &label);
  MotionResult solve_ik(const Pose6 &pose, std::array<double, 7> &seed);
  MotionResult set_hand(const std::array<uint8_t, 6> &positions);
  static lbot_motion::CartesianPose to_device_pose(const Pose6 &pose);
  static MotionResult from_device_result(const lbot_motion::DeviceResult &result);

  RosLeftArmMotionConfig config_;
  std::shared_ptr<lbot_motion::LeftArmMotionDevice> device_;
  TableRoutePlan table_route_;
  bool route_prepared_{false};
  bool task_plan_prepared_{false};
  bool motion_started_{false};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__ROS_LEFT_ARM_MOTION_SYSTEM_HPP_
