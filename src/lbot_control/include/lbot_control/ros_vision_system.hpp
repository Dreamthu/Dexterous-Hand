#ifndef LBOT_CONTROL__ROS_VISION_SYSTEM_HPP_
#define LBOT_CONTROL__ROS_VISION_SYSTEM_HPP_

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include "lbot_control/vision_system.hpp"
#include "lbot_vision/msg/nut_sequence_state.hpp"
#include "lbot_vision/srv/set_nut_state.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lbot_control {

struct RosVisionConfig
{
  std::string base_frame{"base_torso_root"};
  std::string sequence_topic{"/nut_detections/sequence"};
  std::string slots_topic{"/nut_slots"};
  std::string event_service{"/nut_detections/set_state"};
  std::chrono::milliseconds wait_timeout{10000};
  std::chrono::milliseconds service_timeout{3000};
};

// Adapts the running lbot_vision node to the task-level VisionSystem API.
// It consumes structured fixed-ID sequence messages, not the legacy untagged
// PoseArray for nut identity.
class RosVisionSystem final : public VisionSystem
{
public:
  RosVisionSystem(const rclcpp::Node::SharedPtr &node, RosVisionConfig config = {});

  SceneResult initial_scene() override;
  VisionCommandResult start_target(NutSize size) override;
  PickCheckResult check_nut_in_source(NutSize size) override;

private:
  using Sequence = lbot_vision::msg::NutSequenceState;
  using SetNutState = lbot_vision::srv::SetNutState;

  bool valid_frame(const std::string &frame) const;
  bool target_pose(const Sequence &message, std::size_t index, Pose6 &pose) const;
  bool slot_pose(const geometry_msgs::msg::PoseArray &message, std::size_t index, Pose6 &pose) const;
  bool wait_for_observation(std::int64_t previous_stamp_ns, Sequence &sequence,
                            geometry_msgs::msg::PoseArray &slots);
  VisionCommandResult send_event(NutSize size, const std::string &action);
  static Pose6 pose_from_point(double x, double y, double z);

  rclcpp::Node::SharedPtr node_;
  RosVisionConfig config_;
  rclcpp::Subscription<Sequence>::SharedPtr sequence_sub_;
  rclcpp::Subscription<geometry_msgs::msg::PoseArray>::SharedPtr slots_sub_;
  rclcpp::Client<SetNutState>::SharedPtr event_client_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  Sequence::ConstSharedPtr sequence_;
  geometry_msgs::msg::PoseArray::ConstSharedPtr slots_;
  std::uint64_t event_sequence_{0};
  std::int64_t target_start_stamp_ns_{-1};
};

}  // namespace lbot_control

#endif  // LBOT_CONTROL__ROS_VISION_SYSTEM_HPP_
