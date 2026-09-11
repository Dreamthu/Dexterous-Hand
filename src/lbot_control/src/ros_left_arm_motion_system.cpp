#include "lbot_control/ros_left_arm_motion_system.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <thread>
#include <utility>

namespace lbot_control {

RosLeftArmMotionSystem::RosLeftArmMotionSystem(
  const rclcpp::Node::SharedPtr &node,
  RosLeftArmMotionConfig config)
: node_(node), config_(std::move(config)),
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
  const std::array<const char *, 6> names{{
    "pregrasp", "grasp", "lift", "slot_pre", "slot_release", "slot_retreat"}};
  for (std::size_t index = 0; index < poses.size(); ++index) {
    const auto *pose = poses[index];
    if (!finite_pose(*pose)) {
      return MotionResult::fail(std::string("non-finite pose for ") + to_string(target.size));
    }
    auto result = solve_ik(*pose, seed);
    if (!result.success) {
      std::ostringstream message;
      message << "IK validation failed for " << to_string(target.size) << '/' << names[index]
              << " in " << config_.base_frame << ": xyz=[" << pose->x << ',' << pose->y
              << ',' << pose->z << "] rpy=[" << pose->roll << ',' << pose->pitch << ','
              << pose->yaw << "]; " << result.message;
      return MotionResult::fail(message.str());
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

MotionResult RosLeftArmMotionSystem::approach_at_table_height(
  const Pose6 &nut, const MotionPlanOptions &options, bool execute, const Pose6 *slot)
{
  if (config_.approach_place_enabled && (!config_.approach_grasp_enabled || !slot ||
      !finite_pose(*slot) || !std::isfinite(config_.approach_place_lift_m) ||
      config_.approach_place_lift_m <= 0.0 || config_.approach_place_lift_m > 2.0)) {
    return MotionResult::fail("placement requires grasp, a saved slot, and a valid positive lift");
  }
  if (!route_prepared_ || config_.table_waypoints.empty()) {
    return MotionResult::fail("table route has not been prepared");
  }
  if (options.base_frame != config_.base_frame) {
    return MotionResult::fail("horizontal approach is not in the configured base frame");
  }
  auto start_joints = config_.table_waypoints.back().joints;
  if (execute) {
    if (!device_->wait_for_fresh_state()) return MotionResult::fail("fresh left arm state is unavailable");
    start_joints = device_->left_joints();
    for (std::size_t i = 0; i < start_joints.size(); ++i) {
      if (!std::isfinite(start_joints[i]) ||
          std::abs(start_joints[i] - config_.table_waypoints.back().joints[i]) >
          config_.waypoint_tolerance_rad) {
        return MotionResult::fail("left arm must first settle at the taught above-table waypoint");
      }
    }
  }
  if (!joints_within_limits(start_joints)) {
    return MotionResult::fail("horizontal approach start violates joint limits");
  }
  lbot_motion::CartesianPose actual;
  const auto fk = device_->forward_kinematics(start_joints, actual);
  if (!fk.success) return from_device_result(fk);
  const Pose6 start{actual.x, actual.y, actual.z, actual.roll, actual.pitch, actual.yaw};
  const auto approach = build_horizontal_approach(start, nut, options);
  if (!approach.success) return MotionResult::fail(approach.message);
  const auto &goal = approach.goal;
  HorizontalApproachResult vertical;
  if (config_.approach_grasp_enabled) {
    if (config_.approach_grasp_z_frame == "arm_tip") {
      vertical = {true, "direct Arm_Tip Z target", goal};
      vertical.goal.z = config_.approach_grasp_z_m;
      if (!finite_pose(vertical.goal)) return MotionResult::fail("vertical Arm_Tip target is invalid");
    } else if (config_.approach_grasp_z_frame == "palm") {
      vertical = build_vertical_tcp_target(goal, config_.approach_grasp_z_m, options);
    } else {
      return MotionResult::fail("approach_grasp_z_frame must be arm_tip or palm");
    }
    if (!vertical.success) return MotionResult::fail(vertical.message);
    if (std::abs(vertical.goal.z - goal.z) > 2.0) {
      return MotionResult::fail("vertical move exceeds 2 m");
    }
    RCLCPP_INFO(node_->get_logger(),
      "post-approach sequence: ready hand, %s z=%.6f m (Arm_Tip z=%.6f m), close after pose feedback; %s",
      config_.approach_grasp_z_frame.c_str(), config_.approach_grasp_z_m,
      vertical.goal.z, execute ? "execution enabled" : "preview only");
  }
  if (options.approach_target_is_tcp && options.approach_reference_z_rpy) {
    RCLCPP_INFO(node_->get_logger(),
      "palm TCP target in %s: xyz=[%.6f,%.6f,%.6f]; "
      "Arm_Tip-local offset=[%.6f,%.6f,%.6f]",
      config_.base_frame.c_str(), nut.x, nut.y, (*options.approach_reference_z_rpy)[0],
      options.tcp_offset_x_m, options.tcp_offset_y_m, options.tcp_offset_z_m);
  }
  RCLCPP_INFO(node_->get_logger(),
    "MoveJP target (%s, %s): xyz=[%.4f,%.4f,%.4f] -> [%.4f,%.4f,%.4f], "
    "target rpy=[%.4f,%.4f,%.4f]",
    execute ? "actual reached pose" : "taught waypoint, validation only",
    options.approach_reference_z_rpy ?
      (options.approach_target_is_tcp ? "palm target -> Arm_Tip" : "saved web z/RPY + nut x/y") :
      "preserve reached z/RPY",
    start.x, start.y, start.z, goal.x, goal.y, goal.z, goal.roll, goal.pitch, goal.yaw);
  // Send the final pose to the controller's MoveJP planner. Do not impose
  // a separately sampled Cartesian IK path on this joint-interpolated move.
  const double distance = std::hypot(goal.x - start.x, goal.y - start.y, goal.z - start.z);
  if (distance > 2.0) return MotionResult::fail("table-height approach exceeds 2 m");
  if (config_.approach_place_enabled) {
    auto lift = vertical.goal;
    lift.z += config_.approach_place_lift_m;
    auto placement_options = options;
    if (!config_.approach_place_waypoints.empty()) placement_options.approach_place_reference_z_rpy.reset();
    const auto transfer = build_slot_transfer_target(lift, *slot, placement_options);
    if (!transfer.success) return MotionResult::fail(transfer.message);
    RCLCPP_INFO(node_->get_logger(),
      "placement preview: lift Arm_Tip by %.6f m to z=%.6f; %s xyz=[%.6f,%.6f,%.6f] "
      "rpy=[%.6f,%.6f,%.6f]; placement Z/RPY source=%s",
      config_.approach_place_lift_m, lift.z,
      config_.slot_transfer_planner == "moveit" ? "MoveIt + joint_follow" : "MoveJP",
      transfer.goal.x, transfer.goal.y, transfer.goal.z,
      transfer.goal.roll, transfer.goal.pitch, transfer.goal.yaw,
      placement_options.approach_place_reference_z_rpy ? "independent configured placement pose" : "measured lift pose");
    for (std::size_t index = 0; index < config_.approach_place_waypoints.size(); ++index) {
      const auto &waypoint = config_.approach_place_waypoints[index];
      const auto &q = waypoint.joints;
      RCLCPP_INFO(node_->get_logger(),
        "after lift: %s waypoint %zu/%zu (%s) [%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f]; "
        "after all waypoints, plan slot transfer from last reached joints; "
        "measured lift Z fixed, RPY search <= %.3f rad; release=%s",
        config_.slot_transfer_planner == "moveit" ? "MoveIt + joint_follow" : "MoveJ",
        index + 1, config_.approach_place_waypoints.size(), waypoint.name.c_str(),
        q[0], q[1], q[2], q[3], q[4], q[5], q[6], config_.approach_place_max_orientation_change_rad,
        config_.approach_place_release_enabled ? "true" : "false");
    }
  }
  if (!execute) {
    return MotionResult::ok(
      "MoveJP target computed; no motion sent; controller reachability/path not validated");
  }
  if (!device_->wait_for_fresh_state()) {
    return MotionResult::fail("fresh left arm state is unavailable before MoveJP");
  }
  const auto current = device_->left_joints();
  for (std::size_t i = 0; i < current.size(); ++i) {
    if (!std::isfinite(current[i]) || std::abs(current[i] - start_joints[i]) >
        config_.waypoint_tolerance_rad) {
      return MotionResult::fail("left arm moved while preparing the MoveJP target");
    }
  }
  motion_started_ = true;
  RCLCPP_INFO(node_->get_logger(),
    "sending saved LARGE target through left_arm/move_pose (MoveJP, block=true); "
    "controller plans the path; no separate IK requests");
  auto result = move_pose(goal, "approach above saved LARGE nut");
  if (!result.success || !config_.approach_grasp_enabled) return result;
  Pose6 reached;
  auto confirm_pose = [&](const Pose6 &target, const char *stage) {
    RCLCPP_INFO(node_->get_logger(), "waiting for actual pose after %s (position tolerance=%.3f mm)",
      stage, config_.pose_tolerance_m * 1000);
    lbot_motion::CartesianPose measured;
    const auto checked = device_->wait_until_left_pose(to_device_pose(target), config_.base_frame,
      config_.pose_tolerance_m, config_.pose_tolerance_rad, config_.pose_arrival_timeout,
      config_.pose_stable_samples, &measured);
    if (checked.success) {
      reached = {measured.x, measured.y, measured.z, measured.roll, measured.pitch, measured.yaw};
      RCLCPP_INFO(node_->get_logger(), "%s", checked.message.c_str());
    }
    return from_device_result(checked);
  };
  result = confirm_pose(goal, "MoveJP approach");
  if (!result.success) return result;
  auto hand_step = [&](const std::array<uint8_t, 6> &values, const char *label) {
    RCLCPP_INFO(node_->get_logger(), "%s: [%u,%u,%u,%u,%u,%u]", label,
      static_cast<unsigned>(values[0]), static_cast<unsigned>(values[1]),
      static_cast<unsigned>(values[2]), static_cast<unsigned>(values[3]),
      static_cast<unsigned>(values[4]), static_cast<unsigned>(values[5]));
    const auto hand_result = set_hand(values);
    if (hand_result.success && config_.grip_settle.count() > 0) {
      std::this_thread::sleep_for(config_.grip_settle);
    }
    return hand_result;
  };
  result = hand_step(config_.approach_hand_ready, "LARGE hand ready after approach");
  if (!result.success) return result;
  RCLCPP_INFO(node_->get_logger(),
    "vertical MoveL: %s z=%.6f m, Arm_Tip z %.6f -> %.6f (delta=%.3f mm); X/Y/RPY unchanged",
    config_.approach_grasp_z_frame.c_str(), config_.approach_grasp_z_m,
    goal.z, vertical.goal.z, (vertical.goal.z-goal.z)*1000);
  result = move_linear(vertical.goal, "LARGE vertical move before closing hand");
  if (!result.success) return result;
  result = confirm_pose(vertical.goal, "vertical MoveL before hand close");
  if (!result.success) return result;
  result = hand_step(config_.approach_hand_close, "LARGE hand close after vertical move");
  if (!result.success || !config_.approach_place_enabled) return result;
  // Re-read settled feedback after closing so the 12 cm is relative to the
  // actual Arm_Tip position, including any small settling error.
  result = confirm_pose(vertical.goal, "hand close before lift");
  if (!result.success) return result;
  auto lift = reached;
  lift.z += config_.approach_place_lift_m;
  if (!finite_pose(lift)) return MotionResult::fail("lift goal is non-finite");
  RCLCPP_INFO(node_->get_logger(),
    "LARGE lift MoveL: actual Arm_Tip z %.6f -> %.6f (+%.3f mm); X/Y/RPY unchanged",
    reached.z, lift.z, config_.approach_place_lift_m*1000);
  result = move_linear(lift, "LARGE lift after closing hand");
  if (!result.success) return result;
  result = confirm_pose(lift, "lift MoveL before slot transfer");
  if (!result.success) return result;
  // Freeze this BEFORE all joint waypoints, whose heights and orientations differ.
  const Pose6 lifted = reached;
  auto transfer = build_slot_transfer_target(lifted, *slot, options);
  MoveItTransferPlan planned_transfer;
  if (!config_.approach_place_waypoints.empty()) {
    std::array<double, 7> seed{};
    for (std::size_t index = 0; index < config_.approach_place_waypoints.size(); ++index) {
      const auto &waypoint = config_.approach_place_waypoints[index];
      const auto label = "LARGE post-lift transfer waypoint " + std::to_string(index + 1) + "/" +
        std::to_string(config_.approach_place_waypoints.size()) + " (" + waypoint.name + ")";
      if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh state before " + label);
      RCLCPP_INFO(node_->get_logger(), "starting %s", label.c_str());
      if (config_.slot_transfer_planner == "moveit") {
        result = wait_for_stationary_left_joints(seed);
        if (!result.success) return result;
        try {
          const auto goal = moveit_planner_->forward_kinematics(waypoint.joints);
          auto waypoint_plan = moveit_planner_->plan(seed, {goal},
            config_.approach_place_planning_timeout.count()/1000., &waypoint.joints);
          if (!waypoint_plan.success) return MotionResult::fail(label + ": " + waypoint_plan.message);
          RCLCPP_INFO(node_->get_logger(), "%s: %s", label.c_str(), waypoint_plan.message.c_str());
          result = execute_moveit_transfer(waypoint_plan);
        } catch (const std::exception &error) {
          return MotionResult::fail(label + ": MoveIt planning exception: " + error.what());
        }
      } else {
        result = execute_joint_segment({label, device_->left_joints(), waypoint.joints});
      }
      if (!result.success) return result;
      if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh state after " + label);
      seed = device_->left_joints();
      for (std::size_t i = 0; i < seed.size(); ++i) {
        if (!std::isfinite(seed[i]) || std::abs(seed[i] - waypoint.joints[i]) >
            config_.waypoint_tolerance_rad) return MotionResult::fail(label + " is no longer settled");
      }
      RCLCPP_INFO(node_->get_logger(), "reached %s", label.c_str());
    }
    RCLCPP_INFO(node_->get_logger(),
      "all transfer waypoints reached; planning from last reached joints with frozen lift "
      "Arm_Tip z=%.6f, rpy=[%.6f,%.6f,%.6f]",
      lifted.z, lifted.roll, lifted.pitch, lifted.yaw);
    if (config_.slot_transfer_planner == "sdk") {
      Pose6 selected;
      result = plan_slot_transfer(lifted, *slot, options, seed, selected);
      if (!result.success) return result;
      transfer = {true, result.message, selected};
    }
    if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh state after transfer planning");
    const auto current_joints = device_->left_joints();
    for (std::size_t i = 0; i < seed.size(); ++i) {
      if (!std::isfinite(current_joints[i]) || std::abs(current_joints[i] - seed[i]) >
          config_.waypoint_tolerance_rad) return MotionResult::fail("left arm moved during transfer planning");
    }
  }
  if (config_.slot_transfer_planner == "moveit") {
    std::array<double, 7> seed{};
    result = wait_for_stationary_left_joints(seed);
    if (!result.success) return result;
    try {
      const auto hint = moveit_planner_->forward_kinematics(seed);
      planned_transfer = moveit_planner_->plan(seed,
        build_slot_transfer_candidates(lifted, *slot, options, config_.approach_place_max_orientation_change_rad, &hint),
        config_.approach_place_planning_timeout.count()/1000.);
    } catch (const std::exception &error) {
      return MotionResult::fail(std::string("MoveIt planning exception: ") + error.what());
    }
    if (!planned_transfer.success) return MotionResult::fail(planned_transfer.message);
    transfer = {true, planned_transfer.message, planned_transfer.goal};
    RCLCPP_INFO(node_->get_logger(), "%s", planned_transfer.message.c_str());
    RCLCPP_INFO(node_->get_logger(), "selected orientation change from lift: %.3f rad (%.2f deg); frozen Arm_Tip Z=%.6f",
      orientation_distance(lifted, planned_transfer.goal), orientation_distance(lifted, planned_transfer.goal)*180./3.141592653589793, lifted.z);
  }
  if (!transfer.success) return MotionResult::fail(transfer.message);
  RCLCPP_INFO(node_->get_logger(),
    "LARGE slot target: saved slot xy=[%.6f,%.6f], Arm_Tip xyz=[%.6f,%.6f,%.6f] "
    "rpy=[%.6f,%.6f,%.6f]; Z/RPY source=%s; palm XY compensation=%s",
    slot->x, slot->y, transfer.goal.x, transfer.goal.y, transfer.goal.z,
    transfer.goal.roll, transfer.goal.pitch, transfer.goal.yaw,
    config_.slot_transfer_planner == "moveit" || !config_.approach_place_waypoints.empty() ? "frozen measured lift Z, selected RPY" :
      (options.approach_place_reference_z_rpy ? "independent configured placement pose" : "measured lift pose"),
    options.approach_target_is_tcp ? "true" : "false");
  result = config_.slot_transfer_planner == "moveit" ? execute_moveit_transfer(planned_transfer) :
    move_pose(transfer.goal, "LARGE transfer to saved slot");
  if (!result.success) return result;
  result = confirm_pose(transfer.goal, "slot transfer");
  if (!result.success) return result;
  if (!config_.approach_place_release_enabled) {
    return MotionResult::ok("slot reached; keeping hand closed without release");
  }
  return hand_step(config_.approach_hand_release, "LARGE release at saved slot");
}

MotionResult RosLeftArmMotionSystem::capture_transfer_obstacles()
{
  if (!moveit_planner_) return MotionResult::ok();
  if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh left joints before obstacle snapshot");
  const auto before = device_->left_joints();
  const auto captured = moveit_planner_->capture_obstacle_cloud(before);
  if (!captured.success) return captured;
  if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh left joints after obstacle snapshot");
  const auto after = device_->left_joints();
  for (std::size_t j = 0; j < before.size(); ++j) {
    if (std::abs(after[j]-before[j]) > config_.joint_follow_start_tolerance_rad) {
      return MotionResult::fail("left arm moved during obstacle snapshot; self-filter state is invalid");
    }
  }
  return captured;
}

MotionResult RosLeftArmMotionSystem::wait_for_stationary_left_joints(std::array<double, 7> &joints)
{
  using Clock = std::chrono::steady_clock;
  const auto deadline = Clock::now() + config_.joint_follow_settle_timeout;
  auto window_start = Clock::now();
  std::array<double, 7> low{}, high{};
  std::uint64_t previous = 0;
  std::size_t samples = 0;
  double largest_range = 0;
  RCLCPP_INFO(node_->get_logger(), "waiting for stationary left joints before planning: range <= %.6f rad over %ld ms",
    config_.joint_follow_settle_tolerance_rad, config_.joint_follow_settle_duration.count());
  while (rclcpp::ok() && Clock::now() < deadline) {
    std::uint64_t sequence = 0;
    if (!device_->fresh_left_joints(joints, config_.joint_follow_feedback_timeout, &sequence) || !joints_within_limits(joints)) {
      return MotionResult::fail("fresh bounded left joint feedback unavailable while waiting for stationary start");
    }
    if (sequence != previous) {
      previous = sequence;
      if (samples == 0) {low = high = joints; window_start = Clock::now();}
      largest_range = 0;
      for (std::size_t j = 0; j < 7; ++j) {
        low[j] = std::min(low[j], joints[j]); high[j] = std::max(high[j], joints[j]);
        largest_range = std::max(largest_range, high[j]-low[j]);
      }
      ++samples;
      if (largest_range > config_.joint_follow_settle_tolerance_rad) {
        low = high = joints; samples = 1; window_start = Clock::now();
      } else if (samples >= config_.stable_joint_samples && Clock::now()-window_start >= config_.joint_follow_settle_duration) {
        RCLCPP_INFO(node_->get_logger(), "stationary MoveIt start [%0.7f,%0.7f,%0.7f,%0.7f,%0.7f,%0.7f,%0.7f], range=%.7f rad",
          joints[0], joints[1], joints[2], joints[3], joints[4], joints[5], joints[6], largest_range);
        return MotionResult::ok();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return MotionResult::fail("left joints did not become stationary before planning; last range=" + std::to_string(largest_range));
}

MotionResult RosLeftArmMotionSystem::execute_moveit_transfer(MoveItTransferPlan &plan)
{
  try {
    for (int attempt = 0; attempt <= config_.joint_follow_start_replans; ++attempt) {
      if (!moveit_planner_ || !plan.success || plan.points.size() < 2) return MotionResult::fail("no accepted MoveIt trajectory");
      for (const auto &point : plan.points) {
        if (!joints_within_limits(point.joints)) return MotionResult::fail("MoveIt trajectory violates task joint limits");
      }
      // Read-only SDK FK verifies axis conventions, base frame and Arm_Tip mapping.
      // Check multiple configurations before sending the first streaming command.
      for (std::size_t index : {std::size_t(0), plan.points.size()/2, plan.points.size()-1}) {
        lbot_motion::CartesianPose sdk;
        constexpr int max_fk_attempts = 3;
        for (int fk_attempt = 1; fk_attempt <= max_fk_attempts; ++fk_attempt) {
          if (!rclcpp::ok()) return MotionResult::fail("shutdown during pre-transfer FK verification");
          if (fk_attempt > 1) {
            std::array<double, 7> measured_joints{};
            if (!device_->fresh_left_joints(measured_joints, config_.joint_follow_feedback_timeout) ||
                !joints_within_limits(measured_joints)) {
              return MotionResult::fail("pre-transfer FK retry stopped: fresh bounded left joint feedback unavailable; "
                "no joint_follow command sent for this segment");
            }
          }
          const auto read = device_->forward_kinematics(plan.points[index].joints, sdk);
          if (read.success) {
            if (fk_attempt > 1) RCLCPP_INFO(node_->get_logger(),
              "pre-transfer FK query recovered at sample %zu/%zu on attempt %d/%d; model agreement still required",
              index+1, plan.points.size(), fk_attempt, max_fk_attempts);
            break;
          }
          // Only retry a completed service response with success=false. This
          // includes the SDK's TCP timeout. Do not overlap outstanding ROS
          // requests, retry invalid numeric results, or retry any motion.
          const bool retryable = read.message == "left forward-kinematics request failed";
          if (!retryable || fk_attempt == max_fk_attempts) {
            std::ostringstream detail;
            detail << "pre-transfer FK verification failed at sample " << index+1 << '/' << plan.points.size()
              << " (trajectory t=" << plan.points[index].time_s << " s), attempt " << fk_attempt << '/'
              << max_fk_attempts << ": " << read.message
              << "; no joint_follow command sent for this segment";
            return MotionResult::fail(detail.str());
          }
          const auto &q = plan.points[index].joints;
          RCLCPP_WARN(node_->get_logger(),
            "pre-transfer FK query failed at sample %zu/%zu (t=%.3f s), attempt %d/%d: %s; "
            "retrying identical read-only query in 150 ms, no trajectory sent; "
            "joints=[%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f]",
            index+1, plan.points.size(), plan.points[index].time_s, fk_attempt, max_fk_attempts,
            read.message.c_str(), q[0], q[1], q[2], q[3], q[4], q[5], q[6]);
          std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        const auto calculated = moveit_planner_->forward_kinematics(plan.points[index].joints);
        const Pose6 measured{sdk.x, sdk.y, sdk.z, sdk.roll, sdk.pitch, sdk.yaw};
        const double position_error = std::hypot(calculated.x-sdk.x, calculated.y-sdk.y, calculated.z-sdk.z);
        const double angle_error = orientation_distance(calculated, measured);
        if (!std::isfinite(position_error) || !std::isfinite(angle_error) ||
            position_error > config_.pose_tolerance_m || angle_error > config_.pose_tolerance_rad) {
          std::ostringstream detail;
          detail << "MoveIt / controller FK mismatch at sample " << index+1 << '/' << plan.points.size()
            << ": " << position_error*1000. << " mm, " << angle_error
            << " rad; no joint_follow command sent for this segment";
          return MotionResult::fail(detail.str());
        }
      }
      if (!device_->wait_for_joint_follow_subscriber()) return MotionResult::fail("left joint_follow subscriber unavailable");
      if (!device_->wait_for_fresh_state()) return MotionResult::fail("no fresh state before joint_follow");
      const auto epoch = std::chrono::steady_clock::now();
      JointFollowCallbacks callbacks;
      callbacks.now = [epoch]() {return std::chrono::duration<double>(std::chrono::steady_clock::now()-epoch).count();};
      callbacks.sleep_until = [epoch](double t) {
        std::this_thread::sleep_until(epoch + std::chrono::duration<double>(t));
      };
      callbacks.running = []() {return rclcpp::ok();};
      std::string feedback_failure;
      callbacks.read = [this, epoch, &feedback_failure](auto &q, auto &sequence) {
        const auto feedback = device_->left_joint_feedback();
        q = feedback.joints;
        sequence = feedback.sequence;
        if (feedback.available && feedback.age <= config_.joint_follow_feedback_timeout && joints_within_limits(q)) {
          feedback_failure.clear();
          return true;
        }
        std::ostringstream detail;
        detail.precision(9);
        detail << "at t=" << std::chrono::duration<double>(std::chrono::steady_clock::now()-epoch).count()
          << " s, feedback_sequence=" << sequence;
        if (!feedback.available) {
          detail << ": no valid left joint feedback received";
        } else {
          detail << ", age=" << std::chrono::duration<double, std::milli>(feedback.age).count() << " ms";
          if (feedback.age > config_.joint_follow_feedback_timeout) {
            detail << ": feedback timeout (limit=" << config_.joint_follow_feedback_timeout.count() << " ms)";
          } else {
            detail << ": measured joint limit violation";
            for (std::size_t j = 0; j < q.size(); ++j) {
              const double lo = config_.left_joint_min[j] + config_.joint_limit_margin_rad;
              const double hi = config_.left_joint_max[j] - config_.joint_limit_margin_rad;
              if (!std::isfinite(q[j]) || q[j] < lo || q[j] > hi) {
                detail << "; J" << j+1 << " actual=" << q[j] << " allowed=[" << lo << ',' << hi << ']';
              }
            }
          }
          detail << "; last_joints=[";
          for (std::size_t j = 0; j < q.size(); ++j) detail << (j ? "," : "") << q[j];
          detail << ']';
        }
        feedback_failure = detail.str();
        return false;
      };
      bool command_attempted = false;
      callbacks.send = [this, &command_attempted](const auto &q) {
        command_attempted = true;
        return joints_within_limits(q) && device_->joint_follow(q).success;
      };
      std::string final_pose_status;
      callbacks.cartesian_goal_reached = [this, &plan, &final_pose_status](const auto &q) {
        // Use the already SDK-verified model on fresh measured joints, never
        // on commanded joints. The caller still requires independent driver
        // pose feedback before opening the hand.
        const auto actual = moveit_planner_->forward_kinematics(q);
        const auto &target = plan.goal;
        const double position_error = std::hypot(actual.x-target.x, actual.y-target.y, actual.z-target.z);
        const double angle_error = orientation_distance(actual, target);
        std::ostringstream status;
        status.precision(9);
        status << "measured FK xyz=[" << actual.x << ',' << actual.y << ',' << actual.z
          << "] target=[" << target.x << ',' << target.y << ',' << target.z
          << "] errors=" << position_error*1000 << " mm," << angle_error << " rad";
        final_pose_status = status.str();
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
          "joint_follow final arrival: %s", final_pose_status.c_str());
        return std::isfinite(position_error) && std::isfinite(angle_error) &&
          position_error <= config_.pose_tolerance_m && angle_error <= config_.pose_tolerance_rad;
      };
      const JointFollowOptions follow{
        1./config_.joint_follow_rate_hz, config_.joint_follow_max_lateness.count()/1000.,
        config_.joint_follow_start_tolerance_rad, config_.joint_follow_tracking_tolerance_rad,
        config_.joint_follow_goal_tolerance_rad, config_.pose_arrival_timeout.count()/1000., config_.stable_joint_samples};
      RCLCPP_INFO(node_->get_logger(), "starting MoveIt joint_follow: %.1f Hz, %.3f seconds, %zu planned samples",
        config_.joint_follow_rate_hz, plan.points.back().time_s, plan.points.size());
      auto executed = run_joint_follow(plan.points, follow, callbacks);
      if (!executed.success && !feedback_failure.empty()) {
        executed.message += "; " + feedback_failure;
      }
      if (!final_pose_status.empty()) {
        if (executed.success) {
          RCLCPP_INFO(node_->get_logger(), "%s; %s", executed.message.c_str(), final_pose_status.c_str());
        } else {
          executed.message += "; " + final_pose_status;
        }
      }
      // Retry planning ONLY if the start check failed before any publish attempt.
      // Never recover a running/failed stream by sending another motion automatically.
      if (executed.success || command_attempted ||
          executed.message.rfind("joint_follow start moved since planning:", 0) != 0 ||
          attempt == config_.joint_follow_start_replans) return executed;
      RCLCPP_WARN(node_->get_logger(), "%s; replanning from settled current joints (%d/%d)",
        executed.message.c_str(), attempt+1, config_.joint_follow_start_replans);
      std::array<double, 7> current{};
      const auto stationary = wait_for_stationary_left_joints(current);
      if (!stationary.success) return stationary;
      const auto goal = plan.goal;
      const auto goal_joints = plan.points.back().joints;
      const auto height_limits = plan.height_limits;
      plan = moveit_planner_->plan(current, {goal},
        config_.approach_place_planning_timeout.count()/1000., &goal_joints,
        height_limits ? &*height_limits : nullptr);
      if (!plan.success) return MotionResult::fail("MoveIt start resynchronization failed: " + plan.message);
      RCLCPP_INFO(node_->get_logger(), "start resynchronization: %s; selected goal XYZ/RPY unchanged", plan.message.c_str());
    }
    return MotionResult::fail("joint_follow start replan limit reached");
  } catch (const std::exception &error) {
    return MotionResult::fail(std::string("joint_follow exception: ") + error.what());
  }
}

MotionResult RosLeftArmMotionSystem::plan_slot_transfer(
  const Pose6 &lifted, const Pose6 &slot, const MotionPlanOptions &options,
  const std::array<double, 7> &seed, Pose6 &goal)
{
  const auto candidates = build_slot_transfer_candidates(
    lifted, slot, options, config_.approach_place_max_orientation_change_rad);
  const auto deadline = std::chrono::steady_clock::now() + config_.approach_place_planning_timeout;
  std::string last_error;
  std::size_t attempted = 0;
  for (const auto &candidate : candidates) {
    if (!rclcpp::ok() || std::chrono::steady_clock::now() >= deadline) break;
    ++attempted;
    auto solution = seed;  // Every candidate starts from the actual reached branch.
    const auto ik = solve_ik(candidate, solution);
    if (!ik.success) {
      last_error = ik.message;
      if (last_error.find("unavailable") != std::string::npos ||
          last_error.find("timeout") != std::string::npos) break;
      continue;
    }
    lbot_motion::CartesianPose fk;
    const auto checked = device_->forward_kinematics(solution, fk);
    if (!checked.success) {
      last_error = checked.message;
      break;
    }
    const Pose6 calculated{fk.x, fk.y, fk.z, fk.roll, fk.pitch, fk.yaw};
    if (!finite_pose(calculated) ||
        std::hypot(fk.x-candidate.x, fk.y-candidate.y, fk.z-candidate.z) > config_.pose_tolerance_m ||
        orientation_distance(calculated, candidate) > config_.pose_tolerance_rad) {
      last_error = "IK result failed FK pose verification";
      continue;
    }
    goal = candidate;
    RCLCPP_INFO(node_->get_logger(),
      "slot endpoint accepted at candidate %zu/%zu; orientation change=%.4f rad, fixed Arm_Tip z=%.6f; "
      "MoveJP controller will plan actual path from the waypoint",
      attempted, candidates.size(), orientation_distance(lifted, goal), goal.z);
    return MotionResult::ok("slot endpoint IK/FK accepted; controller path pending");
  }
  if (!rclcpp::ok()) return MotionResult::fail("shutdown during slot orientation search");
  // Independent IK is advisory for choosing nearby RPY. It must not become
  // the old pre-MoveJP reachability veto: MoveJP owns its own IK/planning.
  // This fallback happens before ANY transfer command, never after a failed move.
  goal = candidates.front();
  RCLCPP_WARN(node_->get_logger(),
    "no independently verified slot candidate after %zu attempts (%s); "
    "passing unchanged lift RPY and fixed Arm_Tip z=%.6f to MoveJP controller planner; "
    "reachability remains unverified", attempted, last_error.c_str(), goal.z);
  return MotionResult::ok("original lift RPY retained; controller MoveJP planning required");
}

MotionResult RosLeftArmMotionSystem::prepare_table_route()
{
  route_prepared_ = false;
  task_plan_prepared_ = false;
  if (config_.slot_transfer_planner != "sdk" && config_.slot_transfer_planner != "moveit") {
    return MotionResult::fail("slot_transfer_planner must be sdk or moveit");
  }
  if (config_.slot_transfer_planner == "moveit" && config_.approach_place_enabled) {
    if (!std::isfinite(config_.joint_follow_rate_hz) || config_.joint_follow_rate_hz < 10 ||
        config_.joint_follow_rate_hz > 100 ||
        !std::isfinite(config_.joint_follow_start_tolerance_rad) || config_.joint_follow_start_tolerance_rad <= 0 ||
        !std::isfinite(config_.joint_follow_goal_tolerance_rad) || config_.joint_follow_goal_tolerance_rad <= 0 ||
        !std::isfinite(config_.joint_follow_tracking_tolerance_rad) || config_.joint_follow_tracking_tolerance_rad <= 0 ||
        config_.joint_follow_feedback_timeout.count() <= 0 || config_.joint_follow_max_lateness.count() <= 0 ||
        !std::isfinite(config_.joint_follow_settle_tolerance_rad) || config_.joint_follow_settle_tolerance_rad <= 0 ||
        config_.joint_follow_settle_tolerance_rad > config_.joint_follow_start_tolerance_rad ||
        config_.joint_follow_settle_duration.count() <= 0 ||
        config_.joint_follow_settle_timeout < config_.joint_follow_settle_duration ||
        config_.joint_follow_start_replans < 0 || config_.joint_follow_start_replans > 4) {
      return MotionResult::fail("invalid joint_follow timing / tolerance parameters");
    }
    try {
      if (!moveit_planner_) moveit_planner_ = std::make_shared<MoveItTransferPlanner>(node_);
    } catch (const std::exception &error) {
      return MotionResult::fail(std::string("MoveIt initialization failed before motion: ") + error.what());
    }
  }
  if (!std::isfinite(config_.joint_limit_margin_rad) || config_.joint_limit_margin_rad < 0) {
    return MotionResult::fail("joint_limit_margin_rad must be finite and nonnegative");
  }
  for (std::size_t i = 0; i < 7; ++i) {
    if (!std::isfinite(config_.left_joint_min[i]) || !std::isfinite(config_.left_joint_max[i]) ||
        config_.left_joint_min[i] + 2 * config_.joint_limit_margin_rad >= config_.left_joint_max[i]) {
      return MotionResult::fail("configured joint limits are invalid");
    }
  }
  if (config_.approach_place_enabled) {
    for (const auto &waypoint : config_.approach_place_waypoints) {
      if (!joints_within_limits(waypoint.joints)) {
        return MotionResult::fail("approach_place_waypoint " + waypoint.name + " violates configured joint limits");
      }
    }
    if (!std::isfinite(config_.approach_place_max_orientation_change_rad) ||
        config_.approach_place_max_orientation_change_rad < 0 ||
        config_.approach_place_max_orientation_change_rad > 3.141592653589793 ||
        config_.approach_place_planning_timeout.count() <= 0) {
      return MotionResult::fail("placement orientation search or planning timeout is invalid");
    }
  }
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

MotionResult RosLeftArmMotionSystem::open_hand_for_cleanup(bool settle)
{
  RCLCPP_INFO(node_->get_logger(),
    "task cleanup: opening left hand [%u,%u,%u,%u,%u,%u]%s; physical opening is not sensor-verified",
    static_cast<unsigned>(config_.hand_open[0]), static_cast<unsigned>(config_.hand_open[1]),
    static_cast<unsigned>(config_.hand_open[2]), static_cast<unsigned>(config_.hand_open[3]),
    static_cast<unsigned>(config_.hand_open[4]), static_cast<unsigned>(config_.hand_open[5]),
    settle ? "" : " before requesting arm emergency stop");
  const auto result = set_hand(config_.hand_open);
  if (result.success && settle && config_.grip_settle.count() > 0) {
    std::this_thread::sleep_for(config_.grip_settle);
  }
  return result;
}

MotionResult RosLeftArmMotionSystem::finish_task()
{
  if (!motion_started_) return MotionResult::ok("no motion started; no hand cleanup needed");
  return open_hand_for_cleanup(true);
}

void RosLeftArmMotionSystem::stop() noexcept
{
  route_prepared_ = false;
  task_plan_prepared_ = false;
  if (!motion_started_) return;
  motion_started_ = false;
  // User-requested order: publish the open command first, then request stop.
  // Do not wait for grip_settle or require valid arm feedback on this path.
  try {
    const auto opened = open_hand_for_cleanup(false);
    if (!opened.success) {
      RCLCPP_ERROR(node_->get_logger(), "task cleanup hand command failed: %s", opened.message.c_str());
    }
  } catch (const std::exception &error) {
    RCLCPP_ERROR(node_->get_logger(), "task cleanup hand command threw: %s", error.what());
  } catch (...) {
    RCLCPP_ERROR(node_->get_logger(), "task cleanup hand command threw an unknown exception");
  }
  // A failed hand command must never suppress the emergency-stop request.
  try {
    const auto stopped = device_->set_emergency_stop(true);
    if (!stopped.success) {
      RCLCPP_ERROR(node_->get_logger(), "task cleanup emergency stop failed: %s", stopped.message.c_str());
    }
  } catch (const std::exception &error) {
    RCLCPP_ERROR(node_->get_logger(), "task cleanup emergency stop threw: %s", error.what());
  } catch (...) {
    RCLCPP_ERROR(node_->get_logger(), "task cleanup emergency stop threw an unknown exception");
  }
}

}  // namespace lbot_control
