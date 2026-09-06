// lbot_teleoperation.cpp
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include "lbot_arm_interfaces/srv/move_j.hpp"
#include "lbot_arm_interfaces/msg/follow_joint.hpp"
#include <vector>
#include <algorithm>
#include <atomic>
#include <array>
#include <signal.h>
#include <chrono>
#include <thread>

#include "teleopmasterarm.h"

using namespace telebot;

// -------------------- 全局变量 --------------------
static std::atomic<bool> g_running{true};
static std::atomic<bool> left_ready{false};
static std::atomic<bool> right_ready{false};

// 主臂映射配置
// static constexpr std::array<int64_t, 14> NEGATION = 
//     {-1, -1, -1, -1, -1, -1, -1, 
//     -1, -1, -1, -1, -1, -1, -1};

static constexpr std::array<int64_t, 14> NEGATION = 
    {1,1,1,-1,1,-1,1,
    1,1,1,1,1,-1,1};

// -------------------- 信号处理 --------------------
void signal_handler(int)
{
    g_running = false;
}

// -------------------- 类型转换函数 --------------------
std::vector<float> convertToFloatVector(const std::vector<double>& double_vec) {
    std::vector<float> float_vec;
    float_vec.reserve(double_vec.size());
    std::transform(double_vec.begin(), double_vec.end(), std::back_inserter(float_vec),
                   [](double d) { return static_cast<float>(d); });
    return float_vec;
}

// -------------------- MoveJ 工具函数 --------------------
bool call_movej(
        rclcpp::Node::SharedPtr node,
        rclcpp::Client<lbot_arm_interfaces::srv::MoveJ>::SharedPtr client,
        const std::vector<double>& joints,
        double speed = 5, double acce = 1.0)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::MoveJ::Request>();
    req->speed = speed;
    req->acce = acce;
    req->block = true;
    req->joints = convertToFloatVector(joints);

    auto future = client->async_send_request(req);
    auto status = rclcpp::spin_until_future_complete(node, future);

    if (status == rclcpp::FutureReturnCode::SUCCESS) {
        auto result = future.get();
        return result->success;
    }
    return false;
}

// -------------------- 主臂跟随线程函数 --------------------
void master_arm_follow_thread(
    rclcpp::Node::SharedPtr node,
    TeleopMasterArm& teleArm,
    rclcpp::Publisher<lbot_arm_interfaces::msg::FollowJoint>::SharedPtr left_follow_pub,
    rclcpp::Publisher<lbot_arm_interfaces::msg::FollowJoint>::SharedPtr right_follow_pub)
{
    RCLCPP_INFO(node->get_logger(), "Master arm follow thread started");
    bool follow_mode = false;
    
    // 创建50Hz定时器频率
    auto rate = std::chrono::milliseconds(20); // 50Hz
    
    while (g_running && rclcpp::ok()) {
        auto start_time = std::chrono::steady_clock::now();
        
        if (!left_ready || !right_ready) {
            std::this_thread::sleep_for(rate);
            continue;
        }
        
        // 获取主臂关节角度
        std::vector<float> master_pos = teleArm.getJointPosition();
        std::vector<bool> master_err = teleArm.getJointErrorCode();
        
        if (master_pos.size() < 14) {
            RCLCPP_WARN(node->get_logger(), "Master arm position size < 14, skip frame");
            std::this_thread::sleep_for(rate);
            continue;
        }
        
        // 检查是否有错误
        bool has_error = false;
        for (size_t i = 0; i < 14 && i < master_err.size(); ++i) {
            if (master_err[i]) {
                has_error = true;
                break;
            }
        }
        
        if (has_error) {
            RCLCPP_WARN_THROTTLE(node->get_logger(), *node->get_clock(), 1000, 
                                "Master arm has errors, skip frame");
            std::this_thread::sleep_for(rate);
            continue;
        }
        
        // 准备左臂数据（前7个关节）
        lbot_arm_interfaces::msg::FollowJoint left_follow;
        left_follow.follow = follow_mode;
        left_follow.joints.resize(7);
        for (int i = 0; i < 7; ++i) {
            // 角度转换：度转弧度，并应用映射
            left_follow.joints[i] = static_cast<float>(
                static_cast<double>(master_pos[i]) * NEGATION[i] * M_PI / 180.0
            );
        }
        
        // 准备右臂数据（后7个关节）
        lbot_arm_interfaces::msg::FollowJoint right_follow;
        right_follow.joints.resize(7);
        right_follow.follow = follow_mode;
        for (int i = 7; i < 14; ++i) {
            // 角度转换：度转弧度，并应用映射
            right_follow.joints[i-7] = static_cast<float>(
                static_cast<double>(master_pos[i]) * NEGATION[i] * M_PI / 180.0
            );
        }
        
        // 发布跟随命令
        left_follow_pub->publish(left_follow);
        right_follow_pub->publish(right_follow);
        
        // 保持50Hz频率
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed = end_time - start_time;
        if (elapsed < rate) {
            std::this_thread::sleep_for(rate - elapsed);
        }
    }
    
    RCLCPP_INFO(node->get_logger(), "Master arm follow thread exited");
}

// -------------------- 主程序 --------------------
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("lbot_teleoperation_node");
    
    // 信号处理
    signal(SIGINT, signal_handler);
    
    RCLCPP_INFO(node->get_logger(), "=== LBOT ROS Teleoperation Started ===");
    
    try {
        // -------------------- 初始化主臂 --------------------
        RCLCPP_INFO(node->get_logger(), "Initializing Master Arm...");
        TeleopMasterArm teleArm("can0", 1000000);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        RCLCPP_INFO(node->get_logger(), "Master Arm initialized successfully");
        
        // -------------------- 创建 Publishers --------------------
        auto left_follow_pub = node->create_publisher<lbot_arm_interfaces::msg::FollowJoint>(
                "left_arm/joint_follow", 10);
        auto right_follow_pub = node->create_publisher<lbot_arm_interfaces::msg::FollowJoint>(
                "right_arm/joint_follow", 10);
        
        // -------------------- 创建 MoveJ 客户端 --------------------
        auto left_movej_client = node->create_client<lbot_arm_interfaces::srv::MoveJ>("left_arm/move_joint");
        auto right_movej_client = node->create_client<lbot_arm_interfaces::srv::MoveJ>("right_arm/move_joint");
        
        // 等待服务可用
        RCLCPP_INFO(node->get_logger(), "Waiting for MoveJ services...");
        while (!left_movej_client->wait_for_service(std::chrono::seconds(1)) ||
               !right_movej_client->wait_for_service(std::chrono::seconds(1))) {
            if (!rclcpp::ok() || !g_running) {
                RCLCPP_ERROR(node->get_logger(), "Service wait interrupted");
                return -1;
            }
            RCLCPP_INFO(node->get_logger(), "Waiting for services...");
        }
        RCLCPP_INFO(node->get_logger(), "MoveJ services available");
        
        // -------------------- 初始化：读取主臂当前位置并移动从臂 --------------------
        RCLCPP_INFO(node->get_logger(), "Reading initial master arm position...");
        
        std::vector<float> master_pos = teleArm.getJointPosition();
        std::vector<bool> master_err = teleArm.getJointErrorCode();
        
        if (master_pos.size() < 14) {
            RCLCPP_ERROR(node->get_logger(), "Master arm position size invalid: %zu", master_pos.size());
            return -1;
        }
        
        // 准备从臂初始位置
        std::vector<double> left_joints(7);
        std::vector<double> right_joints(7);
        
        // 左臂赋值（前7个）
        for (int i = 0; i < 7; ++i) {
            left_joints[i] = static_cast<double>(master_pos[i]) * NEGATION[i] * M_PI / 180.0;
        }
        
        // 右臂赋值（后7个）
        for (int i = 7; i < 14; ++i) {
            right_joints[i-7] = static_cast<double>(master_pos[i]) * NEGATION[i] * M_PI / 180.0;
        }
        
        // 执行初始 MoveJ
        RCLCPP_INFO(node->get_logger(), "Moving slave arms to initial position...");
        
        bool left_ok = call_movej(node, left_movej_client, left_joints, 5, 1.0);
        bool right_ok = call_movej(node, right_movej_client, right_joints, 5, 1.0);
        
        if (!left_ok || !right_ok) {
            RCLCPP_ERROR(node->get_logger(), "Initial MoveJ failed: Left=%s, Right=%s", 
                        left_ok ? "OK" : "FAILED", right_ok ? "OK" : "FAILED");
            return -1;
        }
        
        RCLCPP_INFO(node->get_logger(), "Initial positioning completed successfully");
        
        // -------------------- 启动跟随模式 --------------------
        left_ready = true;
        right_ready = true;
        
        RCLCPP_INFO(node->get_logger(), "Starting real-time teleoperation (50Hz)...");
        
        // 启动主臂跟随线程
        std::thread follow_thread(master_arm_follow_thread, node, std::ref(teleArm), 
                                left_follow_pub, right_follow_pub);
        
        // -------------------- ROS 循环 --------------------
        while (rclcpp::ok() && g_running) {
            rclcpp::spin_some(node);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // -------------------- 清理 --------------------
        RCLCPP_INFO(node->get_logger(), "Shutting down teleoperation...");
        
        if (follow_thread.joinable()) {
            follow_thread.join();
        }
        
        left_ready = false;
        right_ready = false;
        
        RCLCPP_INFO(node->get_logger(), "Teleoperation stopped successfully");
        
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node->get_logger(), "Exception occurred: %s", e.what());
        return -1;
    }
    
    rclcpp::shutdown();
    return 0;
}
