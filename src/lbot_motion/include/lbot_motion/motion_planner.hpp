#ifndef LBOT_MOTION__MOTION_PLANNER_HPP_
#define LBOT_MOTION__MOTION_PLANNER_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "lbot_motion/motion_device.hpp"

namespace lbot_motion {

enum class Subsystem { Arm, Hand };

// One item in a timeline. Arm value is radians; hand value is 0..255.
// For arm commands, relative=true (the default) preserves the old camera
// motion behaviour and adds value to the measured joint position.
struct MotionCommand {
  uint64_t time_ms{0};
  Side side{Side::Right};
  Subsystem subsystem{Subsystem::Arm};
  std::size_t joint{0};
  double value{0.0};
  double speed{0.2};
  double acceleration{0.2};
  bool relative{true};
  uint8_t force{250};

  static MotionCommand arm_delta(uint64_t at_ms, Side side, std::size_t joint,
                                 double radians, double speed = 0.2,
                                 double acceleration = 0.2);
  static MotionCommand arm_target(uint64_t at_ms, Side side, std::size_t joint,
                                  double radians, double speed = 0.2,
                                  double acceleration = 0.2);
  static MotionCommand hand(uint64_t at_ms, Side side, std::size_t joint,
                            uint8_t position, uint8_t speed = 250,
                            uint8_t force = 250);
};

class MotionPlanner {
public:
  explicit MotionPlanner(const std::shared_ptr<MotionDevice> &device);
  void add(const MotionCommand &command);
  void add(const std::vector<MotionCommand> &commands);
  void clear();
  const std::vector<MotionCommand> &commands() const { return commands_; }
  bool execute();

private:
  bool execute_arm_group(Side side, const std::vector<MotionCommand> &group);
  bool execute_hand_group(Side side, const std::vector<MotionCommand> &group);

  std::shared_ptr<MotionDevice> device_;
  std::vector<MotionCommand> commands_;
  std::array<double, 7> planned_left_{};
  std::array<double, 7> planned_right_{};
  std::array<uint8_t, 6> planned_left_hand_{};
  std::array<uint8_t, 6> planned_right_hand_{};
  bool planned_state_ready_{false};
};

}  // namespace lbot_motion

#endif
