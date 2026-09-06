// Copyright (c) 2025  LingSmart Tech
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LBOT_DRIVER_H
#define LBOT_DRIVER_H

#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/clock.hpp" 
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>             
#include <signal.h>             
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <rmw/qos_profiles.h>

#include "lbot_api_cpp.h"

// ROS2 标准消息类型
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/empty.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include "std_msgs/msg/u_int8_multi_array.hpp"

// 自定义 Message 类型
#include "lbot_arm_interfaces/msg/arm_state.hpp"
#include "lbot_arm_interfaces/msg/lbot_pose.hpp"
#include "lbot_arm_interfaces/msg/lbot_frame.hpp"
#include "lbot_arm_interfaces/msg/follow_joint.hpp"

// 自定义 Service 类型
#include "lbot_arm_interfaces/srv/change_frame.hpp"
#include "lbot_arm_interfaces/srv/delete_frame.hpp"
#include "lbot_arm_interfaces/srv/forward_kinematics.hpp"
#include "lbot_arm_interfaces/srv/inverse_kinematics.hpp"
#include "lbot_arm_interfaces/srv/move_c.hpp"
#include "lbot_arm_interfaces/srv/move_j.hpp"
#include "lbot_arm_interfaces/srv/move_jp.hpp"
#include "lbot_arm_interfaces/srv/move_l.hpp"
#include "lbot_arm_interfaces/srv/set_frame.hpp"
#include "lbot_arm_interfaces/srv/set_string.hpp"
#include "lbot_arm_interfaces/srv/set_zero.hpp"
#include "lbot_arm_interfaces/srv/set_enable.hpp"
#include "lbot_arm_interfaces/srv/set_emergency.hpp"
#include "lbot_arm_interfaces/srv/get_frame.hpp"
#include "lbot_arm_interfaces/srv/get_current_frame.hpp"
#include "lbot_arm_interfaces/srv/get_all_frames.hpp"

#define RAD_DEGREE 57.295791433
#define DEGREE_RAD 0.01745329252

using namespace std::chrono_literals;

// 状态回调函数
void lbot_state_callback_wrapper(const lbot_full_state_t* state);
void lbot_error_callback_wrapper(int error_code, const char* error_msg);

// 全局连接状态枚举
enum class GlobalConnState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

// 全局变量
extern bool lbot_ctrl_flag;
extern lbot::LbotApi lbot_api;
extern lbot_handle_t *lbot_handle;
extern std::atomic<GlobalConnState> g_conn_state;  // 全局连接状态

namespace lbot_driver {

// 主节点 - 负责连接管理、状态发布、心跳重连、关节跟随
class LBot: public rclcpp::Node
{
public:
    LBot(const std::string& node_name = "lbot_main_node");
    ~LBot();

    // 线程安全的状态缓存
    std::mutex state_mutex_, conn_mutex_;
    lbot_full_state_t current_lbot_state_;
    // 节点关闭状态变量
    std::atomic<bool> shutting_down_{false};
    // 单例类指针
    static LBot* g_instance;

    // 重连机制相关线程与变量
    std::thread reconnect_thread_;
    std::atomic<bool> reconnect_thread_running_{false};
    std::mutex reconnect_thread_mutex_;

    void disconnect_robot();

private:
    // 初始化和连接相关
    bool connect_robot();
    void get_robot_info();
    void state_publish_timer_callback();

    // 心跳与重连机制回调函数
    void heartbeat_timer_callback();
    void reconnect_timer_callback();

    // 关节跟随订阅回调函数（移到主节点，避免阻塞）
    void left_joint_follow_callback(const lbot_arm_interfaces::msg::FollowJoint::SharedPtr msg);
    void right_joint_follow_callback(const lbot_arm_interfaces::msg::FollowJoint::SharedPtr msg);

    /****************************** 发布器 ******************************/
    
    // 状态发布器 (50Hz)
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr left_joint_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr right_joint_pub_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr left_pose_pub_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr right_pose_pub_;

    /****************************** 订阅器 ******************************/
    
    // 关节跟随订阅器 (用于遥操作) - 移到主节点
    rclcpp::Subscription<lbot_arm_interfaces::msg::FollowJoint>::SharedPtr left_joint_follow_sub_;
    rclcpp::Subscription<lbot_arm_interfaces::msg::FollowJoint>::SharedPtr right_joint_follow_sub_;

    /****************************** 定时器 ******************************/
    
    // 状态发布定时器 (50Hz)
    rclcpp::TimerBase::SharedPtr state_publish_timer_;
    rclcpp::TimerBase::SharedPtr heartbeat_timer_;
    rclcpp::TimerBase::SharedPtr reconnect_timer_;

    // 参数
    std::string arm_ip_ = "192.168.10.21";

    // 回调组
    rclcpp::CallbackGroup::SharedPtr callback_group_timer_;
    rclcpp::CallbackGroup::SharedPtr callback_group_subscribers_;

    // 连接状态
    bool is_state_monitor_started_ = false;

    GlobalConnState conn_state_ = GlobalConnState::DISCONNECTED;

    // 心跳计数器（连续失败次数）
    int heartbeat_fail_count_ = 0;
};

// 左臂服务节点 - 负责左臂所有服务
class LeftArmServiceNode : public rclcpp::Node
{
public:
    LeftArmServiceNode(const std::string& node_name = "lbot_left_arm_node");
    ~LeftArmServiceNode() = default;

private:
    rclcpp::CallbackGroup::SharedPtr callback_group_, callback_group_subscribers_;
    
    void create_services();

    // 灵巧手设置回调函数
    void left_hand_l6_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void left_hand_l6_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void left_hand_l6_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void left_hand_l10_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void left_hand_l10_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void left_hand_l10_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    
    /****************************** 连接状态检查函数 ******************************/
    
    bool check_connection_state(const std::string& service_name) {
        if (g_conn_state.load() != GlobalConnState::CONNECTED) {
            RCLCPP_WARN(this->get_logger(), "%s: Robot not connected, service rejected", service_name.c_str());
            return false;
        }
        return true;
    }

    /****************************** 左臂服务回调函数 ******************************/
    
    // 运动控制
    void move_joint_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Response> response);
    
    void move_pose_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Response> response);
    
    void move_linear_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Response> response);

    // 运动学计算
    void forward_kinematics_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Response> response);
    
    void inverse_kinematics_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Response> response);

    // 工具坐标系管理
    void set_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Response> response);
    
    void get_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Response> response);

    void get_current_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Response> response);
    
    void change_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Response> response);
    
    void delete_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Response> response);
    
    void get_all_tool_frames_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Response> response);

    // 系统设置
    void set_zero_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Response> response);
    void set_enable_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Response> response);
    void set_emergency_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Response> response);

    /****************************** 服务器 ******************************/
    
    // 运动控制服务器
    rclcpp::Service<lbot_arm_interfaces::srv::MoveJ>::SharedPtr move_joint_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::MoveJP>::SharedPtr move_pose_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::MoveL>::SharedPtr move_linear_service_;

    // 运动学计算服务器
    rclcpp::Service<lbot_arm_interfaces::srv::ForwardKinematics>::SharedPtr forward_kinematics_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::InverseKinematics>::SharedPtr inverse_kinematics_service_;

    // 工具坐标系管理服务器
    rclcpp::Service<lbot_arm_interfaces::srv::SetFrame>::SharedPtr set_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetFrame>::SharedPtr get_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetCurrentFrame>::SharedPtr get_current_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::ChangeFrame>::SharedPtr change_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::DeleteFrame>::SharedPtr delete_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetAllFrames>::SharedPtr get_all_tool_frames_service_;

    // 系统设置服务器
    rclcpp::Service<lbot_arm_interfaces::srv::SetZero>::SharedPtr set_zero_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::SetEnable>::SharedPtr set_enable_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::SetEmergency>::SharedPtr set_emergency_service_;

    /****************************** 灵巧手Topic ******************************/

    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l6_joint_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l6_force_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l6_speed_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l10_joint_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l10_force_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_l10_speed_sub_;
};

// 右臂服务节点 - 负责右臂所有服务
class RightArmServiceNode : public rclcpp::Node
{
public:
    RightArmServiceNode(const std::string& node_name = "lbot_right_arm_node");
    ~RightArmServiceNode() = default;

private:
    rclcpp::CallbackGroup::SharedPtr callback_group_, callback_group_subscribers_;
    
    void create_services();

    // 灵巧手设置回调函数
    void right_hand_l6_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void right_hand_l6_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void right_hand_l6_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void right_hand_l10_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void right_hand_l10_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    void right_hand_l10_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
    
    /****************************** 连接状态检查函数 ******************************/
    
    bool check_connection_state(const std::string& service_name) {
        if (g_conn_state.load() != GlobalConnState::CONNECTED) {
            RCLCPP_WARN(this->get_logger(), "%s: Robot not connected, service rejected", service_name.c_str());
            return false;
        }
        return true;
    }

    /****************************** 右臂服务回调函数 ******************************/
    
    // 运动控制
    void move_joint_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Response> response);
    
    void move_pose_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Response> response);
    
    void move_linear_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Response> response);

    // 运动学计算
    void forward_kinematics_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Response> response);
    
    void inverse_kinematics_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Response> response);

    // 工具坐标系管理
    void set_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Response> response);
    
    void get_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Response> response);

    void get_current_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Response> response);
    
    void change_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Response> response);
    
    void delete_tool_frame_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Response> response);
    
    void get_all_tool_frames_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Response> response);

    // 系统设置
    void set_zero_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Response> response);
    void set_enable_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Response> response);
    void set_emergency_callback(
        const std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Request> request,
        std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Response> response);

    /****************************** 服务器 ******************************/
    
    // 运动控制服务器
    rclcpp::Service<lbot_arm_interfaces::srv::MoveJ>::SharedPtr move_joint_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::MoveJP>::SharedPtr move_pose_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::MoveL>::SharedPtr move_linear_service_;

    // 运动学计算服务器
    rclcpp::Service<lbot_arm_interfaces::srv::ForwardKinematics>::SharedPtr forward_kinematics_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::InverseKinematics>::SharedPtr inverse_kinematics_service_;

    // 工具坐标系管理服务器
    rclcpp::Service<lbot_arm_interfaces::srv::SetFrame>::SharedPtr set_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetFrame>::SharedPtr get_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetCurrentFrame>::SharedPtr get_current_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::ChangeFrame>::SharedPtr change_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::DeleteFrame>::SharedPtr delete_tool_frame_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::GetAllFrames>::SharedPtr get_all_tool_frames_service_;

    // 系统设置服务器
    rclcpp::Service<lbot_arm_interfaces::srv::SetZero>::SharedPtr set_zero_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::SetEnable>::SharedPtr set_enable_service_;
    rclcpp::Service<lbot_arm_interfaces::srv::SetEmergency>::SharedPtr set_emergency_service_;

    /****************************** 灵巧手Topic ******************************/

    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l6_joint_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l6_force_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l6_speed_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l10_joint_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l10_force_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr right_hand_l10_speed_sub_;
};

}  // namespace lbot_driver

#endif // LBOT_DRIVER_H
