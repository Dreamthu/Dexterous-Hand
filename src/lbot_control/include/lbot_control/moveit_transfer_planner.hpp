#ifndef LBOT_CONTROL__MOVEIT_TRANSFER_PLANNER_HPP_
#define LBOT_CONTROL__MOVEIT_TRANSFER_PLANNER_HPP_
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "lbot_control/motion_plan.hpp"
#include "lbot_control/joint_follow_trajectory.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lbot_control {
struct MoveItTransferHeightLimits {
  double tip_z;
  double hand_z;
};
struct MoveItTransferPlan {
  bool success{false};
  std::string message;
  Pose6 goal;
  std::vector<TimedJointPoint> points;
  // Preserve these across start resynchronization; never lower the floor on retry.
  std::optional<MoveItTransferHeightLimits> height_limits;
};
// Local planning only: this class has no device clients or motion publisher.
class MoveItTransferPlanner {
public:
  explicit MoveItTransferPlanner(const rclcpp::Node::SharedPtr &node);
  ~MoveItTransferPlanner();
  Pose6 forward_kinematics(const std::array<double, 7> &joints) const;
  MotionResult capture_obstacle_cloud(const std::array<double, 7> &joints);
  MoveItTransferPlan plan(const std::array<double, 7> &start,
    const std::vector<Pose6> &candidates, double timeout_s,
    const std::array<double, 7> *fixed_goal_joints = nullptr,
    const MoveItTransferHeightLimits *height_limits = nullptr);
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace lbot_control
#endif
