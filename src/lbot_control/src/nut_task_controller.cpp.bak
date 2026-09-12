#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
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

// Declared after ExecutorRunner so exception cleanup keeps ROS callbacks alive.
class TaskCleanup
{
public:
  TaskCleanup(const rclcpp::Node::SharedPtr &node,
    const std::shared_ptr<lbot_control::RosLeftArmMotionSystem> &motion)
  : node_(node), motion_(motion) {}

  ~TaskCleanup()
  {
    if (!finished_) motion_->stop();
  }

  bool finish(bool success)
  {
    if (success) {
      const auto opened = motion_->finish_task();
      if (!opened.success) {
        RCLCPP_ERROR(node_->get_logger(), "task cleanup failed: %s", opened.message.c_str());
        success = false;
      }
    }
    if (!success) motion_->stop();
    finished_ = true;
    return success;
  }

private:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<lbot_control::RosLeftArmMotionSystem> motion_;
  bool finished_{false};
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
  auto read_limits = [&](const char *name, std::array<double, 7> &destination) {
    const auto values = node->declare_parameter<std::vector<double>>(
      name, std::vector<double>(destination.begin(), destination.end()));
    if (values.size() != 7) throw std::runtime_error(std::string(name) + " must contain 7 values");
    std::copy(values.begin(), values.end(), destination.begin());
  };
  read_limits("left_joint_min", config.left_joint_min);
  read_limits("left_joint_max", config.left_joint_max);
  config.joint_limit_margin_rad = node->declare_parameter(
    "joint_limit_margin_rad", config.joint_limit_margin_rad);
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
  config.route_control_hand = node->declare_parameter("route_control_hand", false);
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
  config.approach_grasp_enabled = node->declare_parameter("approach_grasp_enabled", false);
  config.approach_grasp_z_m = node->declare_parameter("approach_grasp_z_m", config.approach_grasp_z_m);
  config.approach_grasp_z_frame = node->declare_parameter("approach_grasp_z_frame", config.approach_grasp_z_frame);
  config.pose_tolerance_m = node->declare_parameter("pose_tolerance_m", config.pose_tolerance_m);
  config.pose_tolerance_rad = node->declare_parameter("pose_tolerance_rad", config.pose_tolerance_rad);
  config.pose_arrival_timeout = std::chrono::milliseconds(
    node->declare_parameter<int64_t>("pose_arrival_timeout_ms", config.pose_arrival_timeout.count()));
  const auto pose_samples = node->declare_parameter<int64_t>("pose_stable_samples", config.pose_stable_samples);
  if (pose_samples <= 0) throw std::runtime_error("pose_stable_samples must be positive");
  config.pose_stable_samples = static_cast<std::size_t>(pose_samples);
  auto read_hand = [&](const std::string &name, std::array<uint8_t, 6> &destination) {
    const auto values = node->declare_parameter<std::vector<int64_t>>(
      name, std::vector<int64_t>(destination.begin(), destination.end()));
    if (values.size() != 6 || !std::all_of(values.begin(), values.end(),
        [](int64_t value) {return value >= 0 && value <= 255;})) {
      throw std::runtime_error(name + " must contain exactly six values in 0..255");
    }
    std::copy(values.begin(), values.end(), destination.begin());
  };
  read_hand("approach_hand_ready", config.approach_hand_ready);
  read_hand("approach_hand_close", config.approach_hand_close);
  config.approach_place_enabled = node->declare_parameter("approach_place_enabled", false);
  config.approach_place_lift_m = node->declare_parameter("approach_place_lift_m", config.approach_place_lift_m);
  const auto transfer_waypoint = node->declare_parameter<std::vector<double>>(
    "approach_place_waypoint", std::vector<double>{});
  const auto transfer_names = node->declare_parameter<std::vector<std::string>>(
    "approach_place_route.names", std::vector<std::string>{});
  if (!transfer_waypoint.empty() && !transfer_names.empty()) {
    throw std::runtime_error("use approach_place_route or legacy approach_place_waypoint, not both");
  }
  for (std::size_t i = 0; i < transfer_names.size(); ++i) {
    const auto &name = transfer_names[i];
    if (name.empty() || std::find(transfer_names.begin(), transfer_names.begin() + i, name) !=
        transfer_names.begin() + i) {
      throw std::runtime_error("approach_place_route.names must be nonempty and unique");
    }
    config.approach_place_waypoints.push_back({name, joints(node, "approach_place_route." + name)});
  }
  if (!transfer_waypoint.empty()) {
    if (transfer_waypoint.size() != 7) throw std::runtime_error("approach_place_waypoint must contain 7 joint radians");
    std::array<double, 7> waypoint{};
    std::copy(transfer_waypoint.begin(), transfer_waypoint.end(), waypoint.begin());
    config.approach_place_waypoints.push_back({"legacy", waypoint});
  }
  config.approach_place_max_orientation_change_rad = node->declare_parameter(
    "approach_place_max_orientation_change_rad", config.approach_place_max_orientation_change_rad);
  config.approach_place_planning_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "approach_place_planning_timeout_ms", config.approach_place_planning_timeout.count()));
  config.approach_place_release_enabled = node->declare_parameter(
    "approach_place_release_enabled", config.approach_place_release_enabled);
  config.slot_transfer_planner = node->declare_parameter("slot_transfer_planner", config.slot_transfer_planner);
  config.joint_follow_rate_hz = node->declare_parameter("joint_follow_rate_hz", config.joint_follow_rate_hz);
  config.joint_follow_start_tolerance_rad = node->declare_parameter(
    "joint_follow_start_tolerance_rad", config.joint_follow_start_tolerance_rad);
  config.joint_follow_tracking_tolerance_rad = node->declare_parameter(
    "joint_follow_tracking_tolerance_rad", config.joint_follow_tracking_tolerance_rad);
  config.joint_follow_goal_tolerance_rad = node->declare_parameter(
    "joint_follow_goal_tolerance_rad", config.joint_follow_goal_tolerance_rad);
  config.joint_follow_feedback_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "joint_follow_feedback_timeout_ms", config.joint_follow_feedback_timeout.count()));
  config.joint_follow_max_lateness = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "joint_follow_max_lateness_ms", config.joint_follow_max_lateness.count()));
  config.joint_follow_settle_tolerance_rad = node->declare_parameter(
    "joint_follow_settle_tolerance_rad", config.joint_follow_settle_tolerance_rad);
  config.joint_follow_settle_duration = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "joint_follow_settle_duration_ms", config.joint_follow_settle_duration.count()));
  config.joint_follow_settle_timeout = std::chrono::milliseconds(node->declare_parameter<int64_t>(
    "joint_follow_settle_timeout_ms", config.joint_follow_settle_timeout.count()));
  config.joint_follow_start_replans = static_cast<int>(node->declare_parameter<int64_t>(
    "joint_follow_start_replans", config.joint_follow_start_replans));
  read_hand("approach_hand_release", config.approach_hand_release);
  if (config.approach_place_enabled && (!config.approach_grasp_enabled ||
      !std::isfinite(config.approach_place_lift_m) || config.approach_place_lift_m <= 0.0 ||
      config.approach_place_lift_m > 2.0)) {
    throw std::runtime_error("placement requires approach_grasp_enabled and lift in (0, 2] m");
  }
  if (config.approach_grasp_enabled &&
      (!std::isfinite(config.approach_grasp_z_m) || config.grip_settle.count() < 0 ||
       (config.approach_grasp_z_frame != "palm" && config.approach_grasp_z_frame != "arm_tip") ||
       !std::isfinite(config.pose_tolerance_m) || config.pose_tolerance_m <= 0.0 ||
       !std::isfinite(config.pose_tolerance_rad) || config.pose_tolerance_rad <= 0.0 ||
       config.pose_arrival_timeout.count() <= 0)) {
    throw std::runtime_error("approach grasp height/frame or arrival/settling parameters are invalid");
  }
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
  options.approach_target_is_tcp = node->declare_parameter("approach_target_is_tcp", false);
  const auto reference = node->declare_parameter<std::vector<double>>(
    "approach_reference_z_rpy", std::vector<double>{});
  if (!reference.empty()) {
    if (reference.size() != 4 || !std::all_of(reference.begin(), reference.end(),
        [](double value) {return std::isfinite(value);})) {
      throw std::runtime_error("approach_reference_z_rpy must contain finite [z, roll, pitch, yaw]");
    }
    options.approach_reference_z_rpy = std::array<double, 4>{
      reference[0], reference[1], reference[2], reference[3]};
  }
  const auto placement_reference = node->declare_parameter<std::vector<double>>(
    "approach_place_reference_z_rpy", std::vector<double>{});
  if (!placement_reference.empty()) {
    if (placement_reference.size() != 4 || !std::all_of(placement_reference.begin(), placement_reference.end(),
        [](double value) {return std::isfinite(value);})) {
      throw std::runtime_error("approach_place_reference_z_rpy must contain finite Arm_Tip [z, roll, pitch, yaw]");
    }
    options.approach_place_reference_z_rpy = std::array<double, 4>{
      placement_reference[0], placement_reference[1], placement_reference[2], placement_reference[3]};
  }
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
    if (task_mode != "validate" && task_mode != "validate_pregrasp" && task_mode != "pregrasp" &&
      task_mode != "return" && task_mode != "full")
    {
      throw std::runtime_error("task_mode must be validate, validate_pregrasp, pregrasp, return, or full");
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
    const auto slot_selection = node->declare_parameter("approach_place_slot_selection", std::string("farthest"));
    if (slot_selection != "max_x" && slot_selection != "farthest") {
      throw std::runtime_error("approach_place_slot_selection must be max_x or farthest");
    }
    const bool tool_calibrated = node->declare_parameter("tool_calibrated", false);
    const bool hand_calibrated = node->declare_parameter("hand_calibrated", false);
    const bool vision_calibrated = node->declare_parameter("vision_calibrated", false);

    auto motion = std::make_shared<lbot_control::RosLeftArmMotionSystem>(node, motion_config);
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    ExecutorRunner executor_runner(executor);
    TaskCleanup cleanup(node, motion);

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
      const bool succeeded = cleanup.finish(result.success);
      rclcpp::shutdown();
      return succeeded ? 0 : 1;
    }

    lbot_control::RosVisionConfig vision_config;
    node->get_parameter("base_frame", vision_config.base_frame);
    vision_config.sequence_topic = node->declare_parameter("sequence_topic", vision_config.sequence_topic);
    vision_config.slots_topic = node->declare_parameter("slots_topic", vision_config.slots_topic);
    vision_config.large_target_topic = node->declare_parameter("large_target_topic", vision_config.large_target_topic);
    vision_config.event_service = node->declare_parameter("event_service", vision_config.event_service);
    vision_config.wait_timeout = std::chrono::milliseconds(
      node->declare_parameter<int64_t>("vision_wait_timeout_ms", vision_config.wait_timeout.count()));
    vision_config.service_timeout = std::chrono::milliseconds(
      node->declare_parameter<int64_t>("vision_service_timeout_ms", vision_config.service_timeout.count()));

    auto vision = std::make_shared<lbot_control::RosVisionSystem>(node, vision_config);

    if (!vision_calibrated && task_mode != "validate" && task_mode != "validate_pregrasp") {
      throw std::runtime_error("vision_calibrated must be true for robot motion");
    }
    if (task_mode == "pregrasp" || task_mode == "validate_pregrasp") {
      const bool move = task_mode == "pregrasp";
      if (move && !tool_calibrated) {
        throw std::runtime_error("tool_calibrated must be true for robot motion");
      }
      lbot_control::MotionResult result;
      try {
        result = motion->prepare_table_route();
        lbot_control::LargeTargetResult captured;
        lbot_control::SlotTargetResult saved_slot;
        if (result.success) {
          RCLCPP_INFO(node->get_logger(),
            "waiting for one current LARGE nut position before enter; no sequence confirmation required");
          captured = vision->capture_large_target();
          if (!captured.success) {
            result = lbot_control::MotionResult::fail(captured.message);
          } else {
            RCLCPP_INFO(node->get_logger(), "saved LARGE target in %s: xyz=[%.4f,%.4f,%.4f]",
              vision_config.base_frame.c_str(), captured.pose.x, captured.pose.y, captured.pose.z);
          }
        }
        if (result.success && motion_config.approach_place_enabled) {
          RCLCPP_INFO(node->get_logger(),
            "waiting for one current three-slot observation before enter; selection=%s", slot_selection.c_str());
          saved_slot = slot_selection == "max_x" ? vision->capture_max_x_slot() : vision->capture_farthest_slot();
          if (!saved_slot.success) {
            result = lbot_control::MotionResult::fail(saved_slot.message);
          } else {
            RCLCPP_INFO(node->get_logger(),
              "saved %s slot_%zu in %s: xyz=[%.6f,%.6f,%.6f], base XY distance=%.6f m; no vision after enter",
              slot_selection.c_str(), saved_slot.index, vision_config.base_frame.c_str(), saved_slot.pose.x, saved_slot.pose.y,
              saved_slot.pose.z, std::hypot(saved_slot.pose.x, saved_slot.pose.y));
          }
        }
        if (result.success && move && motion_config.approach_place_enabled) {
          result = motion->capture_transfer_obstacles();
        }
        if (result.success && move) {
          RCLCPP_INFO(node->get_logger(),
            "starting taught MoveJ enter route; MoveJP target will be computed after settling");
          if (motion_config.route_control_hand) result = motion->begin_route_motion();
          if (result.success) result = motion->execute(lbot_control::MotionStage::MoveAboveTable);
          if (result.success && motion_config.route_control_hand) result = motion->finish_route_motion();
        }
        if (result.success) {
          RCLCPP_INFO(node->get_logger(),
            "preparing MoveJP to the saved LARGE target; no further vision wait");
          result = motion->approach_at_table_height(captured.pose, plan_options, move,
            motion_config.approach_place_enabled ? &saved_slot.pose : nullptr);
        }
      } catch (const std::exception &error) {
        result = lbot_control::MotionResult::fail(error.what());
      }
      if (!result.success) {
        RCLCPP_ERROR(node->get_logger(), "LARGE grasp/place sequence failed: %s", result.message.c_str());
        motion->stop();
      } else {
        RCLCPP_INFO(node->get_logger(), "%s", move ?
          (motion_config.approach_place_enabled ?
            (motion_config.approach_place_release_enabled ?
              "LARGE sequence finished at saved slot and released; grasp/release not sensor-verified" :
              "LARGE sequence finished at saved slot; slot release disabled; task cleanup will open hand") :
           motion_config.approach_grasp_enabled ?
            "LARGE sequence finished: approach, ready hand, vertical MoveL, close hand; stopped without lift; grasp not sensor-verified" :
            "reached approach target above saved LARGE nut; no descent or grasp; use task_mode:=return to retract") :
          "taught route and configured approach/grasp/placement targets checked; no commands sent; reachability/path not validated");
      }
      const bool succeeded = cleanup.finish(result.success);
      rclcpp::shutdown();
      return succeeded ? 0 : 1;
    }
    RCLCPP_INFO(node->get_logger(), "waiting for confirmed 3D vision scene (timeout=%ld ms)",
      static_cast<long>(vision_config.wait_timeout.count()));
    const auto observed = vision->initial_scene();
    if (!observed.success) throw std::runtime_error("initial scene failed: " + observed.message);
    const auto planned = lbot_control::build_motion_plan(observed.scene, plan_options);
    if (!planned.success) throw std::runtime_error(planned.message);
    RCLCPP_INFO(node->get_logger(), "vision scene accepted; checking table route and IK");
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
    const bool succeeded = cleanup.finish(result.success);
    rclcpp::shutdown();
    return succeeded ? 0 : 1;
  } catch (const std::exception &error) {
    RCLCPP_ERROR(node->get_logger(), "%s", error.what());
    rclcpp::shutdown();
    return 2;
  }
}
