#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "lbot_control/ros_left_arm_motion_system.hpp"
#include "lbot_control/ros_vision_system.hpp"
#include "lbot_control/task_coordinator.hpp"
#include "rclcpp/executors/single_threaded_executor.hpp"
#include "rclcpp/rclcpp.hpp"

namespace {

using lbot_control::RosLeftArmMotionConfig;

class ExecutorRunner
{
public:
  explicit ExecutorRunner(rclcpp::executors::SingleThreadedExecutor &executor)
  : executor_(executor), thread_([this]() {executor_.spin();}) {}

  ~ExecutorRunner()
  {
    executor_.cancel();
    if (thread_.joinable()) thread_.join();
  }

  ExecutorRunner(const ExecutorRunner &) = delete;
  ExecutorRunner &operator=(const ExecutorRunner &) = delete;

private:
  rclcpp::executors::SingleThreadedExecutor &executor_;
  std::thread thread_;
};

std::array<double, 7> joints(const rclcpp::Node::SharedPtr &node, const std::string &name)
{
  const auto values = node->declare_parameter<std::vector<double>>(name, std::vector<double>{});
  if (values.size() != 7) throw std::runtime_error(name + " must contain exactly 7 values");
  std::array<double, 7> result{};
  std::copy(values.begin(), values.end(), result.begin());
  return result;
}

RosLeftArmMotionConfig load_motion_config(const rclcpp::Node::SharedPtr &node)
{
  RosLeftArmMotionConfig config;
  config.robot_namespace = node->declare_parameter("robot_namespace", config.robot_namespace);
  config.base_frame = node->declare_parameter("base_frame", config.base_frame);
  config.table_route_calibrated = node->declare_parameter("table_route_calibrated", false);
  const auto names = node->declare_parameter<std::vector<std::string>>(
    "route.names", std::vector<std::string>{});
  for (const auto &name : names) config.table_waypoints.push_back({name, joints(node, "route." + name)});
  config.joint_speed = node->declare_parameter("joint_speed", config.joint_speed);
  config.joint_acceleration = node->declare_parameter("joint_acceleration", config.joint_acceleration);
  config.cartesian_speed = node->declare_parameter("cartesian_speed", config.cartesian_speed);
  config.cartesian_acceleration = node->declare_parameter("cartesian_acceleration", config.cartesian_acceleration);
  config.waypoint_tolerance_rad = node->declare_parameter(
    "waypoint_tolerance_rad", config.waypoint_tolerance_rad);
  config.stable_joint_samples = static_cast<std::size_t>(std::max<int64_t>(1,
    node->declare_parameter<int64_t>(
      "stable_joint_samples", static_cast<int64_t>(config.stable_joint_samples))));
  config.waypoint_timeout = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("waypoint_timeout_ms", config.waypoint_timeout.count()));
  config.waypoint_settle = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("waypoint_settle_ms", config.waypoint_settle.count()));
  config.state_timeout = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("state_timeout_ms", config.state_timeout.count()));
  config.service_timeout = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("service_timeout_ms", config.service_timeout.count()));
  config.grip_settle = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("grip_settle_ms", config.grip_settle.count()));
  const auto open = node->declare_parameter<std::vector<int64_t>>(
    "hand_open", std::vector<int64_t>{});
  if (!open.empty()) {
    if (open.size() != 6) throw std::runtime_error("hand_open must contain 6 values");
    for (std::size_t i = 0; i < 6; ++i) config.hand_open[i] = static_cast<uint8_t>(std::clamp<int64_t>(open[i], 0, 255));
  }
  const std::array<std::string, 3> names_by_size{"hand_closed_large", "hand_closed_medium", "hand_closed_small"};
  for (std::size_t target = 0; target < names_by_size.size(); ++target) {
    const auto values = node->declare_parameter<std::vector<int64_t>>(
      names_by_size[target], std::vector<int64_t>{});
    if (values.empty()) continue;
    if (values.size() != 6) throw std::runtime_error(names_by_size[target] + " must contain 6 values");
    for (std::size_t i = 0; i < 6; ++i) {
      config.hand_closed[target][i] = static_cast<uint8_t>(std::clamp<int64_t>(values[i], 0, 255));
    }
  }
  config.hand_speed = static_cast<uint8_t>(std::clamp<int64_t>(
    node->declare_parameter<int64_t>("hand_speed", config.hand_speed), 0, 255));
  config.hand_force = static_cast<uint8_t>(std::clamp<int64_t>(
    node->declare_parameter<int64_t>("hand_force", config.hand_force), 0, 255));
  return config;
}

lbot_control::MotionPlanOptions load_plan_options(const rclcpp::Node::SharedPtr &node)
{
  lbot_control::MotionPlanOptions options;
  node->get_parameter("base_frame", options.base_frame);
  options.pregrasp_height_m = node->declare_parameter("pregrasp_height_m", options.pregrasp_height_m);
  options.lift_height_m = node->declare_parameter("lift_height_m", options.lift_height_m);
  options.slot_release_offset_m = node->declare_parameter("slot_release_offset_m", options.slot_release_offset_m);
  options.tool_roll_rad = node->declare_parameter("tool_roll_rad", options.tool_roll_rad);
  options.tool_pitch_rad = node->declare_parameter("tool_pitch_rad", options.tool_pitch_rad);
  options.tool_yaw_offset_rad = node->declare_parameter("tool_yaw_offset_rad", options.tool_yaw_offset_rad);
  options.tcp_offset_x_m = node->declare_parameter("tcp_offset_x_m", options.tcp_offset_x_m);
  options.tcp_offset_y_m = node->declare_parameter("tcp_offset_y_m", options.tcp_offset_y_m);
  options.tcp_offset_z_m = node->declare_parameter("tcp_offset_z_m", options.tcp_offset_z_m);
  return options;
}

}  // namespace

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("nut_task_controller");
  try {
    const bool execute_task = node->declare_parameter("execute_task", false);
    const std::string task_mode = node->declare_parameter("task_mode", std::string("validate"));
    if (task_mode != "validate" && task_mode != "pregrasp" &&
      task_mode != "return" && task_mode != "full")
    {
      throw std::runtime_error("task_mode must be validate, pregrasp, return, or full");
    }
    if (!execute_task) {
      RCLCPP_INFO(
        node->get_logger(),
        "task controller disabled; no vision wait or robot command was performed (task_mode=%s)",
        task_mode.c_str());
      rclcpp::shutdown();
      return 0;
    }
    const auto motion_config = load_motion_config(node);
    const auto plan_options = load_plan_options(node);
    const bool tool_calibrated = node->declare_parameter("tool_calibrated", false);
    const bool hand_calibrated = node->declare_parameter("hand_calibrated", false);
    const bool vision_calibrated = node->declare_parameter("vision_calibrated", false);

    auto motion = std::make_shared<lbot_control::RosLeftArmMotionSystem>(node, motion_config);
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    ExecutorRunner executor_runner(executor);

    if (task_mode == "return") {
      auto result = motion->prepare_table_route();
      if (result.success) result = motion->execute(lbot_control::MotionStage::ReturnAboveTable);
      if (result.success) result = motion->execute(lbot_control::MotionStage::Retract);
      if (!result.success) {
        RCLCPP_ERROR(node->get_logger(), "calibration return failed: %s", result.message.c_str());
        motion->stop();
      } else {
        RCLCPP_INFO(node->get_logger(), "returned above table and completed RETRACT");
      }
      rclcpp::shutdown();
      return result.success ? 0 : 1;
    }

    lbot_control::RosVisionConfig vision_config;
    node->get_parameter("base_frame", vision_config.base_frame);
    vision_config.sequence_topic = node->declare_parameter("sequence_topic", vision_config.sequence_topic);
    vision_config.slots_topic = node->declare_parameter("slots_topic", vision_config.slots_topic);
    vision_config.event_service = node->declare_parameter("event_service", vision_config.event_service);
    vision_config.wait_timeout = std::chrono::milliseconds(
      node->declare_parameter<int64_t>("vision_wait_timeout_ms", vision_config.wait_timeout.count()));
    vision_config.service_timeout = std::chrono::milliseconds(
      node->declare_parameter<int64_t>("vision_service_timeout_ms", vision_config.service_timeout.count()));

    auto vision = std::make_shared<lbot_control::RosVisionSystem>(node, vision_config);

    if (!vision_calibrated && task_mode != "validate") {
      throw std::runtime_error("vision_calibrated must be true for robot motion");
    }
    const auto observed = vision->initial_scene();
    if (!observed.success) throw std::runtime_error("initial scene failed: " + observed.message);
    const auto planned = lbot_control::build_motion_plan(observed.scene, plan_options);
    if (!planned.success) throw std::runtime_error(planned.message);
    auto result = motion->prepare(planned.plan);
    if (!result.success) throw std::runtime_error(result.message);

    if (task_mode == "validate") {
      RCLCPP_INFO(
        node->get_logger(),
        "vision scene, table route, TCP-adjusted targets, joint limits, and IK validated; no motion sent");
      rclcpp::shutdown();
      return 0;
    }

    if (!tool_calibrated) {
      throw std::runtime_error("tool_calibrated must be true for robot motion");
    }
    if (task_mode == "pregrasp") {
      result = motion->execute(lbot_control::MotionStage::MoveAboveTable);
      if (result.success) {
        result = motion->execute(
          lbot_control::MotionStage::MoveToPregrasp, &planned.plan.targets[0]);
      }
      if (!result.success) {
        RCLCPP_ERROR(node->get_logger(), "pregrasp calibration move failed: %s", result.message.c_str());
        motion->stop();
      } else {
        RCLCPP_INFO(node->get_logger(), "stopped at LARGE nut pregrasp; use task_mode:=return to retract");
      }
      rclcpp::shutdown();
      return result.success ? 0 : 1;
    }

    if (!hand_calibrated) {
      throw std::runtime_error("hand_calibrated must be true for full mode");
    }
    lbot_control::TaskCoordinator coordinator(motion, vision, plan_options);

    result = coordinator.begin();
    while (result.success && !coordinator.state_machine().terminal()) result = coordinator.step();
    if (!result.success) {
      RCLCPP_ERROR(node->get_logger(), "nut task failed: %s", result.message.c_str());
      motion->stop();
    } else {
      RCLCPP_INFO(node->get_logger(), "nut task completed");
    }
    rclcpp::shutdown();
    return result.success ? 0 : 1;
  } catch (const std::exception &error) {
    RCLCPP_ERROR(node->get_logger(), "%s", error.what());
    rclcpp::shutdown();
    return 2;
  }
}
