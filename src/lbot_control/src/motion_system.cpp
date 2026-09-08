#include "lbot_control/motion_system.hpp"

namespace lbot_control {

const char *to_string(MotionStage stage) noexcept
{
  switch (stage) {
    case MotionStage::MoveAboveTable: return "MOVE_ABOVE_TABLE";
    case MotionStage::PickAndPlace: return "PICK_AND_PLACE";
    case MotionStage::ReturnAboveTable: return "RETURN_ABOVE_TABLE";
    case MotionStage::Retract: return "RETRACT";
  }
  return "UNKNOWN";
}

}  // namespace lbot_control
