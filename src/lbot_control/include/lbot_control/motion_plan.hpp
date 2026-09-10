#ifndef LBOT_CONTROL__MOTION_PLAN_HPP_
#define LBOT_CONTROL__MOTION_PLAN_HPP_

#include <string>

#include "lbot_control/task_types.hpp"

namespace lbot_control {

struct MotionPlanOptions
{
  std::string base_frame{"base_torso_root"};
  double pregrasp_height_m{0.10};
  double lift_height_m{0.15};
  double slot_release_offset_m{0.04};
  double tool_roll_rad{0.0};
  double tool_pitch_rad{-1.5707963267948966};
  double tool_yaw_offset_rad{0.0};
  // Vector from the driver-controlled end-effector origin to the grasp TCP,
  // expressed in the tool frame.
  double tcp_offset_x_m{0.0};
  double tcp_offset_y_m{0.0};
  double tcp_offset_z_m{0.0};
};

struct PlanResult
{
  bool success{false};
  std::string message;
  MotionPlan plan;
};

PlanResult build_motion_plan(
  const SceneObservation &scene, const MotionPlanOptions &options);

}  // namespace lbot_control

#endif  // LBOT_CONTROL__MOTION_PLAN_HPP_
