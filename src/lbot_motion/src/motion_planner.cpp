#include "lbot_motion/motion_planner.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <thread>

namespace lbot_motion {

MotionCommand MotionCommand::arm_delta(uint64_t at_ms, Side side, std::size_t joint,
                                       double radians, double speed, double acceleration)
{
  MotionCommand command;
  command.time_ms = at_ms; command.side = side; command.subsystem = Subsystem::Arm;
  command.joint = joint; command.value = radians; command.speed = speed;
  command.acceleration = acceleration; command.relative = true;
  return command;
}

MotionCommand MotionCommand::arm_target(uint64_t at_ms, Side side, std::size_t joint,
                                        double radians, double speed, double acceleration)
{
  auto command = arm_delta(at_ms, side, joint, radians, speed, acceleration);
  command.relative = false;
  return command;
}

MotionCommand MotionCommand::hand(uint64_t at_ms, Side side, std::size_t joint,
                                  uint8_t position, uint8_t speed, uint8_t force)
{
  MotionCommand command;
  command.time_ms = at_ms; command.side = side; command.subsystem = Subsystem::Hand;
  command.joint = joint; command.value = position; command.speed = speed;
  command.force = force; command.relative = false;
  return command;
}

MotionPlanner::MotionPlanner(const std::shared_ptr<MotionDevice> &device) : device_(device) {}
void MotionPlanner::add(const MotionCommand &command) { commands_.push_back(command); }
void MotionPlanner::add(const std::vector<MotionCommand> &commands)
{
  commands_.insert(commands_.end(), commands.begin(), commands.end());
}
void MotionPlanner::clear() { commands_.clear(); }

bool MotionPlanner::execute_arm_group(Side side, const std::vector<MotionCommand> &group)
{
  auto &target = side == Side::Left ? planned_left_ : planned_right_;
  double speed = 0.0, acceleration = 0.0;
  for (const auto &command : group) {
    if (command.joint >= target.size()) return false;
    target[command.joint] = command.relative ? target[command.joint] + command.value : command.value;
    speed = std::max(speed, command.speed);
    acceleration = std::max(acceleration, command.acceleration);
  }
  // The supplied driver executes MoveJ synchronously when block=true. Waiting
  // for the response is important here: otherwise a request can disappear
  // with the client process before the driver has accepted it. Commands at a
  // common timestamp are still one atomic seven-joint MoveJ, so joints remain
  // synchronized.
  return device_->arm(side).move_joints(target, {speed, acceleration, true}, "timeline group");
}

bool MotionPlanner::execute_hand_group(Side side, const std::vector<MotionCommand> &group)
{
  auto &positions = side == Side::Left ? planned_left_hand_ : planned_right_hand_;
  uint8_t speed = 0, force = 0;
  for (const auto &command : group) {
    if (command.joint >= positions.size()) return false;
    positions[command.joint] = static_cast<uint8_t>(std::max(0.0, std::min(255.0, command.value)));
    speed = std::max(speed, static_cast<uint8_t>(std::max(0.0, std::min(255.0, command.speed))));
    force = std::max(force, command.force);
  }
  // One publish is enough for a timeline tick; callers that need redundant
  // packets can use HandController directly with its repeat argument.
  return device_->hand(side).set_positions(positions, speed, force, 1);
}

bool MotionPlanner::execute()
{
  if (!device_ || commands_.empty()) return true;
  std::stable_sort(commands_.begin(), commands_.end(),
    [](const MotionCommand &a, const MotionCommand &b) { return a.time_ms < b.time_ms; });
  bool need_left = false, need_right = false;
  for (const auto &command : commands_) {
    if (command.subsystem == Subsystem::Arm) {
      (command.side == Side::Left ? need_left : need_right) = true;
    }
  }
  if ((need_left && !device_->wait_for_state(Side::Left)) ||
      (need_right && !device_->wait_for_state(Side::Right))) return false;
  if (!planned_state_ready_) {
    if (need_left) planned_left_ = device_->arm(Side::Left).current_joints();
    if (need_right) planned_right_ = device_->arm(Side::Right).current_joints();
    planned_left_hand_.fill(255);
    planned_right_hand_.fill(255);
    planned_state_ready_ = true;
  }

  const auto start = std::chrono::steady_clock::now();
  std::size_t offset = 0;
  while (offset < commands_.size() && rclcpp::ok()) {
    const auto timestamp = commands_[offset].time_ms;
    std::this_thread::sleep_until(start + std::chrono::milliseconds(timestamp));
    std::size_t end = offset;
    while (end < commands_.size() && commands_[end].time_ms == timestamp) ++end;

    std::vector<MotionCommand> left_arm, right_arm, left_hand, right_hand;
    for (std::size_t i = offset; i < end; ++i) {
      const auto &command = commands_[i];
      if (command.subsystem == Subsystem::Arm) {
        (command.side == Side::Left ? left_arm : right_arm).push_back(command);
      } else {
        (command.side == Side::Left ? left_hand : right_hand).push_back(command);
      }
    }
    // A hand topic is independent of MoveJ. Publish it before issuing the
    // non-blocking arm requests so both start within the same timeline tick.
    if (!left_hand.empty() && !execute_hand_group(Side::Left, left_hand)) return false;
    if (!right_hand.empty() && !execute_hand_group(Side::Right, right_hand)) return false;
    if (!left_arm.empty() && !execute_arm_group(Side::Left, left_arm)) return false;
    if (!right_arm.empty() && !execute_arm_group(Side::Right, right_arm)) return false;
    offset = end;
  }
  return offset == commands_.size();
}

}  // namespace lbot_motion
