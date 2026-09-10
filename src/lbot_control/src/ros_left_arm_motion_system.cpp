#include "lbot_control/ros_left_arm_motion_system.hpp"

#include <cmath>
#include <thread>
#include <utility>

namespace lbot_control {

RosLeftArmMotionSystem::RosLeftArmMotionSystem(
  const rclcpp::Node::SharedPtr &node,
  RosLeftArmMotionConfig config)
: config_(std::move(config)),
  device_(std::make_shared<lbot_motion::LeftArmMotionDevice>(
      node, config_.robot_namespace, config_.state_timeout, config_.service_timeout))
{}

bool RosLeftArmMotionSystem::joints_within_limits(
  const std::array<double, 7> &joints) const
{
  for (std::size_t index = 0; index < joints.size(); ++index) {
    if (!std::isfinite(joints[index]) ||
      joints[index] < config_.left_joint_min[index] + config_.joint_limit_margin_rad ||
      joints[index] > config_.left_joint_max[index] - config_.joint_limit_margin_rad)
    {
      return false;
    }
  }
  return true;
}

bool RosLeftArmMotionSystem::finite_pose(const Pose6 &pose) const
{
  return std::isfinite(pose.x) && std::isfinite(pose.y) && std::isfinite(pose.z) &&
         std::isfinite(pose.roll) && std::isfinite(pose.pitch) && std::isfinite(pose.yaw);
}

lbot_motion::CartesianPose RosLeftArmMotionSystem::to_device_pose(const Pose6 &pose)
{
  return {pose.x, pose.y, pose.z, pose.roll, pose.pitch, pose.yaw};
}

MotionResult RosLeftArmMotionSystem::from_device_result(
  const lbot_motion::DeviceResult &result)
{
  return {result.success, result.message};
}

MotionResult RosLeftArmMotionSystem::validate_target(
  const PlannedTarget &target, std::array<double, 7> &seed)
{
  const std::array<const Pose6 *, 6> poses{{
    &target.pregrasp, &target.grasp, &target.lift,
    &target.slot_pre, &target.slot_release, &target.slot_retreat}};
  for (const auto *pose : poses) {
    if (!finite_pose(*pose)) {
      return MotionResult::fail(std::string("non-finite pose for ") + to_string(target.size));
    }
    auto result = solve_ik(*pose, seed);
    if (!result.success) {
      return MotionResult::fail(
        std::string("IK validation failed for ") + to_string(target.size) + ": " + result.message);
    }
  }
  return MotionResult::ok();
}

MotionResult RosLeftArmMotionSystem::prepare(const MotionPlan &plan)
{
  task_plan_prepared_ = false;
  const auto route = prepare_table_route();
  if (!route.success) return route;
  if (plan.frame_id != config_.base_frame) {
    return MotionResult::fail("motion plan is not expressed in the configured base frame");
  }
  if (!device_->wait_for_state()) return MotionResult::fail("left arm state is unavailable");
  auto seed = device_->left_joints();
  for (const auto &target : plan.targets) {
    const auto validation = validate_target(target, seed);
    if (!validation.success) return validation;
  }
  task_plan_prepared_ = true;
  return MotionResult::ok("table route and IK validation accepted the complete motion plan");
}

MotionResult RosLeftArmMotionSystem::prepare_table_route()
{
  route_prepared_ = false;
  task_plan_prepared_ = false;
  if (!config_.table_route_calibrated) {
    return MotionResult::fail("table joint route has not been calibrated");
  }
  if (!(std::isfinite(config_.joint_speed) && config_.joint_speed > 0.0 &&
    config_.joint_speed <= 1.0) ||
    !(std::isfinite(config_.joint_acceleration) && config_.joint_acceleration > 0.0 &&
    config_.joint_acceleration <= 1.0) ||
    !(std::isfinite(config_.waypoint_tolerance_rad) &&
    config_.waypoint_tolerance_rad > 0.0) ||
    config_.stable_joint_samples == 0 || config_.waypoint_timeout.count() <= 0)
  {
    return MotionResult::fail("table-route speed, tolerance, or timeout parameters are invalid");
  }
  TableRouteDefinition definition;
  definition.waypoints = config_.table_waypoints;
  const auto generated = build_table_route(definition);
  if (!generated.success) return MotionResult::fail(generated.message);

  for (const auto *segments : {&generated.route.enter, &generated.route.leave}) {
    for (const auto &segment : *segments) {
      if (!joints_within_limits(segment.start) || !joints_within_limits(segment.goal)) {
        return MotionResult::fail(segment.name + " violates the configured joint limits");
      }
    }
  }
  table_route_ = generated.route;
  route_prepared_ = true;
  return MotionResult::ok("table route generated and joint limits accepted");
}

MotionResult RosLeftArmMotionSystem::begin_route_motion()
{
  if (!route_prepared_) return MotionResult::fail("table route has not been prepared");
  motion_started_ = true;
  const auto result = set_hand({{0, 0, 0, 0, 0, 0}});
  if (result.success && config_.grip_settle.count() > 0) {
    std::this_thread::sleep_for(config_.grip_settle);
  }
  return result;
}

MotionResult RosLeftArmMotionSystem::finish_route_motion()
{
  if (!route_prepared_) return MotionResult::fail("table route has not been prepared");
  const auto result = set_hand(config_.hand_open);
  if (result.success && config_.grip_settle.count() > 0) {
    std::this_thread::sleep_for(config_.grip_settle);
  }
  return result;
}

MotionResult RosLeftArmMotionSystem::move_joints(
  const std::array<double, 7> &joints, const std::string &label)
{
  if (!joints_within_limits(joints)) return MotionResult::fail(label + " violates joint limits");
  const lbot_motion::ArmMotionOptions options{
    config_.joint_speed, config_.joint_acceleration, false};
  return from_device_result(device_->move_joints(joints, options, label));
}

MotionResult RosLeftArmMotionSystem::execute_joint_segment(
  const JointPathSegment &segment)
{
  const auto moved = move_joints(segment.goal, segment.name);
  if (!moved.success) return moved;
  if (!device_->wait_until_left_joints(
      segment.goal, config_.waypoint_tolerance_rad,
      config_.waypoint_timeout, config_.stable_joint_samples))
  {
    return MotionResult::fail("left arm did not settle at " + segment.name);
  }
  if (config_.waypoint_settle.count() > 0) {
    std::this_thread::sleep_for(config_.waypoint_settle);
  }
  return MotionResult::ok(segment.name);
}

MotionResult RosLeftArmMotionSystem::execute_joint_route(
  const std::vector<JointPathSegment> &segments)
{
  if (segments.empty()) return MotionResult::fail("joint route has no segments");
  for (const auto &segment : segments) {
    const auto result = execute_joint_segment(segment);
    if (!result.success) return result;
  }
  return MotionResult::ok("joint route completed");
}

MotionResult RosLeftArmMotionSystem::move_pose(const Pose6 &pose, const std::string &label)
{
  if (!finite_pose(pose)) return MotionResult::fail(label + " is invalid");
  const lbot_motion::ArmMotionOptions options{
    config_.joint_speed, config_.joint_acceleration, true};
  return from_device_result(device_->move_pose(to_device_pose(pose), options, label));
}

MotionResult RosLeftArmMotionSystem::move_linear(const Pose6 &pose, const std::string &label)
{
  if (!finite_pose(pose)) return MotionResult::fail(label + " is invalid");
  const lbot_motion::ArmMotionOptions options{
    config_.cartesian_speed, config_.cartesian_acceleration, true};
  return from_device_result(device_->move_linear(to_device_pose(pose), options, label));
}

MotionResult RosLeftArmMotionSystem::solve_ik(
  const Pose6 &pose, std::array<double, 7> &seed)
{
  std::array<double, 7> solution{};
  const auto result = device_->inverse_kinematics(to_device_pose(pose), seed, solution);
  if (!result.success) return from_device_result(result);
  if (!joints_within_limits(solution)) {
    return MotionResult::fail("inverse-kinematics result is too close to a joint limit");
  }
  seed = solution;
  return MotionResult::ok();
}

MotionResult RosLeftArmMotionSystem::set_hand(
  const std::array<uint8_t, 6> &positions)
{
  return from_device_result(
    device_->set_hand(positions, config_.hand_speed, config_.hand_force));
}

MotionResult RosLeftArmMotionSystem::execute(
  MotionStage stage, const PlannedTarget *target)
{
  const bool task_stage = stage == MotionStage::PickAndPlace;
  if (!route_prepared_) return MotionResult::fail("table route has not been prepared");
  if (task_stage && !task_plan_prepared_) {
    return MotionResult::fail("motion system has not accepted a complete task plan");
  }
  motion_started_ = true;
  switch (stage) {
    case MotionStage::MoveAboveTable:
      return execute_joint_route(table_route_.enter);
    case MotionStage::MoveToPregrasp:
      if (target == nullptr) return MotionResult::fail("pregrasp move requires a target");
      return move_pose(target->pregrasp, "calibration pregrasp");
    case MotionStage::Retract:
      return execute_joint_route(table_route_.leave);
    case MotionStage::PickAndPlace: {
      if (target == nullptr) return MotionResult::fail("pick-and-place requires a target");
      auto result = set_hand(config_.hand_open);
      if (!result.success) return result;
      result = move_pose(target->pregrasp, "pregrasp");
      if (!result.success) return result;
      result = move_linear(target->grasp, "vertical grasp descent");
      if (!result.success) return result;
      result = set_hand(config_.hand_closed[static_cast<std::size_t>(target->size)]);
      if (!result.success) return result;
      std::this_thread::sleep_for(config_.grip_settle);
      result = move_linear(target->lift, "vertical post-grasp lift");
      if (!result.success) return result;
      result = move_pose(target->slot_pre, "slot pre-place");
      if (!result.success) return result;
      result = move_linear(target->slot_release, "vertical slot descent");
      if (!result.success) return result;
      result = set_hand(config_.hand_open);
      if (!result.success) return result;
      std::this_thread::sleep_for(config_.grip_settle);
      result = move_linear(target->slot_retreat, "vertical slot retreat");
      if (!result.success) return result;
      // Keep this camera-visible pose while the coordinator checks whether
      // the nut is still in the source frame.  Returning above the frame is a
      // separate stage executed after that check.
      return MotionResult::ok("pick and place completed; ready for camera check");
    }
    case MotionStage::ReturnAboveTable: {
      if (config_.table_waypoints.empty()) {
        return MotionResult::fail("table route has no above-table waypoint");
      }
      const JointPathSegment return_above{
        "return_above_source_frame", device_->left_joints(),
        config_.table_waypoints.back().joints};
      return execute_joint_segment(return_above);
    }
  }
  return MotionResult::fail("unsupported motion stage");
}

void RosLeftArmMotionSystem::stop() noexcept
{
  route_prepared_ = false;
  task_plan_prepared_ = false;
  if (!motion_started_) return;
  try {
    device_->set_emergency_stop(true);
  } catch (...) {
    // stop() is a last-resort safety path and must never throw during cleanup.
  }
}

}  // namespace lbot_control
