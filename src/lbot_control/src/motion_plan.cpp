#include "lbot_control/motion_plan.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace lbot_control {
namespace {

using Quaternion = std::array<double, 4>;  // x, y, z, w

Quaternion quaternion(const Pose6 &pose)
{
  const double cr = std::cos(pose.roll / 2), sr = std::sin(pose.roll / 2);
  const double cp = std::cos(pose.pitch / 2), sp = std::sin(pose.pitch / 2);
  const double cy = std::cos(pose.yaw / 2), sy = std::sin(pose.yaw / 2);
  return {sr*cp*cy-cr*sp*sy, cr*sp*cy+sr*cp*sy,
    cr*cp*sy-sr*sp*cy, cr*cp*cy+sr*sp*sy};
}

bool finite_pose(const Pose6 &pose)
{
  return std::isfinite(pose.x) && std::isfinite(pose.y) && std::isfinite(pose.z) &&
         std::isfinite(pose.roll) && std::isfinite(pose.pitch) && std::isfinite(pose.yaw);
}

Pose6 end_effector_pose_for_tcp(const Pose6 &tcp, const MotionPlanOptions &options)
{
  const double cr = std::cos(tcp.roll);
  const double sr = std::sin(tcp.roll);
  const double cp = std::cos(tcp.pitch);
  const double sp = std::sin(tcp.pitch);
  const double cy = std::cos(tcp.yaw);
  const double sy = std::sin(tcp.yaw);
  const double offset_x =
    cy * cp * options.tcp_offset_x_m +
    (cy * sp * sr - sy * cr) * options.tcp_offset_y_m +
    (cy * sp * cr + sy * sr) * options.tcp_offset_z_m;
  const double offset_y =
    sy * cp * options.tcp_offset_x_m +
    (sy * sp * sr + cy * cr) * options.tcp_offset_y_m +
    (sy * sp * cr - cy * sr) * options.tcp_offset_z_m;
  const double offset_z =
    -sp * options.tcp_offset_x_m + cp * sr * options.tcp_offset_y_m +
    cp * cr * options.tcp_offset_z_m;
  Pose6 result = tcp;
  result.x -= offset_x;
  result.y -= offset_y;
  result.z -= offset_z;
  return result;
}

}  // namespace

double orientation_distance(const Pose6 &first, const Pose6 &second)
{
  const auto a = quaternion(first), b = quaternion(second);
  double dot = 0.0;
  for (std::size_t i = 0; i < 4; ++i) dot += a[i] * b[i];
  return 2 * std::acos(std::clamp(std::abs(dot), 0.0, 1.0));
}

std::vector<Pose6> build_slot_transfer_candidates(
  const Pose6 &lifted, const Pose6 &slot, const MotionPlanOptions &options,
  double max_orientation_change_rad, const Pose6 *orientation_hint)
{
  if (!std::isfinite(max_orientation_change_rad) || max_orientation_change_rad < 0 ||
      max_orientation_change_rad > 3.141592653589793) {
    throw std::invalid_argument("placement orientation search must be in [0, pi] rad");
  }
  auto fixed_height_options = options;
  fixed_height_options.approach_place_reference_z_rpy.reset();
  std::vector<Pose6> goals;
  auto append = [&](const Pose6 &reference) {
    const auto target = build_slot_transfer_target(reference, slot, fixed_height_options);
    if (!target.success) throw std::invalid_argument(target.message);
    goals.push_back(target.goal);
  };
  append(lifted);
  if (max_orientation_change_rad == 0) return goals;
  const auto a = quaternion(lifted);
  // Three angular shells, 26 directions each; no Cartesian path IK sampling.
  for (int shell = 1; shell <= 3; ++shell) {
    const double angle = max_orientation_change_rad * shell / 3;
    for (int x = -1; x <= 1; ++x) for (int y = -1; y <= 1; ++y) for (int z = -1; z <= 1; ++z) {
      if (x == 0 && y == 0 && z == 0) continue;
      const double scale = std::sin(angle / 2) / std::sqrt(x*x + y*y + z*z);
      const Quaternion b{x*scale, y*scale, z*scale, std::cos(angle / 2)};
      const Quaternion q{
        a[3]*b[0]+a[0]*b[3]+a[1]*b[2]-a[2]*b[1],
        a[3]*b[1]-a[0]*b[2]+a[1]*b[3]+a[2]*b[0],
        a[3]*b[2]+a[0]*b[1]-a[1]*b[0]+a[2]*b[3],
        a[3]*b[3]-a[0]*b[0]-a[1]*b[1]-a[2]*b[2]};
      auto reference = lifted;
      reference.roll = std::atan2(2*(q[3]*q[0]+q[1]*q[2]), 1-2*(q[0]*q[0]+q[1]*q[1]));
      reference.pitch = std::asin(std::clamp(2*(q[3]*q[1]-q[2]*q[0]), -1.0, 1.0));
      reference.yaw = std::atan2(2*(q[3]*q[2]+q[0]*q[1]), 1-2*(q[1]*q[1]+q[2]*q[2]));
      append(reference);
    }
  }
  if (orientation_hint) {
    if (!finite_pose(*orientation_hint)) throw std::invalid_argument("non-finite orientation hint");
    auto b = quaternion(*orientation_hint);
    double dot = 0; for (std::size_t i = 0; i < 4; ++i) dot += a[i]*b[i];
    if (dot < 0) {for (auto &v : b) v = -v; dot = -dot;}
    const double half_angle = std::acos(std::clamp(dot, 0., 1.));
    if (half_angle > 1e-8) {
      const double maximum_fraction = std::min(1., max_orientation_change_rad/(2*half_angle));
      // Include measured last-waypoint RPY and nearby orientations toward the
      // frozen lift RPY. This avoids relying only on a coarse spherical grid.
      for (int step = 1; step <= 10; ++step) {
        const double fraction = maximum_fraction*step/10.;
        Quaternion q{};
        for (std::size_t i = 0; i < 4; ++i) q[i] =
          (std::sin((1-fraction)*half_angle)*a[i] + std::sin(fraction*half_angle)*b[i])/std::sin(half_angle);
        auto reference = lifted;
        reference.roll = std::atan2(2*(q[3]*q[0]+q[1]*q[2]), 1-2*(q[0]*q[0]+q[1]*q[1]));
        reference.pitch = std::asin(std::clamp(2*(q[3]*q[1]-q[2]*q[0]), -1., 1.));
        reference.yaw = std::atan2(2*(q[3]*q[2]+q[0]*q[1]), 1-2*(q[1]*q[1]+q[2]*q[2]));
        append(reference);
      }
      std::stable_sort(goals.begin()+1, goals.end(), [&](const auto &first, const auto &second) {
        return orientation_distance(lifted, first) < orientation_distance(lifted, second);
      });
    }
  }
  return goals;
}

HorizontalApproachResult build_horizontal_approach(
  const Pose6 &reached_pose, const Pose6 &nut, const MotionPlanOptions &options)
{
  if (!finite_pose(reached_pose) || !finite_pose(nut) ||
      !std::isfinite(options.tcp_offset_x_m) || !std::isfinite(options.tcp_offset_y_m) ||
      !std::isfinite(options.tcp_offset_z_m) ||
      !std::isfinite(options.pregrasp_height_m) || options.pregrasp_height_m <= 0.0) {
    return {false, "horizontal approach contains an invalid pose, TCP offset, or clearance", {}};
  }
  if (options.approach_reference_z_rpy) {
    const auto &reference = *options.approach_reference_z_rpy;
    Pose6 goal{nut.x, nut.y, reference[0], reference[1], reference[2], reference[3]};
    if (!finite_pose(goal)) return {false, "captured approach z/RPY is non-finite", {}};
    if (options.approach_target_is_tcp) {
      if (goal.z - nut.z < options.pregrasp_height_m) {
        return {false, "palm target does not provide the required clearance above the nut", {}};
      }
      // XYZ describes the palm centre, while RPY still describes Arm_Tip.
      // Compensate all three position components; never reset flange z.
      const auto flange = end_effector_pose_for_tcp(goal, options);
      if (!finite_pose(flange)) return {false, "palm-to-Arm_Tip conversion is non-finite", {}};
      return {true, "palm XYZ converted to Arm_Tip XYZ with unchanged RPY", flange};
    }
    // Keep the actual grasp-centre clearance check without shifting the
    // explicitly requested Arm_Tip x/y target.
    const auto shifted = end_effector_pose_for_tcp(goal, options);
    const double tcp_z = goal.z + (goal.z - shifted.z);
    if (!std::isfinite(tcp_z) || tcp_z - nut.z < options.pregrasp_height_m) {
      return {false, "captured approach height does not provide the required TCP clearance", {}};
    }
    return {true, "approach uses nut x/y and captured end-effector z/RPY", goal};
  }
  Pose6 tcp = reached_pose;
  tcp.x = nut.x;
  tcp.y = nut.y;
  Pose6 goal = end_effector_pose_for_tcp(tcp, options);
  const double offset_z = reached_pose.z - goal.z;
  if (!finite_pose(goal) || !std::isfinite(reached_pose.z + offset_z)) {
    return {false, "horizontal approach TCP conversion is non-finite", {}};
  }
  if (reached_pose.z + offset_z - nut.z < options.pregrasp_height_m) {
    return {false, "reached table height does not provide the required TCP clearance above the nut", {}};
  }
  goal.z = reached_pose.z;
  return {true, "horizontal approach preserves the reached height and orientation", goal};
}

HorizontalApproachResult build_vertical_tcp_target(
  const Pose6 &arm_tip_start, double tcp_z, const MotionPlanOptions &options)
{
  if (!finite_pose(arm_tip_start) || !std::isfinite(tcp_z) ||
      !std::isfinite(options.tcp_offset_x_m) || !std::isfinite(options.tcp_offset_y_m) ||
      !std::isfinite(options.tcp_offset_z_m)) {
    return {false, "vertical palm target contains non-finite values", {}};
  }
  Pose6 tcp = arm_tip_start;
  tcp.z = tcp_z;
  Pose6 goal = arm_tip_start;
  goal.z = end_effector_pose_for_tcp(tcp, options).z;
  if (!finite_pose(goal)) return {false, "vertical palm target conversion is non-finite", {}};
  return {true, "vertical palm target preserves Arm_Tip X/Y/RPY", goal};
}

HorizontalApproachResult build_slot_transfer_target(
  const Pose6 &arm_tip_start, const Pose6 &slot, const MotionPlanOptions &options)
{
  if (!finite_pose(arm_tip_start) || !finite_pose(slot) ||
      !std::isfinite(options.tcp_offset_x_m) || !std::isfinite(options.tcp_offset_y_m) ||
      !std::isfinite(options.tcp_offset_z_m)) {
    return {false, "slot transfer contains non-finite pose or TCP offset", {}};
  }
  Pose6 goal = arm_tip_start;
  if (options.approach_place_reference_z_rpy) {
    const auto &reference = *options.approach_place_reference_z_rpy;
    goal.z = reference[0];
    goal.roll = reference[1];
    goal.pitch = reference[2];
    goal.yaw = reference[3];
    if (!finite_pose(goal)) return {false, "placement reference Z/RPY is non-finite", {}};
  }
  const double arm_tip_z = goal.z;
  goal.x = slot.x;
  goal.y = slot.y;
  if (options.approach_target_is_tcp) goal = end_effector_pose_for_tcp(goal, options);
  goal.z = arm_tip_z;
  if (!finite_pose(goal)) return {false, "slot transfer TCP conversion is non-finite", {}};
  if (std::hypot(goal.x-arm_tip_start.x, goal.y-arm_tip_start.y, goal.z-arm_tip_start.z) > 2.0) {
    return {false, "slot transfer exceeds 2 m", {}};
  }
  return {true, options.approach_place_reference_z_rpy ?
    "slot transfer uses independent placement Arm_Tip Z/RPY" :
    "slot transfer preserves reached Arm_Tip Z/RPY", goal};
}

PlanResult build_motion_plan(
  const SceneObservation &scene, const MotionPlanOptions &options)
{
  PlanResult result;
  if (scene.frame_id != options.base_frame) {
    result.message = "scene frame must be " + options.base_frame + ", got " + scene.frame_id;
    return result;
  }
  if (!(options.pregrasp_height_m > 0.0) || !(options.lift_height_m > 0.0) ||
      !(options.slot_release_offset_m >= 0.0)) {
    result.message = "motion height options are invalid";
    return result;
  }
  if (!std::isfinite(options.tool_roll_rad) || !std::isfinite(options.tool_pitch_rad) ||
      !std::isfinite(options.tool_yaw_offset_rad) ||
      !std::isfinite(options.tcp_offset_x_m) || !std::isfinite(options.tcp_offset_y_m) ||
      !std::isfinite(options.tcp_offset_z_m)) {
    result.message = "tool pose or TCP offset options are invalid";
    return result;
  }

  result.plan.frame_id = scene.frame_id;
  for (std::size_t i = 0; i < scene.targets.size(); ++i) {
    const auto expected = static_cast<NutSize>(i);
    const auto &detected = scene.targets[i];
    if (detected.size != expected) {
      result.message = "targets must be ordered large, medium, small";
      return result;
    }
    if (!finite_pose(detected.nut) || !finite_pose(detected.slot)) {
      result.message = std::string("non-finite pose for ") + to_string(expected);
      return result;
    }
    PlannedTarget target;
    target.size = expected;
    Pose6 grasp_tcp = detected.nut;
    grasp_tcp.roll = options.tool_roll_rad;
    grasp_tcp.pitch = options.tool_pitch_rad;
    grasp_tcp.yaw = detected.nut.yaw + options.tool_yaw_offset_rad;
    Pose6 pregrasp_tcp = grasp_tcp;
    pregrasp_tcp.z += options.pregrasp_height_m;
    Pose6 lift_tcp = grasp_tcp;
    lift_tcp.z += options.lift_height_m;
    Pose6 release_tcp = detected.slot;
    release_tcp.roll = options.tool_roll_rad;
    release_tcp.pitch = options.tool_pitch_rad;
    release_tcp.yaw = options.tool_yaw_offset_rad;
    release_tcp.z += options.slot_release_offset_m;
    Pose6 slot_pre_tcp = release_tcp;
    slot_pre_tcp.z += options.pregrasp_height_m;
    target.grasp = end_effector_pose_for_tcp(grasp_tcp, options);
    target.pregrasp = end_effector_pose_for_tcp(pregrasp_tcp, options);
    target.lift = end_effector_pose_for_tcp(lift_tcp, options);
    target.slot_release = end_effector_pose_for_tcp(release_tcp, options);
    target.slot_pre = end_effector_pose_for_tcp(slot_pre_tcp, options);
    target.slot_retreat = target.slot_pre;
    result.plan.targets[i] = target;
  }

  result.success = true;
  result.message = "motion plan built";
  return result;
}

}  // namespace lbot_control
