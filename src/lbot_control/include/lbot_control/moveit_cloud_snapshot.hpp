#ifndef LBOT_CONTROL__MOVEIT_CLOUD_SNAPSHOT_HPP_
#define LBOT_CONTROL__MOVEIT_CLOUD_SNAPSHOT_HPP_
#include <memory>
#include <string>
#include <moveit/robot_state/robot_state.hpp>
#include <octomap_msgs/msg/octomap.hpp>
#include <rclcpp/rclcpp.hpp>

namespace lbot_control {
struct MoveItCloudSnapshotResult {
  bool success{false};
  std::string message;
  octomap_msgs::msg::Octomap map;
};
// Read-only perception: capture once before enter, transform at the cloud stamp,
// remove the measured left robot body, and voxelize the observed environment.
class MoveItCloudSnapshot {
public:
  explicit MoveItCloudSnapshot(const rclcpp::Node::SharedPtr &parameters);
  ~MoveItCloudSnapshot();
  bool enabled() const;
  MoveItCloudSnapshotResult capture(const moveit::core::RobotState &robot);
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
}
#endif
