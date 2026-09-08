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
    target.grasp = detected.nut;
    target.grasp.roll = options.tool_roll_rad;
    target.grasp.pitch = options.tool_pitch_rad;
    target.grasp.yaw = detected.nut.yaw + options.tool_yaw_offset_rad;
    target.pregrasp = target.grasp;
    target.pregrasp.z += options.pregrasp_height_m;
    target.lift = target.grasp;
    target.lift.z += options.lift_height_m;

    target.slot_release = detected.slot;
    target.slot_release.roll = options.tool_roll_rad;
    target.slot_release.pitch = options.tool_pitch_rad;
    target.slot_release.yaw = options.tool_yaw_offset_rad;
    target.slot_release.z += options.slot_release_offset_m;
    target.slot_pre = target.slot_release;
    target.slot_pre.z += options.pregrasp_height_m;
    target.slot_retreat = target.slot_pre;
    result.plan.targets[i] = target;
  }

  result.success = true;
  result.message = "motion plan built";
  return result;
}

}  // namespace lbot_control
