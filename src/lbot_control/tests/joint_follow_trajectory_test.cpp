#include <iostream>
#include <limits>
#include <stdexcept>
#include "lbot_control/joint_follow_trajectory.hpp"
using namespace lbot_control;
void require(bool value, const char *why) {if (!value) throw std::runtime_error(why);}
int main() {
  try {
    for (int mode = 0; mode < 12; ++mode) {
      double now = 0; std::size_t sends = 0; std::uint64_t sequence = 0;
      std::array<double, 7> actual{};
      std::vector<TimedJointPoint> points{{0, {}}, {1, {.1, .02, -.03, 0, 0, 0, 0}}};
      if (mode == 1) actual[0] = .2;
      if (mode == 2) points[1].joints[0] = std::numeric_limits<double>::quiet_NaN();
      if (mode == 3) points[1].time_s = 0;
      JointFollowCallbacks c;
      c.now = [&]() {return now;};
      c.sleep_until = [&](double due) {now = due + ((mode == 4 && sends > 5) ? .2 : 0);};
      c.running = [&]() {return mode != 5 || now < .3;};
      c.read = [&](auto &q, auto &seq) {
        if (mode == 6 && sends > 5) return false;
        q = actual; seq = mode == 8 ? 1 : ++sequence; return true;
      };
      c.send = [&](const auto &q) {
        ++sends;
        if (mode == 7 && sends > 5) return false;
        if (mode != 9) actual = q;
        if (mode == 10 || mode == 11) actual[6] += .006;
        return true;
      };
      if (mode >= 4 && mode <= 9) {
        c.cartesian_goal_reached = [](const auto &) {return true;};
      } else if (mode == 10 || mode == 11) {
        c.cartesian_goal_reached = [&](const auto &q) {
          require(now >= points.back().time_s, "Cartesian arrival checked before trajectory completion");
          return mode == 10 && q[0] >= .1-1e-9;
        };
      }
      const auto result = run_joint_follow(points, {}, c);
      require(result.success == (mode == 0 || mode == 10), "incorrect follow success/failure");
      if (mode == 1) require(result.message.find("J1 planned=0 actual=0.2 delta=0.2") != std::string::npos,
                            "start mismatch lacks measured/planned joint diagnostic");
      if (mode >= 1 && mode <= 3) require(sends == 0, "invalid start/trajectory sent a command");
      if (mode == 0) {require(sends >= 50, "trajectory not streamed"); require(actual == points.back().joints, "goal missing");}
      if (mode == 10) require(result.message.find("measured FK pose arrival") != std::string::npos,
                              "valid task-space arrival still blocked by redundant joint residual");
      if (mode == 11) {
        require(result.message.find("final joints did not settle") != std::string::npos, "wrong pose accepted");
        require(result.message.find("J7 target=0 actual=0.006 delta=0.006") != std::string::npos,
                "final timeout lacks actual joint error");
      }
    }
    std::cout << "joint_follow timing, start, feedback, cancellation, tracking, arrival tests passed\n";
  } catch (const std::exception &e) {std::cerr << e.what() << '\n'; return 1;}
  return 0;
}
