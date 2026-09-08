#ifndef LBOT_CONTROL__TASK_COORDINATOR_HPP_
#define LBOT_CONTROL__TASK_COORDINATOR_HPP_

#include <memory>

#include "lbot_control/motion_plan.hpp"
#include "lbot_control/motion_system.hpp"
#include "lbot_control/task_state_machine.hpp"
#include "lbot_control/vision_system.hpp"

namespace lbot_control {

// Coordinates one state-machine step at a time. It knows task ordering but
// depends only on the abstract motion and visual pick-result interfaces.
class TaskCoordinator
{
public:
  TaskCoordinator(
    std::shared_ptr<MotionSystem> motion,
    std::shared_ptr<VisionSystem> vision,
    MotionPlanOptions plan_options = {});

  MotionResult begin();
  MotionResult step();
  MotionResult cancel(const std::string &reason = "operator cancelled task");

  const TaskStateMachine &state_machine() const noexcept { return state_machine_; }
  const MotionPlan &plan() const noexcept { return plan_; }

private:
  MotionResult fail(const std::string &reason, bool request_stop);
  MotionResult execute_stage(MotionStage stage, const PlannedTarget *target = nullptr);

  std::shared_ptr<MotionSystem> motion_;
  std::shared_ptr<VisionSystem> vision_;
  MotionPlanOptions plan_options_;
  TaskStateMachine state_machine_;
  MotionPlan plan_;
  bool begun_{false};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__TASK_COORDINATOR_HPP_
