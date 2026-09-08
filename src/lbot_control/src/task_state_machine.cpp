#include "lbot_control/task_state_machine.hpp"

#include <sstream>

namespace lbot_control {

const char *to_string(NutSize size) noexcept
{
  switch (size) {
    case NutSize::Large: return "large";
    case NutSize::Medium: return "medium";
    case NutSize::Small: return "small";
  }
  return "unknown";
}

const char *to_string(TaskState state) noexcept
{
  switch (state) {
    case TaskState::Idle: return "IDLE";
    case TaskState::MoveAboveTable: return "MOVE_ABOVE_TABLE";
    case TaskState::PickAndPlace: return "PICK_AND_PLACE";
    case TaskState::CheckPickResult: return "CHECK_PICK_RESULT";
    case TaskState::ReturnAboveTable: return "RETURN_ABOVE_TABLE";
    case TaskState::Retract: return "RETRACT";
    case TaskState::Complete: return "COMPLETE";
    case TaskState::Aborted: return "ABORTED";
  }
  return "UNKNOWN";
}

const char *to_string(TaskEvent event) noexcept
{
  switch (event) {
    case TaskEvent::StartRequested: return "START_REQUESTED";
    case TaskEvent::MotionSucceeded: return "MOTION_SUCCEEDED";
    case TaskEvent::NutStillPresent: return "NUT_STILL_PRESENT";
    case TaskEvent::NutRemoved: return "NUT_REMOVED";
    case TaskEvent::Failure: return "FAILURE";
    case TaskEvent::CancelRequested: return "CANCEL_REQUESTED";
    case TaskEvent::ResetRequested: return "RESET_REQUESTED";
  }
  return "UNKNOWN";
}

NutSize TaskStateMachine::active_size() const noexcept
{
  if (active_index_ == 0) return NutSize::Large;
  if (active_index_ == 1) return NutSize::Medium;
  return NutSize::Small;
}

bool TaskStateMachine::terminal() const noexcept
{
  return state_ == TaskState::Complete || state_ == TaskState::Aborted;
}

TransitionResult TaskStateMachine::transition(TaskState next, const std::string &message)
{
  TransitionResult result;
  result.accepted = true;
  result.previous = state_;
  result.current = next;
  result.message = message;
  state_ = next;
  return result;
}

TransitionResult TaskStateMachine::reject(TaskEvent event) const
{
  std::ostringstream message;
  message << "event " << to_string(event) << " is invalid while in " << to_string(state_);
  return {false, state_, state_, message.str()};
}

TransitionResult TaskStateMachine::handle(TaskEvent event, const std::string &reason)
{
  if (event == TaskEvent::ResetRequested) {
    if (!terminal()) return reject(event);
    active_index_ = 0;
    next_after_return_ = TaskState::PickAndPlace;
    return transition(TaskState::Idle, "task reset");
  }

  if (event == TaskEvent::Failure || event == TaskEvent::CancelRequested) {
    if (state_ == TaskState::Idle || terminal()) return reject(event);
    return transition(
      TaskState::Aborted,
      reason.empty() ? (event == TaskEvent::Failure ? "task failed" : "task cancelled") : reason);
  }

  switch (state_) {
    case TaskState::Idle:
      if (event == TaskEvent::StartRequested) {
        active_index_ = 0;
        return transition(TaskState::MoveAboveTable, "task started");
      }
      break;
    case TaskState::MoveAboveTable:
      if (event == TaskEvent::MotionSucceeded) {
        return transition(TaskState::PickAndPlace, "left hand moved above the table");
      }
      break;
    case TaskState::PickAndPlace:
      if (event == TaskEvent::MotionSucceeded) {
        return transition(
          TaskState::CheckPickResult,
          "pick and place finished; camera check before returning above the source frame");
      }
      break;
    case TaskState::CheckPickResult:
      if (event == TaskEvent::NutStillPresent) {
        next_after_return_ = TaskState::PickAndPlace;
        return transition(
          TaskState::ReturnAboveTable,
          "nut still present; returning above table before retrying same nut");
      }
      if (event == TaskEvent::NutRemoved) {
        if (active_index_ + 1 < 3) {
          ++active_index_;
          next_after_return_ = TaskState::PickAndPlace;
          return transition(
            TaskState::ReturnAboveTable,
            "nut removed; returning above table before proceeding to next nut");
        }
        next_after_return_ = TaskState::Retract;
        return transition(
          TaskState::ReturnAboveTable,
          "all three nuts removed; returning above table before retracting");
      }
      break;
    case TaskState::ReturnAboveTable:
      if (event == TaskEvent::MotionSucceeded) {
        if (next_after_return_ == TaskState::Retract) {
          return transition(TaskState::Retract, "left arm returned above the table");
        }
        return transition(TaskState::PickAndPlace, "left arm returned above the table");
      }
      break;
    case TaskState::Retract:
      if (event == TaskEvent::MotionSucceeded) {
        return transition(TaskState::Complete, "left arm retracted");
      }
      break;
    case TaskState::Complete:
    case TaskState::Aborted:
      break;
  }
  return reject(event);
}

}  // namespace lbot_control
