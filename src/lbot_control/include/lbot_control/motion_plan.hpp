#ifndef LBOT_CONTROL__MOTION_PLAN_HPP_
#define LBOT_CONTROL__MOTION_PLAN_HPP_

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "lbot_control/task_types.hpp"

namespace lbot_control {

struct MotionPlanOptions
{
  std::string base_frame{"base_link"};
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
  // If set, approach uses nut x/y directly with this captured Arm_Tip z/RPY.
  // Full grasp planning continues to use the tool/TCP fields above.
  std::optional<std::array<double, 4>> approach_reference_z_rpy;
  // Interpret captured-reference XYZ as the grasp TCP, with Arm_Tip RPY.
  bool approach_target_is_tcp{false};
  // Optional placement-only Arm_Tip [z, roll, pitch, yaw]. The saved slot
  // still supplies XY; rotate palm compensation with this placement RPY.
  std::optional<std::array<double, 4>> approach_place_reference_z_rpy;
};

struct PlanResult
{
  bool success{false};
  std::string message;
  MotionPlan plan;
};

PlanResult build_motion_plan(
  const SceneObservation &scene, const MotionPlanOptions &options);

struct HorizontalApproachResult
{
  bool success{false};
  std::string message;
  Pose6 goal;
};

// Align the grasp TCP's x/y with a nut while preserving the reached
// end-effector height and orientation. The tool offset is rotated accordingly.
// With a captured reference, use raw nut x/y and reference z/RPY instead.
HorizontalApproachResult build_horizontal_approach(
  const Pose6 &reached_pose, const Pose6 &nut, const MotionPlanOptions &options);

// Change only the driver's Z to put the palm TCP at tcp_z. X/Y/RPY stay fixed.
HorizontalApproachResult build_vertical_tcp_target(
  const Pose6 &arm_tip_start, double tcp_z, const MotionPlanOptions &options);

// Replace XY with the saved slot's XY, compensating the palm when configured.
// Use placement-only Arm_Tip Z/RPY when configured, otherwise preserve the
// measured values. Slot Z never sets the release height.
HorizontalApproachResult build_slot_transfer_target(
  const Pose6 &arm_tip_start, const Pose6 &slot, const MotionPlanOptions &options);

// Quaternion rotation distance: equivalent Euler representations compare equal.
double orientation_distance(const Pose6 &first, const Pose6 &second);

// Exact lifted orientation first, then a finite local rotation search in
// increasing angular distance. Every candidate keeps the measured lift Z;
// legacy placement Z/RPY overrides are intentionally ignored in this mode.
// An optional measured orientation adds samples along its quaternion arc,
// clipped to the same angular bound and sorted with the other candidates.
std::vector<Pose6> build_slot_transfer_candidates(
  const Pose6 &lifted, const Pose6 &slot, const MotionPlanOptions &options,
  double max_orientation_change_rad, const Pose6 *orientation_hint = nullptr);

}  // namespace lbot_control

#endif  // LBOT_CONTROL__MOTION_PLAN_HPP_
