#include <cmath>
#include <limits>
#include <stdexcept>
#include "lbot_control/motion_plan.hpp"

void require(bool ok, const char *message)
{
  if (!ok) throw std::runtime_error(message);
}

int main()
{
  using namespace lbot_control;
  const Pose6 reached{0.4, 0.2, -0.24, 0.0, 0.0, 1.5707963267948966};
  const Pose6 nut{0.3, 0.35, -0.51, 0.0, 0.0, 0.0};
  MotionPlanOptions options;
  options.tcp_offset_x_m = 0.1;
  options.tcp_offset_z_m = -0.02;
  const auto plan = build_horizontal_approach(reached, nut, options);
  require(plan.success, "valid horizontal approach rejected");
  require(std::abs(plan.goal.x - 0.3) < 1e-9 && std::abs(plan.goal.y - 0.25) < 1e-9,
          "TCP offset was not rotated into the reached orientation");
  require(plan.goal.z == reached.z && plan.goal.roll == reached.roll &&
          plan.goal.pitch == reached.pitch && plan.goal.yaw == reached.yaw,
          "horizontal approach changed reached height/orientation");
  auto too_high = nut;
  too_high.z = -0.3;
  require(!build_horizontal_approach(reached, too_high, options).success,
          "insufficient actual TCP clearance accepted");
  options.tcp_offset_z_m = std::numeric_limits<double>::quiet_NaN();
  require(!build_horizontal_approach(reached, nut, options).success, "invalid TCP accepted");
  options = MotionPlanOptions{};
  auto invalid = reached;
  invalid.roll = std::numeric_limits<double>::infinity();
  require(!build_horizontal_approach(invalid, nut, options).success, "invalid actual pose accepted");
  options.pregrasp_height_m = 0.0;
  require(!build_horizontal_approach(reached, nut, options).success, "invalid clearance accepted");
  options = MotionPlanOptions{};
  options.tcp_offset_x_m = -0.1;
  options.tcp_offset_y_m = 0.02;
  options.approach_reference_z_rpy = std::array<double, 4>{-0.36, 1.08, 0.12, -1.18};
  const auto reference_plan = build_horizontal_approach(reached, nut, options);
  require(reference_plan.success, "valid captured reference rejected");
  require(reference_plan.goal.x == nut.x && reference_plan.goal.y == nut.y,
          "captured reference mode shifted the requested nut x/y by TCP offset");
  require(reference_plan.goal.z == -0.36 && reference_plan.goal.roll == 1.08 &&
          reference_plan.goal.pitch == 0.12 && reference_plan.goal.yaw == -1.18,
          "captured z/RPY was replaced with enter pose or full-grasp orientation");
  require(!build_horizontal_approach(reached, too_high, options).success,
          "captured reference with insufficient TCP clearance accepted");
  (*options.approach_reference_z_rpy)[0] = std::numeric_limits<double>::quiet_NaN();
  require(!build_horizontal_approach(reached, nut, options).success,
          "non-finite captured height accepted");
  options.approach_target_is_tcp = true;
  options.tcp_offset_x_m = 0.1;
  options.tcp_offset_y_m = 0.02;
  options.tcp_offset_z_m = -0.03;
  options.approach_reference_z_rpy = std::array<double, 4>{-0.36, 0.0, 0.0, 1.5707963267948966};
  const auto palm_plan = build_horizontal_approach(reached, nut, options);
  require(palm_plan.success, "valid palm target rejected");
  // A +90 degree yaw maps the local offset to [-0.02, 0.1, -0.03].
  require(std::abs(palm_plan.goal.x - (nut.x + 0.02)) < 1e-9 &&
          std::abs(palm_plan.goal.y - (nut.y - 0.1)) < 1e-9 &&
          std::abs(palm_plan.goal.z - (-0.33)) < 1e-9,
          "palm-to-flange conversion must compensate XYZ with the rotated offset");
  require(palm_plan.goal.roll == 0.0 && palm_plan.goal.pitch == 0.0 &&
          palm_plan.goal.yaw == (*options.approach_reference_z_rpy)[3],
          "palm reference changed Arm_Tip RPY");
  require(!build_horizontal_approach(reached, too_high, options).success,
          "palm target clearance checked at the flange instead of the palm");
  (*options.approach_reference_z_rpy)[2] = std::numeric_limits<double>::quiet_NaN();
  require(!build_horizontal_approach(reached, nut, options).success,
          "non-finite palm orientation accepted");
  const auto vertical = build_vertical_tcp_target(palm_plan.goal, -0.370, options);
  require(vertical.success, "valid vertical palm target rejected");
  require(vertical.goal.x == palm_plan.goal.x && vertical.goal.y == palm_plan.goal.y &&
          vertical.goal.roll == palm_plan.goal.roll && vertical.goal.pitch == palm_plan.goal.pitch &&
          vertical.goal.yaw == palm_plan.goal.yaw,
          "vertical move changed X/Y/RPY");
  require(std::abs(vertical.goal.z - (-0.340)) < 1e-9,
          "absolute palm Z was mistaken for flange Z");
  require(!build_vertical_tcp_target(palm_plan.goal,
          std::numeric_limits<double>::quiet_NaN(), options).success,
          "non-finite vertical height accepted");
  const Pose6 slot{0.65, -0.4, -0.55, 0.2, 0.4, 0.8};
  const auto transfer = build_slot_transfer_target(reached, slot, options);
  require(transfer.success, "valid slot transfer rejected");
  require(std::abs(transfer.goal.x - 0.67) < 1e-9 &&
          std::abs(transfer.goal.y - (-0.5)) < 1e-9,
          "slot XY did not compensate rotated palm offset");
  require(transfer.goal.z == reached.z && transfer.goal.roll == reached.roll &&
          transfer.goal.pitch == reached.pitch && transfer.goal.yaw == reached.yaw,
          "slot transfer replaced measured Z/RPY with slot or saved approach pose");
  options.approach_target_is_tcp = false;
  const auto raw_transfer = build_slot_transfer_target(reached, slot, options);
  require(raw_transfer.success && raw_transfer.goal.x == slot.x && raw_transfer.goal.y == slot.y,
          "raw Arm_Tip slot XY was shifted by a TCP offset");
  auto distant = slot;
  distant.x = 5.0;
  require(!build_slot_transfer_target(reached, distant, options).success,
          "slot transfer over 2 metres accepted");
  require(!build_slot_transfer_target(invalid, slot, options).success,
          "slot transfer accepted a non-finite measured pose");
  options.approach_target_is_tcp = true;
  options.approach_place_reference_z_rpy = std::array<double, 4>{-0.10, 1.5707963267948966, 0.0, 0.0};
  const auto independent = build_slot_transfer_target(reached, slot, options);
  require(independent.success, "valid independent placement pose rejected");
  require(std::abs(independent.goal.x - 0.55) < 1e-9 &&
          std::abs(independent.goal.y - (-0.43)) < 1e-9,
          "placement TCP compensation used grasp RPY instead of placement RPY");
  require(independent.goal.z == -0.10 && independent.goal.roll == 1.5707963267948966 &&
          independent.goal.pitch == 0.0 && independent.goal.yaw == 0.0,
          "placement Arm_Tip Z/RPY was overwritten by palm compensation or measured pose");
  options.approach_reference_z_rpy = std::array<double, 4>{-0.36, 0.0, 0.0, 1.5707963267948966};
  const auto unchanged_grasp = build_horizontal_approach(reached, nut, options);
  require(unchanged_grasp.success && unchanged_grasp.goal.z == palm_plan.goal.z &&
          unchanged_grasp.goal.roll == palm_plan.goal.roll && unchanged_grasp.goal.yaw == palm_plan.goal.yaw,
          "placement reference changed the grasp target");
  (*options.approach_place_reference_z_rpy)[0] = 5.0;
  require(!build_slot_transfer_target(reached, slot, options).success,
          "placement reference bypassed the 3D distance limit");
  (*options.approach_place_reference_z_rpy)[0] = std::numeric_limits<double>::quiet_NaN();
  require(!build_slot_transfer_target(reached, slot, options).success,
          "non-finite independent placement height accepted");
  const auto candidates = build_slot_transfer_candidates(reached, slot, options, 0.35);
  require(candidates.size() == 79, "bounded orientation candidate set is incomplete");
  require(candidates.front().roll == reached.roll && candidates.front().pitch == reached.pitch &&
          candidates.front().yaw == reached.yaw, "unchanged lifted RPY was not first");
  double previous_angle = 0;
  for (const auto &candidate : candidates) {
    require(candidate.z == reached.z, "orientation search changed frozen lift Z");
    const auto angle = orientation_distance(reached, candidate);
    require(angle <= 0.35 + 1e-7 && angle + 1e-7 >= previous_angle,
            "candidate orientation changes are unbounded or unordered");
    previous_angle = angle;
    // Reconstruct the configured palm point, independently of the builder.
    const double cr=std::cos(candidate.roll), sr=std::sin(candidate.roll);
    const double cp=std::cos(candidate.pitch), sp=std::sin(candidate.pitch);
    const double cy=std::cos(candidate.yaw), sy=std::sin(candidate.yaw);
    const double ox=cy*cp*options.tcp_offset_x_m+(cy*sp*sr-sy*cr)*options.tcp_offset_y_m+
      (cy*sp*cr+sy*sr)*options.tcp_offset_z_m;
    const double oy=sy*cp*options.tcp_offset_x_m+(sy*sp*sr+cy*cr)*options.tcp_offset_y_m+
      (sy*sp*cr-cy*sr)*options.tcp_offset_z_m;
    require(std::abs(candidate.x+ox-slot.x)<1e-9 && std::abs(candidate.y+oy-slot.y)<1e-9,
            "changed RPY failed to rotate palm XY compensation");
  }
  auto equivalent = reached;
  equivalent.yaw += 2 * 3.141592653589793;
  require(orientation_distance(reached, equivalent) < 1e-7, "equivalent Euler angles differ");
  require(build_slot_transfer_candidates(reached, slot, options, 0).size() == 1,
          "zero orientation tolerance must preserve exact RPY only");
  auto hint = reached; hint.yaw -= 1.63; hint.z += .1;
  const auto expanded = build_slot_transfer_candidates(reached, slot, options, 1.7453292519943295, &hint);
  bool found_hint = false; previous_angle = 0;
  for (const auto &candidate : expanded) {
    const auto angle = orientation_distance(reached, candidate);
    require(candidate.z == reached.z && angle <= 1.7453292519943295 + 1e-7 && angle+1e-7 >= previous_angle,
            "expanded orientation search changed Z, exceeded bound, or lost ordering");
    previous_angle = angle;
    found_hint = found_hint || orientation_distance(candidate, hint) < 1e-7;
  }
  require(found_hint, "last waypoint orientation missing from expanded search");
  bool rejected = false;
  try {build_slot_transfer_candidates(reached, slot, options, -0.1);}
  catch (const std::invalid_argument &) {rejected = true;}
  require(rejected, "negative orientation tolerance accepted");
  return 0;
}
