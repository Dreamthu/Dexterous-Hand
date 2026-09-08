#include "lbot_control/task_coordinator.hpp"

#include <utility>

namespace lbot_control {

TaskCoordinator::TaskCoordinator(
  std::shared_ptr<MotionSystem> motion,
  std::shared_ptr<VisionSystem> vision,
  MotionPlanOptions plan_options)
: motion_(std::move(motion)), vision_(std::move(vision)),
  plan_options_(std::move(plan_options))
{}

MotionResult TaskCoordinator::fail(const std::string &reason, bool request_stop)
{
  state_machine_.handle(TaskEvent::Failure, reason);
  if (request_stop && motion_) motion_->stop();
  return MotionResult::fail(reason);
}

MotionResult TaskCoordinator::begin()
{
  if (!motion_) return MotionResult::fail("motion system is not installed");
  if (!vision_) return MotionResult::fail("vision system is not installed");
  if (state_machine_.state() != TaskState::Idle) {
    return MotionResult::fail("task coordinator can only begin from IDLE");
  }
  const auto observed = vision_->initial_scene();
  if (!observed.success) return MotionResult::fail("initial scene failed: " + observed.message);
  const auto planned = build_motion_plan(observed.scene, plan_options_);
  if (!planned.success) return fail(planned.message, false);
  plan_ = planned.plan;
  const auto prepared = motion_->prepare(plan_);
  if (!prepared.success) return fail(prepared.message, false);
  state_machine_.handle(TaskEvent::StartRequested);
  begun_ = true;
  return MotionResult::ok("task prepared; first executable state is MOVE_ABOVE_TABLE");
}

MotionResult TaskCoordinator::execute_stage(
  MotionStage stage, const PlannedTarget *target)
{
  const auto result = motion_->execute(stage, target);
  if (!result.success) {
    return fail(std::string(to_string(stage)) + " failed: " + result.message, true);
  }
  const auto transition = state_machine_.handle(TaskEvent::MotionSucceeded);
  if (!transition.accepted) return fail(transition.message, true);
  return MotionResult::ok(transition.message);
}

MotionResult TaskCoordinator::step()
{
  if (!begun_) return MotionResult::fail("task has not been prepared");
  const auto index = state_machine_.active_index();
  const PlannedTarget *target = index < plan_.targets.size() ? &plan_.targets[index] : nullptr;
  switch (state_machine_.state()) {
    case TaskState::MoveAboveTable:
      return execute_stage(MotionStage::MoveAboveTable);
    case TaskState::PickAndPlace: {
      // Keep placement, the camera check, and the return to above-table in one
      // coordinator action.  The camera observes while the arm is still at
      // the slot retreat pose, before that final joint motion can occlude it.
      const auto started = vision_->start_target(state_machine_.active_size());
      if (!started.success) return fail("vision start failed: " + started.message, true);
      const auto placed = motion_->execute(MotionStage::PickAndPlace, target);
      if (!placed.success) {
        return fail(
          std::string(to_string(MotionStage::PickAndPlace)) + " failed: " + placed.message, true);
      }
      auto transition = state_machine_.handle(TaskEvent::MotionSucceeded);
      if (!transition.accepted) return fail(transition.message, true);

      const auto checked = vision_->check_nut_in_source(state_machine_.active_size());
      if (!checked.success) {
        // Even when vision cannot decide, finish this action at above_table.
        const auto returned = motion_->execute(MotionStage::ReturnAboveTable);
        if (!returned.success) {
          return fail(
            std::string(to_string(MotionStage::ReturnAboveTable)) + " failed: " +
            returned.message, true);
        }
        return fail("pick-result check failed: " + checked.message, false);
      }

      const auto event = checked.nut_still_present ?
        TaskEvent::NutStillPresent : TaskEvent::NutRemoved;
      transition = state_machine_.handle(event);
      if (!transition.accepted) return fail(transition.message, true);
      return execute_stage(MotionStage::ReturnAboveTable);
    }
    case TaskState::CheckPickResult: {
      // Normally consumed inside the PickAndPlace case above.  Retaining this
      // path keeps the coordinator recoverable if execution is stepped here.
      const auto checked = vision_->check_nut_in_source(state_machine_.active_size());
      if (!checked.success) return fail("pick-result check failed: " + checked.message, false);
      const auto event = checked.nut_still_present ?
        TaskEvent::NutStillPresent : TaskEvent::NutRemoved;
      const auto transition = state_machine_.handle(event);
      if (!transition.accepted) return fail(transition.message, true);
      return MotionResult::ok(transition.message);
    }
    case TaskState::ReturnAboveTable:
      return execute_stage(MotionStage::ReturnAboveTable);
    case TaskState::Retract:
      return execute_stage(MotionStage::Retract);
    case TaskState::Complete:
      return MotionResult::ok("task complete");
    case TaskState::Aborted:
      return MotionResult::fail("task is aborted");
    case TaskState::Idle:
      return MotionResult::fail("task is not ready to execute motion");
  }
  return MotionResult::fail("unknown task state");
}

MotionResult TaskCoordinator::cancel(const std::string &reason)
{
  const auto transition = state_machine_.handle(TaskEvent::CancelRequested, reason);
  if (!transition.accepted) return MotionResult::fail(transition.message);
  if (motion_) motion_->stop();
  return MotionResult::ok(reason);
}

}  // namespace lbot_control
