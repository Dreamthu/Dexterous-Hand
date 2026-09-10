#include "lbot_control/motion_plan.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace lbot_control {
namespace {

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
