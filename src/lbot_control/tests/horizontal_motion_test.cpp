#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <thread>
#include "lbot_control/ros_left_arm_motion_system.hpp"

using namespace std::chrono_literals;
void require(bool ok, const char *message)
{
  if (!ok) throw std::runtime_error(message);
}

struct Runner
{
  rclcpp::executors::SingleThreadedExecutor &executor;
  std::thread thread;
  explicit Runner(rclcpp::executors::SingleThreadedExecutor &e)
  : executor(e), thread([&e]() {e.spin();}) {}
  ~Runner() {executor.cancel(); thread.join();}
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  try {
    using namespace lbot_arm_interfaces::srv;
    auto options = rclcpp::NodeOptions().use_intra_process_comms(true);
    auto node = std::make_shared<rclcpp::Node>("horizontal_motion_test", options);
    auto driver = std::make_shared<rclcpp::Node>("horizontal_fake_driver", options);
    lbot_control::RosLeftArmMotionConfig config;
    config.robot_namespace = "/horizontal_test_" + std::to_string(
      std::chrono::steady_clock::now().time_since_epoch().count());
    config.table_route_calibrated = true;
    config.table_waypoints = {{"natural", {0,0,0,0,0,0,0}},
                             {"above", {-0.7,0,0,-0.8,1.5,0,0}}};
    config.waypoint_settle = 0ms;
    config.stable_joint_samples = 2;
    config.waypoint_timeout = 2000ms;
    config.service_timeout = 2000ms;
    config.state_timeout = 1000ms;
    const auto prefix = config.robot_namespace + "/left_arm/";
    std::mutex mutex;
    std::array<double, 7> state{};
    std::atomic<int> movej_count{0}, movel_count{0}, movejp_count{0}, ik_count{0}, failure{0};
    auto states = driver->create_publisher<sensor_msgs::msg::JointState>(prefix + "joint_states", 10);
    auto timer = driver->create_wall_timer(5ms, [&]() {
      sensor_msgs::msg::JointState message;
      std::lock_guard<std::mutex> lock(mutex);
      message.position.assign(state.begin(), state.end());
      states->publish(message);
    });
    auto movej = driver->create_service<MoveJ>(prefix + "move_joint",
      [&](const MoveJ::Request::SharedPtr request, MoveJ::Response::SharedPtr response) {
        std::lock_guard<std::mutex> lock(mutex);
        std::copy(request->joints.begin(), request->joints.end(), state.begin());
        ++movej_count;
        response->success = true;
      });
    MoveJP::Request last_pose;
    auto movel = driver->create_service<MoveL>(prefix + "move_linear",
      [&](const MoveL::Request::SharedPtr, MoveL::Response::SharedPtr response) {
        ++movel_count;
        response->success = true;
      });
    auto movejp = driver->create_service<MoveJP>(prefix + "move_pose",
      [&](const MoveJP::Request::SharedPtr request, MoveJP::Response::SharedPtr response) {
        last_pose = *request;
        ++movejp_count;
        response->success = failure != 6;
      });
    auto fk = driver->create_service<ForwardKinematics>(prefix + "forward_kinematics",
      [&](const ForwardKinematics::Request::SharedPtr, ForwardKinematics::Response::SharedPtr response) {
        response->success = failure != 4;
        response->position.x = failure == 5 ? std::numeric_limits<double>::quiet_NaN() : 0.4;
        response->position.y = 0.2;
        response->position.z = -0.24;
        response->euler.x = 1.5;
        response->euler.y = -0.07;
        response->euler.z = -1.56;
      });
    auto ik = driver->create_service<InverseKinematics>(prefix + "inverse_kinematics",
      [&](const InverseKinematics::Request::SharedPtr request, InverseKinematics::Response::SharedPtr response) {
        ++ik_count;
        response->success = failure != 1;
        response->joints = request->joints;
        if (failure == 2) response->joints[1] = 0.5;
        if (failure == 3) response->joints[0] += 0.5;
      });
    lbot_control::RosLeftArmMotionSystem motion(node, config);
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    executor.add_node(driver);
    Runner runner(executor);
    const lbot_control::Pose6 nut{0.48, 0.26, -0.51, 0, 0, 0};
    lbot_control::MotionPlanOptions plan_options;
    require(!motion.approach_at_table_height(nut, plan_options).success, "unprepared route accepted");
    require(motion.prepare_table_route().success, "route preparation failed");
    require(motion.approach_at_table_height(nut, plan_options).success, "dry validation failed");
    require(movej_count == 0 && movel_count == 0 && movejp_count == 0 && ik_count == 0,
            "target preview sent a motion or IK command");
    require(!motion.approach_at_table_height(nut, plan_options, true).success,
            "approach started before taught route was reached");
    require(motion.execute(lbot_control::MotionStage::MoveAboveTable).success, "MoveJ route failed");
    require(movej_count == 1, "taught route was not executed exactly once");
    for (int mode : {4, 5}) {
      failure = mode;
      require(!motion.approach_at_table_height(nut, plan_options, true).success,
              "invalid FK was accepted");
      require(movejp_count == 0 && movel_count == 0, "invalid FK still sent motion");
    }
    failure = 1;  // A standalone IK service failure must not gate MoveJP.
    require(motion.approach_at_table_height(nut, plan_options, true).success, "valid approach failed");
    require(movel_count == 0 && movejp_count == 1 && ik_count == 0,
            "approach must use one MoveJP and no standalone IK or MoveL");
    require(std::abs(last_pose.position.x - nut.x) < 1e-9 &&
            std::abs(last_pose.position.y - nut.y) < 1e-9 &&
            std::abs(last_pose.position.z + 0.24) < 1e-9 &&
            std::abs(last_pose.euler.x - 1.5) < 1e-9 &&
            std::abs(last_pose.euler.y + 0.07) < 1e-9 &&
            std::abs(last_pose.euler.z + 1.56) < 1e-9 && last_pose.block &&
            std::abs(last_pose.speed - config.joint_speed) < 1e-6,
            "MoveJP changed the target pose, speed, or blocking flag");
    failure = 6;
    require(!motion.approach_at_table_height(nut, plan_options, true).success,
            "MoveJP rejection was ignored");
    require(movejp_count == 2 && movel_count == 0 && ik_count == 0,
            "MoveJP rejection triggered a fallback or retry");
    std::cout << "Horizontal approach mock-service checks passed\n";
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
