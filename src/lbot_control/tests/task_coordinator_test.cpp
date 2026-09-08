#include <cassert>
#include <memory>
#include <vector>

#include "lbot_control/task_coordinator.hpp"

namespace {

lbot_control::SceneObservation scene();

class FakeMotion final : public lbot_control::MotionSystem
{
public:
  lbot_control::MotionResult prepare(const lbot_control::MotionPlan &) override
  {
    return lbot_control::MotionResult::ok();
  }
  lbot_control::MotionResult execute(
    lbot_control::MotionStage stage,
    const lbot_control::PlannedTarget *target) override
  {
    stages.push_back(stage);
    if (stage == lbot_control::MotionStage::PickAndPlace) assert(target != nullptr);
    return lbot_control::MotionResult::ok();
  }
  void stop() noexcept override {stopped = true;}

  std::vector<lbot_control::MotionStage> stages;
  bool stopped{false};
};

class FakeVision final : public lbot_control::VisionSystem
{
public:
  lbot_control::SceneResult initial_scene() override
  {
    return lbot_control::SceneResult::ok(scene());
  }
  lbot_control::PickCheckResult check_nut_in_source(lbot_control::NutSize size) override
  {
    checked.push_back(size);
    if (checked.size() == 1) return lbot_control::PickCheckResult::present();
    return lbot_control::PickCheckResult::removed();
  }
  std::vector<lbot_control::NutSize> checked;
};

lbot_control::SceneObservation scene()
{
  lbot_control::SceneObservation value;
  value.frame_id = "base_torso_root";
  for (std::size_t index = 0; index < value.targets.size(); ++index) {
    value.targets[index].size = static_cast<lbot_control::NutSize>(index);
    value.targets[index].nut = {0.3, 0.2, 0.01, 0.0, 0.0, 0.0};
    value.targets[index].slot = {0.3, -0.2, 0.02, 0.0, 0.0, 0.0};
  }
  return value;
}

}  // namespace

int main()
{
  auto motion = std::make_shared<FakeMotion>();
  auto vision = std::make_shared<FakeVision>();
  lbot_control::TaskCoordinator coordinator(motion, vision);
  assert(coordinator.begin().success);
  while (!coordinator.state_machine().terminal()) assert(coordinator.step().success);

  assert(coordinator.state_machine().state() == lbot_control::TaskState::Complete);
  assert(vision->checked.size() == 4);
  assert(vision->checked[0] == lbot_control::NutSize::Large);
  assert(vision->checked[1] == lbot_control::NutSize::Large);
  // Initial entry, one PickAndPlace + ReturnAboveTable pair per attempt,
  // then final retract (the first large nut is intentionally retried).
  assert(motion->stages.size() == 10);
  assert(motion->stages.front() == lbot_control::MotionStage::MoveAboveTable);
  assert(motion->stages[1] == lbot_control::MotionStage::PickAndPlace);
  assert(motion->stages[2] == lbot_control::MotionStage::ReturnAboveTable);
  assert(motion->stages[3] == lbot_control::MotionStage::PickAndPlace);
  assert(motion->stages[4] == lbot_control::MotionStage::ReturnAboveTable);
  assert(motion->stages[5] == lbot_control::MotionStage::PickAndPlace);
  assert(motion->stages[6] == lbot_control::MotionStage::ReturnAboveTable);
  assert(motion->stages[7] == lbot_control::MotionStage::PickAndPlace);
  assert(motion->stages[8] == lbot_control::MotionStage::ReturnAboveTable);
  assert(motion->stages.back() == lbot_control::MotionStage::Retract);
  return 0;
}
