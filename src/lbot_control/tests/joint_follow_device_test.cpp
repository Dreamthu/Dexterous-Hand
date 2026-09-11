#include <cmath>
#include <iostream>
#include <thread>
#include <stdexcept>
#include "lbot_motion/left_arm_motion_device.hpp"
void require(bool value, const char *why) {if (!value) throw std::runtime_error(why);}
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("joint_follow_device_test");
  auto driver = std::make_shared<rclcpp::Node>("fake_joint_follow_driver");
  const std::string prefix = "/offline_follow_test/left_arm/";
  lbot_motion::LeftArmMotionDevice device(node, "/offline_follow_test",
    std::chrono::milliseconds(500), std::chrono::milliseconds(1000));
  std::atomic<int> commands{0}; std::atomic<bool> valid_command{true};
  auto sub = driver->create_subscription<lbot_arm_interfaces::msg::FollowJoint>(prefix + "joint_follow", 10,
    [&](const lbot_arm_interfaces::msg::FollowJoint &m) {
      valid_command = m.follow && m.joints.size() == 7 && std::abs(m.joints[0]-.1) < 1e-6;
      ++commands;
    });
  auto pub = driver->create_publisher<sensor_msgs::msg::JointState>(prefix + "joint_states", 10);
  auto timer = driver->create_wall_timer(std::chrono::milliseconds(10), [&]() {
    sensor_msgs::msg::JointState message; message.position = {.1, .2, .3, .4, .5, .6, .7}; pub->publish(message);
  });
  rclcpp::executors::SingleThreadedExecutor executor; executor.add_node(node); executor.add_node(driver);
  std::thread thread([&]() {executor.spin();});
  int code = 0;
  try {
    require(device.wait_for_joint_follow_subscriber(), "fake subscriber not discovered");
    require(device.wait_for_fresh_state(), "fake joint feedback not received");
    std::array<double, 7> actual{}; std::uint64_t sequence = 0;
    require(device.fresh_left_joints(actual, std::chrono::milliseconds(100), &sequence) && sequence > 0, "fresh state rejected");
    const auto fresh = device.left_joint_feedback();
    require(fresh.available && fresh.sequence >= sequence && fresh.joints == actual &&
      fresh.age <= std::chrono::milliseconds(100), "feedback diagnostic snapshot inconsistent");
    require(device.joint_follow(actual).success, "follow publication failed");
    for (int i = 0; i < 100 && commands.load() == 0; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(5));
    require(commands == 1 && valid_command, "wrong follow endpoint/units/flag");
    timer->cancel(); std::this_thread::sleep_for(std::chrono::milliseconds(150));
    require(!device.fresh_left_joints(actual, std::chrono::milliseconds(100)), "stale feedback accepted");
    const auto stale = device.left_joint_feedback();
    require(stale.available && stale.joints == actual && stale.age > std::chrono::milliseconds(100),
            "stale sample lost its age or last measured joints");
    sensor_msgs::msg::JointState invalid; invalid.position.assign(7, NAN); pub->publish(invalid);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    require(!device.fresh_left_joints(actual, std::chrono::milliseconds(100)), "NaN refreshed feedback");
    actual[0] = NAN; require(!device.joint_follow(actual).success, "NaN command accepted");
  } catch (const std::exception &e) {std::cerr << e.what() << '\n'; code = 1;}
  executor.cancel(); thread.join(); rclcpp::shutdown(); return code;
}
