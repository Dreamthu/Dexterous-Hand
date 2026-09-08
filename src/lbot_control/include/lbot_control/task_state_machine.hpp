#ifndef LBOT_CONTROL__TASK_STATE_MACHINE_HPP_
#define LBOT_CONTROL__TASK_STATE_MACHINE_HPP_

#include <cstddef>
#include <string>

#include "lbot_control/task_types.hpp"

namespace lbot_control {

enum class TaskState
{
  Idle,
  MoveAboveTable,
  PickAndPlace,
  CheckPickResult,
  ReturnAboveTable,
  Retract,
  Complete,
  Aborted
};

enum class TaskEvent
{
  StartRequested,
  MotionSucceeded,
  NutStillPresent,
  NutRemoved,
  Failure,
  CancelRequested,
  ResetRequested
};

const char *to_string(TaskState state) noexcept;
const char *to_string(TaskEvent event) noexcept;

struct TransitionResult
{
  bool accepted{false};
  TaskState previous{TaskState::Idle};
  TaskState current{TaskState::Idle};
  std::string message;
};

// This class owns only legal task order. It has no ROS dependency and cannot
// move hardware. The coordinator reports outcomes through handle().
class TaskStateMachine
{
public:
  TaskState state() const noexcept { return state_; }
  NutSize active_size() const noexcept;
  std::size_t active_index() const noexcept { return active_index_; }
  bool terminal() const noexcept;

  TransitionResult handle(TaskEvent event, const std::string &reason = "");

private:
  TransitionResult transition(TaskState next, const std::string &message);
  TransitionResult reject(TaskEvent event) const;

  TaskState state_{TaskState::Idle};
  std::size_t active_index_{0};
  TaskState next_after_return_{TaskState::PickAndPlace};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__TASK_STATE_MACHINE_HPP_
