#ifndef LBOT_CONTROL__JOINT_FOLLOW_TRAJECTORY_HPP_
#define LBOT_CONTROL__JOINT_FOLLOW_TRAJECTORY_HPP_
#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "lbot_control/motion_system.hpp"
namespace lbot_control {
struct TimedJointPoint {double time_s{0}; std::array<double, 7> joints{};};
struct JointFollowOptions {
  double period_s{.02}, max_lateness_s{.06}, start_tolerance{.005}, tracking_tolerance{.08};
  double goal_tolerance{.005}, goal_timeout_s{10};
  std::size_t stable_samples{3};
};
struct JointFollowCallbacks {
  std::function<double()> now;
  std::function<void(double)> sleep_until;
  std::function<bool()> running;
  // Returns false for stale, malformed or unavailable measured state.
  std::function<bool(std::array<double, 7> &, std::uint64_t &)> read;
  std::function<bool(const std::array<double, 7> &)> send;
  // Optional task-space arrival check on fresh measured joints. Evaluated only
  // after the complete trajectory, with tracking/feedback gates still enforced.
  std::function<bool(const std::array<double, 7> &)> cartesian_goal_reached;
};
MotionResult run_joint_follow(const std::vector<TimedJointPoint> &points,
  const JointFollowOptions &options, const JointFollowCallbacks &callbacks);
}  // namespace lbot_control
#endif
