#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "lbot_control/ros_vision_system.hpp"

using namespace std::chrono_literals;

namespace {
void require(bool condition, const char *message)
{
  if (!condition) throw std::runtime_error(message);
}
}  // namespace

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  try {
    // In-process publishers exercise callback ordering without a robot or
    // dependence on cross-process DDS discovery.
    auto options = rclcpp::NodeOptions().use_intra_process_comms(true);
    auto node = std::make_shared<rclcpp::Node>("vision_scene_test", options);
    lbot_control::RosVisionConfig config;
    const auto prefix = "/vision_scene_test_" + std::to_string(
      std::chrono::steady_clock::now().time_since_epoch().count());
    config.sequence_topic = prefix + "/sequence";
    config.slots_topic = prefix + "/slots";
    config.large_target_topic = prefix + "/large";
    config.event_service = prefix + "/event";
    config.wait_timeout = 1000ms;
    lbot_control::RosVisionSystem vision(node, config);
    using Sequence = lbot_vision::msg::NutSequenceState;
    auto sequences = node->create_publisher<Sequence>(config.sequence_topic, 10);
    auto slots = node->create_publisher<geometry_msgs::msg::PoseArray>(config.slots_topic, 10);
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    auto result = std::async(std::launch::async, [&]() {return vision.initial_scene();});
    auto still_waiting = [&]() {
      executor.spin_some();
      require(result.wait_for(10ms) == std::future_status::timeout,
              "interim or invalid scene ended the wait");
    };

    Sequence sequence;
    sequence.header.stamp.sec = 1;
    sequence.initialized = true;
    sequence.observation_valid = true;
    sequence.expected_count = 3;
    sequence.current_target_id = 1;
    sequence.targets.resize(3);
    for (std::size_t i = 0; i < 3; ++i) {
      auto &target = sequence.targets[i];
      target.id = i + 1;
      target.visible = true;
      target.position.header = sequence.header;
      target.position.header.frame_id = "base_link";
      target.position.point.x = 0.1 * (i + 1);
      target.position.point.z = 0.2;
    }
    geometry_msgs::msg::PoseArray slot_message;
    slot_message.header.stamp.sec = 1;
    slot_message.header.frame_id = "base_link";
    slot_message.poses.resize(3);
    slots->publish(slot_message);
    sequences->publish(sequence);  // 2D identity is published before positions.
    still_waiting();

    for (auto &target : sequence.targets) target.position_valid = true;
    slot_message.header.stamp.sec = 2;
    slots->publish(slot_message);
    executor.spin_some();
    sequences->publish(sequence);
    still_waiting();  // Never combine observations from different frames.

    slot_message.header.stamp.sec = 1;
    slot_message.header.frame_id = "camera_color_optical_frame";
    slots->publish(slot_message);
    still_waiting();

    sequence.targets[0].position.point.x = std::numeric_limits<double>::quiet_NaN();
    sequences->publish(sequence);
    executor.spin_some();
    slot_message.header.frame_id = "base_link";
    slots->publish(slot_message);
    still_waiting();

    sequence.targets[0].position.point.x = 0.1;
    sequences->publish(sequence);
    executor.spin_some();
    require(result.wait_for(500ms) == std::future_status::ready, "valid scene was not accepted");
    const auto scene = result.get();
    require(scene.success && scene.scene.frame_id == "base_link", "valid scene result is wrong");
    require(scene.scene.targets[0].nut.x == 0.1, "scene used invalid or stale coordinates");
    auto large = node->create_publisher<geometry_msgs::msg::PointStamped>(config.large_target_topic, 1);
    auto fresh = std::async(std::launch::async, [&]() {return vision.capture_large_target();});
    require(fresh.wait_for(20ms) == std::future_status::timeout,
            "single-target capture reused the old full scene");
    geometry_msgs::msg::PointStamped point;
    point.header.stamp = node->now();
    point.header.frame_id = "camera_color_optical_frame";
    point.point.x = 0.12;
    large->publish(point);
    executor.spin_some();
    require(fresh.wait_for(10ms) == std::future_status::timeout,
            "camera-frame target accepted");
    point.header.frame_id = "base_link";
    point.point.x = std::numeric_limits<double>::quiet_NaN();
    large->publish(point);
    executor.spin_some();
    require(fresh.wait_for(10ms) == std::future_status::timeout, "invalid target accepted");
    point.point.x = 0.12;
    point.header.stamp.sec -= 2;
    large->publish(point);
    executor.spin_some();
    require(fresh.wait_for(10ms) == std::future_status::timeout, "stale target accepted");
    point.header.stamp = node->now();
    large->publish(point);
    executor.spin_some();
    require(fresh.wait_for(500ms) == std::future_status::ready, "single valid target did not finish capture");
    const auto saved = fresh.get();
    require(saved.success && saved.pose.x == 0.12, "single target capture is incorrect");
    point.point.x = 0.5;
    large->publish(point);
    executor.spin_some();
    require(saved.pose.x == 0.12, "later vision changed the saved target");
    auto slot_capture = std::async(std::launch::async, [&]() {return vision.capture_farthest_slot();});
    require(slot_capture.wait_for(20ms) == std::future_status::timeout,
            "slot capture reused an old scene");
    auto reject_slots = [&]() {
      slots->publish(slot_message);
      executor.spin_some();
      require(slot_capture.wait_for(10ms) == std::future_status::timeout,
              "invalid or incomplete slot observation was accepted");
    };
    slot_message.header.stamp = node->now();
    slot_message.poses.resize(2);
    reject_slots();
    slot_message.poses.resize(3);
    slot_message.header.frame_id = "camera_link";
    reject_slots();
    slot_message.header.frame_id = "base_link";
    slot_message.header.stamp.sec -= 2;
    reject_slots();
    slot_message.header.stamp = node->now();
    slot_message.poses[2].position.x = std::numeric_limits<double>::quiet_NaN();
    reject_slots();
    slot_message.poses[0].position.x = 0.65;
    slot_message.poses[0].position.y = 0.1;
    slot_message.poses[1].position.x = 0.2;
    slot_message.poses[1].position.y = -0.7;
    slot_message.poses[2].position.x = 0.3;
    slot_message.poses[2].position.y = -0.4;
    slots->publish(slot_message);
    executor.spin_some();
    require(slot_capture.wait_for(500ms) == std::future_status::ready,
            "one valid three-slot observation did not complete capture");
    const auto saved_slot = slot_capture.get();
    require(saved_slot.success && saved_slot.index == 2 && saved_slot.pose.y == -0.7,
            "farthest slot must use base XY distance, not index or X alone");
    slot_message.poses[1].position.y = 0.0;
    slots->publish(slot_message);
    executor.spin_some();
    require(saved_slot.pose.y == -0.7, "later vision changed the saved slot");
    auto max_x = std::async(std::launch::async, [&]() {return vision.capture_max_x_slot();});
    require(max_x.wait_for(20ms) == std::future_status::timeout, "max X capture reused old slots");
    slot_message.poses[1].position.y = -0.7;
    slot_message.header.stamp = node->now();
    slots->publish(slot_message);
    executor.spin_some();
    require(max_x.wait_for(500ms) == std::future_status::ready, "max X capture did not finish");
    const auto selected_x = max_x.get();
    require(selected_x.success && selected_x.index == 1 && selected_x.pose.x == 0.65,
            "max X selection used radial distance or array index");
    rclcpp::shutdown();
    std::cout << "Vision scene wait accepts coherent 3D results after interim updates\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    rclcpp::shutdown();
    return 1;
  }
}
