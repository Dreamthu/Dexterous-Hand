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
#include "lbot_control/motion_plan.hpp"
#include "lbot_control/table_route.hpp"
#include "lbot_control/moveit_transfer_planner.hpp"
#include "lbot_motion/left_arm_motion_device.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lbot_control {

struct RosLeftArmMotionConfig
{
  std::string robot_namespace{"/robot1"};
  std::string base_frame{"base_link"};

  // Keeping this false prevents placeholder arrays from becoming executable.
  bool table_route_calibrated{false};
  std::vector<JointWaypoint> table_waypoints;

  std::array<double, 7> left_joint_min{
    -2.91, -0.07, -2.69, -2.05, -2.69, -1.59, -1.59};
  std::array<double, 7> left_joint_max{
    2.91, 3.20, 2.69, 2.05, 2.69, 1.59, 1.59};
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
  std::chrono::milliseconds service_timeout{30000};
  std::chrono::milliseconds grip_settle{500};
  // The standalone table-route bring-up is a joint-route diagnostic. Keep
  // hand commands opt-in so a missing/unavailable hand topic cannot prevent
  // the arm route from being tested.
  bool route_control_hand{false};

  std::array<uint8_t, 6> hand_open{{255, 40, 255, 255, 255, 255}};
  std::array<std::array<uint8_t, 6>, 3> hand_closed{};
  uint8_t hand_speed{80};
  uint8_t hand_force{60};
  bool approach_grasp_enabled{false};
  double approach_grasp_z_m{-0.370};
  std::string approach_grasp_z_frame{"palm"};
  double pose_tolerance_m{0.001};
  double pose_tolerance_rad{0.02};
  std::chrono::milliseconds pose_arrival_timeout{10000};
  std::size_t pose_stable_samples{3};
  std::array<uint8_t, 6> approach_hand_ready{{240, 30, 180, 180, 180, 180}};
  std::array<uint8_t, 6> approach_hand_close{{0, 30, 0, 0, 0, 0}};
  bool approach_place_enabled{false};
  double approach_place_lift_m{0.12};
  std::vector<JointWaypoint> approach_place_waypoints;
  double approach_place_max_orientation_change_rad{0.35};
  std::chrono::milliseconds approach_place_planning_timeout{15000};
  bool approach_place_release_enabled{true};
  std::string slot_transfer_planner{"sdk"};
  double joint_follow_rate_hz{50.0};
  double joint_follow_start_tolerance_rad{0.005};
  double joint_follow_tracking_tolerance_rad{0.08};
  double joint_follow_goal_tolerance_rad{0.005};
  std::chrono::milliseconds joint_follow_feedback_timeout{200};
  std::chrono::milliseconds joint_follow_max_lateness{60};
  double joint_follow_settle_tolerance_rad{0.001};
  std::chrono::milliseconds joint_follow_settle_duration{500};
  std::chrono::milliseconds joint_follow_settle_timeout{5000};
  int joint_follow_start_replans{2};
  std::array<uint8_t, 6> approach_hand_release{{240, 30, 180, 180, 180, 180}};
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
  MotionResult capture_transfer_obstacles();
  MotionResult begin_route_motion();
  MotionResult finish_route_motion();
  // Task exit: open the hand after actual motion, even if slot release was disabled.
  MotionResult finish_task();
  MotionResult prepare(const MotionPlan &plan) override;
  // execute=false previews the target from the taught waypoint, without IK.
  // execute=true requires that waypoint, then sends one controller-planned
  // MoveJP, then the configured grasp/place sequence. Approach and placement
  // can have separate Z/RPY references. With placement waypoints, freeze the
  // measured lift Z and search nearby RPY after reaching the last joint waypoint.
  // slot_transfer_planner selects SDK MoveJP or local MoveIt + joint_follow
  // for all post-lift transfers. Initial enter / vertical motions are unchanged.
  MotionResult approach_at_table_height(
    const Pose6 &nut, const MotionPlanOptions &options, bool execute = false,
    const Pose6 *slot = nullptr);
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
  MotionResult plan_slot_transfer(
    const Pose6 &lifted, const Pose6 &slot, const MotionPlanOptions &options,
    const std::array<double, 7> &seed, Pose6 &goal);
  MotionResult wait_for_stationary_left_joints(std::array<double, 7> &joints);
  MotionResult execute_moveit_transfer(MoveItTransferPlan &plan);
  MotionResult set_hand(const std::array<uint8_t, 6> &positions);
  MotionResult open_hand_for_cleanup(bool settle);
  static lbot_motion::CartesianPose to_device_pose(const Pose6 &pose);
  static MotionResult from_device_result(const lbot_motion::DeviceResult &result);

  rclcpp::Node::SharedPtr node_;
  RosLeftArmMotionConfig config_;
  std::shared_ptr<lbot_motion::LeftArmMotionDevice> device_;
  std::shared_ptr<MoveItTransferPlanner> moveit_planner_;
  TableRoutePlan table_route_;
  bool route_prepared_{false};
  bool task_plan_prepared_{false};
  bool motion_started_{false};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__ROS_LEFT_ARM_MOTION_SYSTEM_HPP_
