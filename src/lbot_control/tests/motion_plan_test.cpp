#include <cassert>
#include <cmath>

#include "lbot_control/motion_plan.hpp"

using lbot_control::DetectedTarget;
using lbot_control::MotionPlanOptions;
using lbot_control::NutSize;
using lbot_control::SceneObservation;

int main()
{
  SceneObservation scene;
  scene.frame_id = "base_link";
  for (std::size_t i = 0; i < scene.targets.size(); ++i) {
    auto &target = scene.targets[i];
    target.size = static_cast<NutSize>(i);
    target.nut.x = 0.30 + 0.05 * static_cast<double>(i);
    target.nut.y = 0.20;
    target.nut.z = 0.01;
    target.nut.yaw = 0.1 * static_cast<double>(i);
    target.slot.x = 0.30 + 0.05 * static_cast<double>(i);
    target.slot.y = -0.30;
    target.slot.z = 0.02;
  }

  MotionPlanOptions options;
  options.pregrasp_height_m = 0.10;
  options.lift_height_m = 0.15;
  options.slot_release_offset_m = 0.04;
  const auto result = lbot_control::build_motion_plan(scene, options);
  assert(result.success);
  assert(std::abs(result.plan.targets[0].pregrasp.z - 0.11) < 1e-12);
  assert(std::abs(result.plan.targets[0].lift.z - 0.16) < 1e-12);
  assert(std::abs(result.plan.targets[0].slot_release.z - 0.06) < 1e-12);
  assert(std::abs(result.plan.targets[0].slot_pre.z - 0.16) < 1e-12);

  scene.frame_id = "camera_color_optical_frame";
  assert(!lbot_control::build_motion_plan(scene, options).success);
  return 0;
}
