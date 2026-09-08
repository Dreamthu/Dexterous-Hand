#ifndef LBOT_CONTROL__VISION_SYSTEM_HPP_
#define LBOT_CONTROL__VISION_SYSTEM_HPP_

#include <string>

#include "lbot_control/task_types.hpp"

namespace lbot_control {

struct SceneResult
{
  bool success{false};
  SceneObservation scene;
  std::string message;

  static SceneResult ok(const SceneObservation &scene) {return {true, scene, ""};}
  static SceneResult fail(const std::string &message) {return {false, {}, message};}
};

struct PickCheckResult
{
  bool success{false};
  bool nut_still_present{false};
  std::string message;

  static PickCheckResult present(const std::string &message = "")
  {
    return {true, true, message};
  }
  static PickCheckResult removed(const std::string &message = "")
  {
    return {true, false, message};
  }
  static PickCheckResult fail(const std::string &message)
  {
    return {false, false, message};
  }
};

struct VisionCommandResult
{
  bool success{false};
  std::string message;

  static VisionCommandResult ok(const std::string &message = "") { return {true, message}; }
  static VisionCommandResult fail(const std::string &message) { return {false, message}; }
};

// The future camera adapter only needs to provide the initial target poses and
// report whether the requested nut remains inside the black source frame.
// The check is made after placement retreat, before the arm returns above the
// source frame, so the camera is not occluded by the above-table pose.
class VisionSystem
{
public:
  virtual ~VisionSystem() = default;
  virtual SceneResult initial_scene() = 0;
  // Called immediately before the mechanical pick-and-place action.  The
  // default keeps non-ROS/fake vision implementations source-compatible.
  virtual VisionCommandResult start_target(NutSize) { return VisionCommandResult::ok(); }
  virtual PickCheckResult check_nut_in_source(NutSize size) = 0;
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__VISION_SYSTEM_HPP_
