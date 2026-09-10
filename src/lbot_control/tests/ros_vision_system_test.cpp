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
    rclcpp::shutdown();
    std::cout << "Vision scene wait accepts coherent 3D results after interim updates\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    rclcpp::shutdown();
    return 1;
  }
}
