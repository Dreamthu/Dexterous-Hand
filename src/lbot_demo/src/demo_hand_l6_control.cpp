#include <chrono>
#include <iostream>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

using namespace std::chrono_literals;

/* 构造 UInt8MultiArray */
std_msgs::msg::UInt8MultiArray
make_msg(const std::vector<uint8_t>& data)
{
    std_msgs::msg::UInt8MultiArray msg;
    msg.data = data;
    return msg;
}

/* 同步发布工具函数 */
void publish_both(
    const rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr& left_pub,
    const rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr& right_pub,
    const std::vector<uint8_t>& data)
{
    auto msg = make_msg(data);
    left_pub->publish(msg);
    right_pub->publish(msg);
}

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_dual_hand_l6_control");

    /* ========== 左右手 publishers ========== */

    auto left_speed_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_speed", 10);
    auto right_speed_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_speed", 10);

    auto left_force_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_force", 10);
    auto right_force_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_force", 10);

    auto left_joint_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_joint", 10);
    auto right_joint_pub = node->create_publisher<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_joint", 10);

    /* 给订阅端一些准备时间 */
    rclcpp::sleep_for(500ms);

    /* ================== 设置速度 ================== */
    std::cout << "\n>>> 设置速度（左右手同步）" << std::endl;
    std::vector<uint8_t> speeds = {250, 250, 250, 250, 250, 250};
    publish_both(left_speed_pub, right_speed_pub, speeds);

    std::cout << "发送速度: ";
    for (auto v : speeds) std::cout << int(v) << " ";
    std::cout << std::endl;

    rclcpp::sleep_for(500ms);

    /* ================== 设置扭矩 ================== */
    std::cout << "\n>>> 设置扭矩（左右手同步）" << std::endl;
    std::vector<uint8_t> torques = {250, 250, 250, 250, 250, 250};
    publish_both(left_force_pub, right_force_pub, torques);

    std::cout << "发送扭矩: ";
    for (auto v : torques) std::cout << int(v) << " ";
    std::cout << std::endl;

    rclcpp::sleep_for(500ms);

    /* ================== 设置关节角度 ================== */
    std::cout << "\n>>> 设置关节角度（左右手同步）" << std::endl;
    // 控制参数：大拇指弯曲，大拇指横摆，食指，中指，无名指，小拇指
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "\n===== 第 " << i + 1 << " 次 =====" << std::endl;

        /* ---------- 第一阶段 ---------- */
        std::vector<uint8_t> pos0 = {128, 128, 128, 128, 128, 128};
        publish_both(left_joint_pub, right_joint_pub, pos0);

        std::cout << "发送角度: ";
        for (auto v : pos0) std::cout << int(v) << " ";
        std::cout << std::endl;

        rclcpp::sleep_for(2s);

        /* ---------- 第二阶段 ---------- */
        std::vector<uint8_t> pos250 = {250, 250, 250, 250, 250, 250};
        publish_both(left_joint_pub, right_joint_pub, pos250);

        std::cout << "发送角度: ";
        for (auto v : pos250) std::cout << int(v) << " ";
        std::cout << std::endl;

        rclcpp::sleep_for(2s);
    }

    std::cout << "\n=== 双手同步测试结束 ===" << std::endl;

    rclcpp::shutdown();
    return 0;
}
