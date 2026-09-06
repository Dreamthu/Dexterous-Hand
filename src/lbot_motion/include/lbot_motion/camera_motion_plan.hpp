#ifndef LBOT_MOTION__CAMERA_MOTION_PLAN_HPP_
#define LBOT_MOTION__CAMERA_MOTION_PLAN_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "lbot_motion/motion_planner.hpp"

namespace lbot_motion {

enum class CameraMotionMode { Extend, Retract };

struct CameraMotionOptions {
  double speed{0.2};
  double acceleration{0.2};
  bool hand_enabled{true};
  int hand_speed{250};
  int hand_force{250};
  int hand_close_position{0};
  int hand_open_position{255};
};

CameraMotionMode camera_motion_mode_from_string(const std::string &value);

// Edit this function to add/change the camera action. Values in arm_delta are
// radians; time values are absolute milliseconds from the beginning.
std::vector<MotionCommand> make_camera_motion_plan(
  Side side, CameraMotionMode mode, const CameraMotionOptions &options);

}  // namespace lbot_motion

#endif
