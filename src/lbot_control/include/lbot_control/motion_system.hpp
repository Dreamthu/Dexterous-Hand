#ifndef LBOT_CONTROL__MOTION_SYSTEM_HPP_
#define LBOT_CONTROL__MOTION_SYSTEM_HPP_

#include <string>

#include "lbot_control/task_types.hpp"

namespace lbot_control {

enum class MotionStage
{
  MoveAboveTable,
  // Executes the mechanical portion through slot retreat.  The coordinator
  // performs the camera check at that visible pose, then invokes
  // ReturnAboveTable before this task action is considered complete.
  PickAndPlace,
  // Return from the camera-visible placement pose to the final above-table
  // joint waypoint after vision has inspected the source frame.
  ReturnAboveTable,
  Retract
};

const char *to_string(MotionStage stage) noexcept;

struct MotionResult
{
  bool success{false};
  std::string message;

  static MotionResult ok(const std::string &message = "") { return {true, message}; }
  static MotionResult fail(const std::string &message) { return {false, message}; }
};

// The task coordinator depends on this abstraction, never on ROS clients.
class MotionSystem
{
public:
  virtual ~MotionSystem() = default;

  virtual MotionResult prepare(const MotionPlan &plan) = 0;
  virtual MotionResult execute(
    MotionStage stage, const PlannedTarget *target = nullptr) = 0;
  virtual void stop() noexcept = 0;
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__MOTION_SYSTEM_HPP_
