#include "lbot_motion/camera_motion_plan.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace lbot_motion {

namespace {
constexpr double kPi = 3.14159265358979323846;
double deg(double value) { return value * kPi / 180.0; }
uint8_t clamp_byte(int value)
{
  return static_cast<uint8_t>(std::max(0, std::min(255, value)));
}
}

CameraMotionMode camera_motion_mode_from_string(const std::string &value)
{
  if (value == "extend") return CameraMotionMode::Extend;
  if (value == "retract") return CameraMotionMode::Retract;
  throw std::invalid_argument("motion must be extend or retract");
}

std::vector<MotionCommand> make_camera_motion_plan(
  Side side, CameraMotionMode mode, const CameraMotionOptions &options)
{
  // Add or reorder arm actions here. Each pair is {J1..J7 index, degrees}.
  const std::vector<std::pair<std::size_t, double>> extension = {
    {0, 20.0},   // t=0 ms:   J1 +20 deg
    {3, -90.0},  // t=3000 ms: J4 -90 deg
    {0, 75.0},   // t=12000 ms: J1 +75 deg
    {5, 90.0}    // t=20000 ms: J6 +90 deg
  };
  const std::array<uint64_t, 4> times_ms = {{0, 3000, 12000, 20000}};
  std::vector<MotionCommand> result;
  result.reserve(extension.size() + (options.hand_enabled ? 6 : 0));

  const bool retract = mode == CameraMotionMode::Retract;
  for (std::size_t i = 0; i < extension.size(); ++i) {
    const auto &step = retract ? extension[extension.size() - 1 - i] : extension[i];
    result.push_back(MotionCommand::arm_delta(
      times_ms[i], side, step.first, deg(retract ? -step.second : step.second),
      options.speed, options.acceleration));
  }

  if (options.hand_enabled) {
    const auto position = clamp_byte(retract ? options.hand_open_position : options.hand_close_position);
    const auto speed = clamp_byte(options.hand_speed);
    const auto force = clamp_byte(options.hand_force);
    for (std::size_t joint = 0; joint < 6; ++joint) {
      result.push_back(MotionCommand::hand(0, side, joint, position, speed, force));
    }
  }
  return result;
}

}  // namespace lbot_motion
