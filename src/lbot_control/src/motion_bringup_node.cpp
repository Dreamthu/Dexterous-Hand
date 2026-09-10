#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "lbot_control/ros_left_arm_motion_system.hpp"
#include "lbot_control/table_route.hpp"
#include "rclcpp/executors/single_threaded_executor.hpp"
#include "rclcpp/rclcpp.hpp"

namespace {

using lbot_control::MotionResult;
using lbot_control::MotionStage;
using lbot_control::RosLeftArmMotionConfig;

std::array<double, 7> joint_parameter(
  const rclcpp::Node::SharedPtr &node, const std::string &name)
{
  const auto values = node->declare_parameter<std::vector<double>>(
    name, std::vector<double>{});
  if (values.size() != 7) {
    throw std::runtime_error(name + " must contain exactly 7 joint angles");
  }
  std::array<double, 7> result{};
  std::copy(values.begin(), values.end(), result.begin());
  return result;
}

RosLeftArmMotionConfig load_config(const rclcpp::Node::SharedPtr &node)
{
  RosLeftArmMotionConfig config;
  config.robot_namespace = node->declare_parameter("robot_namespace", config.robot_namespace);
  config.table_route_calibrated = node->declare_parameter("table_route_calibrated", false);
  const auto names = node->declare_parameter<std::vector<std::string>>(
    "route.names", std::vector<std::string>{});
  for (const auto &name : names) {
    config.table_waypoints.push_back({name, joint_parameter(node, "route." + name)});
  }

  config.joint_speed = node->declare_parameter("joint_speed", config.joint_speed);
  config.joint_acceleration = node->declare_parameter(
    "joint_acceleration", config.joint_acceleration);
  config.waypoint_tolerance_rad = node->declare_parameter(
    "waypoint_tolerance_rad", config.waypoint_tolerance_rad);
  config.stable_joint_samples = static_cast<std::size_t>(node->declare_parameter<int64_t>(
    "stable_joint_samples", static_cast<int64_t>(config.stable_joint_samples)));
  config.waypoint_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "waypoint_timeout_ms", config.waypoint_timeout.count()));
  config.waypoint_settle = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "waypoint_settle_ms", config.waypoint_settle.count()));
  config.state_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "state_timeout_ms", config.state_timeout.count()));
  config.service_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "service_timeout_ms", config.service_timeout.count()));
  config.route_control_hand = node->declare_parameter("route_control_hand", false);
  const auto open = node->declare_parameter<std::vector<int64_t>>(
    "hand_open", std::vector<int64_t>{});
  if (!open.empty()) {
    if (open.size() != 6) throw std::runtime_error("hand_open must contain 6 values");
    for (std::size_t i = 0; i < 6; ++i) {
      if (open[i] < 0 || open[i] > 255) {
        throw std::runtime_error("hand_open values must be in range 0..255");
      }
      config.hand_open[i] = static_cast<uint8_t>(open[i]);
    }
  }
  return config;
}

void print_route(
  const rclcpp::Logger &logger,
  const std::vector<lbot_control::JointWaypoint> &waypoints)
{
  RCLCPP_INFO(logger, "configured %zu table-route waypoints", waypoints.size());
  for (std::size_t index = 0; index < waypoints.size(); ++index) {
    RCLCPP_INFO(logger, "  %zu: %s", index, waypoints[index].name.c_str());
  }
}

int run(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("motion_bringup_node");
  try {
    const bool execute_motion = node->declare_parameter("execute_motion", false);
    const std::string mode = node->declare_parameter("mode", std::string("check"));
    if (mode != "check" && mode != "enter" && mode != "leave" && mode != "round_trip") {
      throw std::runtime_error("mode must be check, enter, leave, or round_trip");
    }

    const auto config = load_config(node);
    lbot_control::TableRouteDefinition definition{config.table_waypoints};
    const auto generated = lbot_control::build_table_route(definition);
    if (!generated.success) throw std::runtime_error(generated.message);
    print_route(node->get_logger(), config.table_waypoints);

    if (!execute_motion || mode == "check") {
      RCLCPP_INFO(
        node->get_logger(), "configuration check only; no robot command was sent (mode=%s)",
        mode.c_str());
      rclcpp::shutdown();
      return 0;
    }
    if (!config.table_route_calibrated) {
      throw std::runtime_error("table_route_calibrated must be true for real execution");
    }

    auto motion = std::make_shared<lbot_control::RosLeftArmMotionSystem>(node, config);
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    std::thread spin_thread([&executor]() {executor.spin();});

    MotionResult result = motion->prepare_table_route();
    if (result.success && (mode == "enter" || mode == "round_trip")) {
      if (config.route_control_hand) result = motion->begin_route_motion();
      if (result.success) result = motion->execute(MotionStage::MoveAboveTable);
      if (result.success && config.route_control_hand) result = motion->finish_route_motion();
    }
    if (result.success && (mode == "leave" || mode == "round_trip")) {
      if (config.route_control_hand) result = motion->begin_route_motion();
      if (result.success) result = motion->execute(MotionStage::Retract);
      if (result.success && config.route_control_hand) result = motion->finish_route_motion();
    }

    if (!result.success) {
      RCLCPP_ERROR(node->get_logger(), "%s", result.message.c_str());
      motion->stop();
    } else {
      RCLCPP_INFO(node->get_logger(), "motion mode '%s' completed", mode.c_str());
    }
    executor.cancel();
    spin_thread.join();
    rclcpp::shutdown();
    return result.success ? 0 : 1;
  } catch (const std::exception &error) {
    RCLCPP_ERROR(node->get_logger(), "%s", error.what());
    rclcpp::shutdown();
    return 2;
  }
}

}  // namespace

int main(int argc, char **argv)
{
  return run(argc, argv);
}
