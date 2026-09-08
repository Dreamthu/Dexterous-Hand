#include <cassert>
#include <string>

#include "lbot_control/task_state_machine.hpp"

using lbot_control::NutSize;
using lbot_control::TaskEvent;
using lbot_control::TaskState;
using lbot_control::TaskStateMachine;

int main()
{
  TaskStateMachine machine;
  assert(machine.state() == TaskState::Idle);
  assert(!machine.handle(TaskEvent::MotionSucceeded).accepted);
  assert(machine.handle(TaskEvent::StartRequested).accepted);
  assert(machine.state() == TaskState::MoveAboveTable);

  machine.handle(TaskEvent::MotionSucceeded);
  assert(machine.state() == TaskState::PickAndPlace);
  assert(machine.active_size() == NutSize::Large);

  machine.handle(TaskEvent::MotionSucceeded);
  assert(machine.state() == TaskState::CheckPickResult);
  machine.handle(TaskEvent::NutStillPresent);
  assert(machine.state() == TaskState::ReturnAboveTable);
  machine.handle(TaskEvent::MotionSucceeded);
  assert(machine.state() == TaskState::PickAndPlace);
  assert(machine.active_size() == NutSize::Large);

  for (int target = 0; target < 3; ++target) {
    machine.handle(TaskEvent::MotionSucceeded);
    assert(machine.state() == TaskState::CheckPickResult);
    machine.handle(TaskEvent::NutRemoved);
    assert(machine.state() == TaskState::ReturnAboveTable);
    machine.handle(TaskEvent::MotionSucceeded);
    if (target < 2) {
      assert(machine.state() == TaskState::PickAndPlace);
      assert(machine.active_index() == static_cast<std::size_t>(target + 1));
    } else {
      assert(machine.state() == TaskState::Retract);
    }
  }
  machine.handle(TaskEvent::MotionSucceeded);
  assert(machine.state() == TaskState::Complete);
  assert(machine.terminal());
  assert(machine.handle(TaskEvent::ResetRequested).accepted);
  assert(machine.state() == TaskState::Idle);

  machine.handle(TaskEvent::StartRequested);
  const auto failed = machine.handle(TaskEvent::Failure, "test failure");
  assert(failed.accepted);
  assert(failed.message == "test failure");
  assert(machine.state() == TaskState::Aborted);
  return 0;
}
