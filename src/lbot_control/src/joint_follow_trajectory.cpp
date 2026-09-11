#include "lbot_control/joint_follow_trajectory.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
namespace lbot_control {
MotionResult run_joint_follow(const std::vector<TimedJointPoint> &points,
  const JointFollowOptions &o, const JointFollowCallbacks &c) {
  const auto finite = [](const auto &q) {return std::all_of(q.begin(), q.end(), [](double v) {return std::isfinite(v);});};
  const auto distance = [](const auto &a, const auto &b) {
    double maximum = 0; for (std::size_t j = 0; j < 7; ++j) maximum = std::max(maximum, std::abs(a[j]-b[j]));
    return maximum;
  };
  if (points.size() < 2 || points.front().time_s != 0 || o.period_s <= 0 ||
      !std::isfinite(o.period_s) || o.max_lateness_s <= 0 || o.start_tolerance <= 0 ||
      o.tracking_tolerance <= 0 || o.goal_tolerance <= 0 || o.goal_timeout_s <= 0 || !o.stable_samples) {
    return MotionResult::fail("invalid joint_follow trajectory/options");
  }
  for (std::size_t i = 0; i < points.size(); ++i) {
    if (!std::isfinite(points[i].time_s) || !finite(points[i].joints) ||
        (i && points[i].time_s <= points[i-1].time_s)) return MotionResult::fail("invalid trajectory sample/time order");
  }
  std::array<double, 7> actual{};
  std::uint64_t sequence = 0;
  if (!c.running() || !c.read(actual, sequence) || !finite(actual)) return MotionResult::fail("no fresh state before joint_follow");
  if (distance(actual, points.front().joints) > o.start_tolerance) {
    std::ostringstream message;
    message.precision(8);
    message << "joint_follow start moved since planning: tolerance=" << o.start_tolerance << " rad;";
    for (std::size_t j = 0; j < 7; ++j) message << " J" << j+1 << " planned=" << points.front().joints[j]
      << " actual=" << actual[j] << " delta=" << actual[j]-points.front().joints[j] << ';';
    return MotionResult::fail(message.str());
  }
  const double start = c.now();
  double due = start;
  std::uint64_t last_sequence = sequence;
  std::size_t stable = 0, cartesian_stable = 0, segment = 1;
  const double duration = points.back().time_s;
  while (c.running()) {
    c.sleep_until(due);
    const double now = c.now();
    if (!c.running()) break;
    if (now - due > o.max_lateness_s) return MotionResult::fail("joint_follow dispatch deadline missed");
    const double t = std::max(0., now - start);
    if (!c.read(actual, sequence) || !finite(actual)) return MotionResult::fail("joint_follow feedback stale or invalid");
    while (segment + 1 < points.size() && points[segment].time_s < t) ++segment;
    const auto &a = points[segment-1]; const auto &b = points[segment];
    const double fraction = std::clamp((t-a.time_s)/(b.time_s-a.time_s), 0., 1.);
    std::array<double, 7> desired{};
    for (std::size_t j = 0; j < 7; ++j) desired[j] = a.joints[j] + fraction*(b.joints[j]-a.joints[j]);
    const double error = distance(actual, desired);
    if (error > o.tracking_tolerance) return MotionResult::fail("joint_follow tracking error " + std::to_string(error) + " rad");
    const bool new_final_feedback = t >= duration && sequence != last_sequence;
    const bool cartesian_arrived = new_final_feedback && c.cartesian_goal_reached && c.cartesian_goal_reached(actual);
    if (t > duration + o.goal_timeout_s) {
      std::ostringstream message;
      message.precision(9);
      message << "joint_follow final joints did not settle: goal tolerance=" << o.goal_tolerance
        << " rad, joint stable=" << stable << '/' << o.stable_samples
        << ", Cartesian stable=" << cartesian_stable << '/' << o.stable_samples
        << ", feedback_sequence=" << sequence << ", max_error=" << distance(actual, points.back().joints) << " rad;";
      for (std::size_t j = 0; j < 7; ++j) message << " J" << j+1 << " target=" << points.back().joints[j]
        << " actual=" << actual[j] << " delta=" << actual[j]-points.back().joints[j] << ';';
      return MotionResult::fail(message.str());
    }
    if (!c.send(desired)) return MotionResult::fail("joint_follow publish failed / subscriber lost");
    if (new_final_feedback) {
      stable = error <= o.goal_tolerance ? stable + 1 : 0;
      cartesian_stable = cartesian_arrived ? cartesian_stable + 1 : 0;
      if (stable >= o.stable_samples) return MotionResult::ok("joint_follow trajectory completed with measured joint arrival");
      if (cartesian_stable >= o.stable_samples) {
        return MotionResult::ok("joint_follow trajectory completed with measured FK pose arrival; driver pose confirmation still required");
      }
    }
    last_sequence = sequence;
    // Sample using actual elapsed trajectory time; never burst queued old points.
    due = now + o.period_s;
  }
  return MotionResult::fail("joint_follow cancelled");
}
}  // namespace lbot_control
