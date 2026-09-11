// Offline planning entry. No lbot_motion device, driver clients or command publisher.
#include <fstream>
#include <iomanip>
#include <iostream>
#include "lbot_control/moveit_transfer_planner.hpp"
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("moveit_transfer_preview");
  int code = 1;
  try {
    const auto start = node->declare_parameter<std::vector<double>>("preview_start_joints", std::vector<double>{});
    const auto goal = node->declare_parameter<std::vector<double>>("preview_goal_pose", std::vector<double>{});
    const auto slot = node->declare_parameter<std::vector<double>>("preview_slot_xy", std::vector<double>{});
    const double angle = node->declare_parameter("preview_orientation_tolerance_rad", .35);
    const double timeout = node->declare_parameter("preview_timeout_s", 15.);
    const auto file = node->declare_parameter("preview_output", std::string("/tmp/lbot_moveit_trajectory.csv"));
    const bool fk_only = node->declare_parameter("preview_fk_only", false);
    const bool capture_cloud = node->declare_parameter("preview_capture_cloud", false);
    if (start.size() != 7 || (!fk_only && goal.size() != 6)) throw std::runtime_error("preview requires 7 start joints and 6 goal XYZ/RPY");
    std::array<double, 7> joints{}; std::copy(start.begin(), start.end(), joints.begin());
    lbot_control::MoveItTransferPlanner planner(node);
    const auto fk = planner.forward_kinematics(joints);
    std::cout << std::setprecision(12) << "MODEL_FK [" << fk.x << ',' << fk.y << ',' << fk.z << ','
      << fk.roll << ',' << fk.pitch << ',' << fk.yaw << "]\n";
    if (fk_only) {rclcpp::shutdown(); return 0;}
    if (capture_cloud) {
      const auto values = node->declare_parameter<std::vector<double>>("preview_cloud_filter_joints", std::vector<double>{});
      auto filter_joints = joints;
      if (!values.empty()) {
        if (values.size() != 7) throw std::runtime_error("preview_cloud_filter_joints must have seven values");
        std::copy(values.begin(), values.end(), filter_joints.begin());
      }
      const auto captured = planner.capture_obstacle_cloud(filter_joints);
      std::cout << captured.message << '\n';
      if (!captured.success) {rclcpp::shutdown(); return 1;}
    }
    const lbot_control::Pose6 target{goal[0], goal[1], goal[2], goal[3], goal[4], goal[5]};
    std::vector<lbot_control::Pose6> candidates{target};
    if (!slot.empty()) {
      if (slot.size() != 2) throw std::runtime_error("preview_slot_xy must contain two values");
      lbot_control::MotionPlanOptions options; options.approach_target_is_tcp = true;
      options.tcp_offset_x_m = node->declare_parameter("tcp_offset_x_m", 0.);
      options.tcp_offset_y_m = node->declare_parameter("tcp_offset_y_m", 0.);
      options.tcp_offset_z_m = node->declare_parameter("tcp_offset_z_m", 0.);
      candidates = lbot_control::build_slot_transfer_candidates(target, {slot[0], slot[1], 0, 0, 0, 0}, options, angle, &fk);
    }
    const auto fixed = node->declare_parameter<std::vector<double>>("preview_goal_joints", std::vector<double>{});
    std::array<double, 7> goal_joints{};
    if (!fixed.empty()) {
      if (fixed.size() != 7) throw std::runtime_error("preview_goal_joints must have seven values");
      std::copy(fixed.begin(), fixed.end(), goal_joints.begin());
    }
    const auto floor_values = node->declare_parameter<std::vector<double>>("preview_height_floors", std::vector<double>{});
    lbot_control::MoveItTransferHeightLimits floors{};
    if (!floor_values.empty()) {
      if (floor_values.size() != 2) throw std::runtime_error("preview_height_floors must be [tip_z, hand_z]");
      floors = {floor_values[0], floor_values[1]};
    }
    const auto plan = planner.plan(joints, candidates, timeout,
      fixed.empty() ? nullptr : &goal_joints, floor_values.empty() ? nullptr : &floors);
    std::cout << plan.message << '\n';
    if (plan.success) {
      std::ofstream out(file);
      if (!out) throw std::runtime_error("cannot write preview_output");
      out << "time_s,j1,j2,j3,j4,j5,j6,j7\n" << std::setprecision(12);
      for (const auto &point : plan.points) {out << point.time_s; for (auto q : point.joints) out << ',' << q; out << '\n';}
      std::cout << "GOAL [" << plan.goal.x << ',' << plan.goal.y << ',' << plan.goal.z << ','
        << plan.goal.roll << ',' << plan.goal.pitch << ',' << plan.goal.yaw << "] CSV=" << file << '\n';
      code = 0;
    }
  } catch (const std::exception &e) {std::cerr << e.what() << '\n';}
  rclcpp::shutdown(); return code;
}
