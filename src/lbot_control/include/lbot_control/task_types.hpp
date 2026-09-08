#ifndef LBOT_CONTROL__TASK_TYPES_HPP_
#define LBOT_CONTROL__TASK_TYPES_HPP_

#include <array>
#include <cstddef>
#include <string>

namespace lbot_control {

enum class NutSize : std::size_t { Large = 0, Medium = 1, Small = 2 };

const char *to_string(NutSize size) noexcept;

struct Pose6
{
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double roll{0.0};
  double pitch{0.0};
  double yaw{0.0};
};

struct DetectedTarget
{
  NutSize size{NutSize::Large};
  Pose6 nut;
  Pose6 slot;
};

// All poses must already be transformed into frame_id. The task layer never
// accepts camera pixels or silently changes coordinate frames.
struct SceneObservation
{
  std::string frame_id;
  std::array<DetectedTarget, 3> targets{};
};

struct PlannedTarget
{
  NutSize size{NutSize::Large};
  Pose6 pregrasp;
  Pose6 grasp;
  Pose6 lift;
  Pose6 slot_pre;
  Pose6 slot_release;
  Pose6 slot_retreat;
};

struct MotionPlan
{
  std::string frame_id;
  std::array<PlannedTarget, 3> targets{};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__TASK_TYPES_HPP_
