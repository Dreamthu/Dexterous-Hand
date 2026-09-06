// Copyright (c) 2025  LinkerRobot Tech
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

#include "lbot_driver.h"
#include <std_srvs/srv/empty.hpp>

using namespace std::chrono_literals;
using lbot_driver::LBot;

// 全局变量
lbot::LbotApi lbot_api;
lbot_handle_t *lbot_handle{};
LBot* LBot::g_instance = nullptr;
std::atomic<GlobalConnState> g_conn_state{GlobalConnState::DISCONNECTED};  // 全局连接状态

// 静态状态回调函数
void lbot_state_callback_wrapper(const lbot_full_state_t* state) {
    if (state && lbot_driver::LBot::g_instance) {
        std::lock_guard<std::mutex> lock(lbot_driver::LBot::g_instance->state_mutex_);
        lbot_driver::LBot::g_instance->current_lbot_state_ = *state;
    }
}

// 错误回调函数
void lbot_error_callback_wrapper(int error_code, const char* error_msg) {
    if (LBot::g_instance && LBot::g_instance->shutting_down_) return; 

    if (error_msg) {
        RCLCPP_ERROR(
            rclcpp::get_logger("lbot_driver"),
            "LBot Error [%d]: %s",
            error_code,
            error_msg
        );
    } else {
        RCLCPP_ERROR(
            rclcpp::get_logger("lbot_driver"),
            "LBot Error [%d]: (null message)",
            error_code
        );
    }
}

namespace lbot_driver {

// ========== 主节点实现 ==========
LBot::LBot(const std::string& node_name) : rclcpp::Node(node_name) {
    // 设置独特的logger名称
    this->get_logger() = rclcpp::get_logger(node_name);

    // 参数初始化
    this->declare_parameter("arm_ip", "192.168.10.21");
    arm_ip_ = this->get_parameter("arm_ip").as_string();

    // 全局指针
    g_instance = this;

    // 创建回调组
    callback_group_timer_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    callback_group_subscribers_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    // 创建发布器 (50Hz状态发布)
    left_joint_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("left_arm/joint_states", 10);
    right_joint_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("right_arm/joint_states", 10);
    left_pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("left_arm/pose_states", 10);
    right_pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("right_arm/pose_states", 10);

    // 创建关节跟随订阅器（避免阻塞服务）
    auto sub_opt = rclcpp::SubscriptionOptions();
    sub_opt.callback_group = callback_group_subscribers_;
    
    left_joint_follow_sub_ = this->create_subscription<lbot_arm_interfaces::msg::FollowJoint>(
        "left_arm/joint_follow", 10,
        std::bind(&LBot::left_joint_follow_callback, this, std::placeholders::_1),
        sub_opt);

    right_joint_follow_sub_ = this->create_subscription<lbot_arm_interfaces::msg::FollowJoint>(
        "right_arm/joint_follow", 10,
        std::bind(&LBot::right_joint_follow_callback, this, std::placeholders::_1),
        sub_opt);

    // 心跳与重连定时器
    heartbeat_timer_ = create_wall_timer(1s, std::bind(&LBot::heartbeat_timer_callback, this));
    reconnect_timer_ = create_wall_timer(2s, std::bind(&LBot::reconnect_timer_callback, this));

    // 创建状态发布定时器 (50Hz)
    state_publish_timer_ = this->create_wall_timer(
        20ms, 
        std::bind(&LBot::state_publish_timer_callback, this),
        callback_group_timer_);

    // 连接机器人
    connect_robot();
    
    RCLCPP_INFO(this->get_logger(), "LBot main node initialized successfully");
}

// 析构函数
LBot::~LBot() {
    shutting_down_ = true;

    // 停止 reconnect 线程
    {
        std::lock_guard<std::mutex> lock(reconnect_thread_mutex_);
        if (reconnect_thread_.joinable())
            reconnect_thread_.join();
    }

    disconnect_robot();
}

bool LBot::connect_robot() {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    if(conn_state_ == GlobalConnState::CONNECTED) return true;

    g_conn_state.store(GlobalConnState::CONNECTING);  // 更新全局状态

    lbot_handle = lbot_api.lbot_init(arm_ip_.c_str());
    if (lbot_handle->id > 0) {
        RCLCPP_INFO(this->get_logger(), "Connected to LBot at %s", arm_ip_.c_str());

        // 获取机械臂信息
        get_robot_info();

        // 启动状态监控
        if (lbot_api.lbot_start_state_monitor(lbot_state_callback_wrapper, lbot_error_callback_wrapper)) {
            is_state_monitor_started_ = true;
            RCLCPP_INFO(this->get_logger(), "State monitor started successfully");
        } else {
            is_state_monitor_started_ = false;
            RCLCPP_ERROR(this->get_logger(), "Failed to start state monitor");
        }

    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to connect to LBot");
    }

    bool success = lbot_handle->id > 0;
    conn_state_ = success ? GlobalConnState::CONNECTED : GlobalConnState::DISCONNECTED;
    g_conn_state.store(conn_state_);  // 更新全局状态

    return success;
}

void LBot::disconnect_robot() {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    if (conn_state_ == GlobalConnState::DISCONNECTED) return;

    RCLCPP_INFO(this->get_logger(), "Disconnecting from LBot...");
    
    // 先设置状态，避免其他线程继续操作
    conn_state_ = GlobalConnState::DISCONNECTED;
    g_conn_state.store(GlobalConnState::DISCONNECTED);  // 更新全局状态
    
    // 停止状态监控
    if (is_state_monitor_started_) {
        RCLCPP_INFO(this->get_logger(), "Stopping state monitor...");
        lbot_api.lbot_stop_state_monitor();
        is_state_monitor_started_ = false;
    }
    
    // 清理连接
    RCLCPP_INFO(this->get_logger(), "Cleaning up LBot connection...");
    lbot_api.lbot_cleanup();
    
    RCLCPP_INFO(this->get_logger(), "Disconnected from LBot successfully");
}

void LBot::get_robot_info() {
    std::string robot_model, controller_version;
    if (lbot_api.lbot_get_controller_info(lbot_handle, robot_model, controller_version)) {
        RCLCPP_INFO(this->get_logger(), "Robot Model: %s, Controller Version: %s", robot_model.c_str(), controller_version.c_str());
    }
}

// 状态发布定时器回调函数 (50Hz)
void LBot::state_publish_timer_callback() {
    if (conn_state_ != GlobalConnState::CONNECTED || !is_state_monitor_started_) return;

    lbot_full_state_t state;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        state = current_lbot_state_;
    }

    rclcpp::Time time_now = this->get_clock()->now();

    // ----------------------- 左臂 -----------------------

    sensor_msgs::msg::JointState left_joint;
    left_joint.header.stamp = time_now;
    left_joint.header.frame_id = state.left_arm.frame_id; 

    // 关节名称
    left_joint.name.resize(7);
    for (size_t i = 0; i < 7; i++)
        left_joint.name[i] = state.left_arm.name[i];

    // 关节角度
    left_joint.position.resize(7);
    for (size_t i = 0; i < 7; i++)
        left_joint.position[i] = state.left_arm.joint_position[i];

    // 关节速度
    left_joint.velocity.resize(7);
    for (size_t i = 0; i < 7; i++)
        left_joint.velocity[i] = state.left_arm.velocity[i];

    // 关节力矩 effort
    left_joint.effort.resize(7);
    for (size_t i = 0; i < 7; i++)
        left_joint.effort[i] = state.left_arm.effort[i];

    left_joint_pub_->publish(left_joint);

    // PoseStamped（末端状态）
    geometry_msgs::msg::PoseStamped left_pose;
    left_pose.header.stamp = time_now;
    left_pose.header.frame_id = state.left_arm.frame_id;

    left_pose.pose.position.x = state.left_arm.end_effector_position.x;
    left_pose.pose.position.y = state.left_arm.end_effector_position.y;
    left_pose.pose.position.z = state.left_arm.end_effector_position.z;

    left_pose.pose.orientation.x = state.left_arm.orientation.x;
    left_pose.pose.orientation.y = state.left_arm.orientation.y;
    left_pose.pose.orientation.z = state.left_arm.orientation.z;
    left_pose.pose.orientation.w = state.left_arm.orientation.w;

    left_pose_pub_->publish(left_pose);

    // ----------------------- 右臂 -----------------------

    sensor_msgs::msg::JointState right_joint;
    right_joint.header.stamp = time_now;
    right_joint.header.frame_id = state.right_arm.frame_id;

    right_joint.name.resize(7);
    for (size_t i = 0; i < 7; i++)
        right_joint.name[i] = state.right_arm.name[i];

    right_joint.position.resize(7);
    for (size_t i = 0; i < 7; i++)
        right_joint.position[i] = state.right_arm.joint_position[i];

    right_joint.velocity.resize(7);
    for (size_t i = 0; i < 7; i++)
        right_joint.velocity[i] = state.right_arm.velocity[i];

    right_joint.effort.resize(7);
    for (size_t i = 0; i < 7; i++)
        right_joint.effort[i] = state.right_arm.effort[i];

    right_joint_pub_->publish(right_joint);

    geometry_msgs::msg::PoseStamped right_pose;
    right_pose.header.stamp = time_now;
    right_pose.header.frame_id = state.right_arm.frame_id;

    right_pose.pose.position.x = state.right_arm.end_effector_position.x;
    right_pose.pose.position.y = state.right_arm.end_effector_position.y;
    right_pose.pose.position.z = state.right_arm.end_effector_position.z;

    right_pose.pose.orientation.x = state.right_arm.orientation.x;
    right_pose.pose.orientation.y = state.right_arm.orientation.y;
    right_pose.pose.orientation.z = state.right_arm.orientation.z;
    right_pose.pose.orientation.w = state.right_arm.orientation.w;

    right_pose_pub_->publish(right_pose);
}


/****************************** 关节跟随订阅器（遥操作） ******************************/

// 左臂关节跟随回调
void LBot::left_joint_follow_callback(const std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint> msg) {
    if (!msg || msg->joints.empty()) return;
    
    if (g_conn_state.load() != GlobalConnState::CONNECTED) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Left joint follow: Robot not connected");
        return;
    }

    std::vector<double> joints(msg->joints.begin(), msg->joints.end());

    if (!lbot_api.lbot_joint_follow(lbot_handle, LBOT_LEFT_ARM, joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to execute left_joint_follow");
    }
}

// 右臂关节跟随回调
void LBot::right_joint_follow_callback(const std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint> msg) {
    if (!msg || msg->joints.empty()) return;
    
    if (g_conn_state.load() != GlobalConnState::CONNECTED) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Right joint follow: Robot not connected");
        return;
    }

    std::vector<double> joints(msg->joints.begin(), msg->joints.end());

    if (!lbot_api.lbot_joint_follow(lbot_handle, LBOT_RIGHT_ARM, joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to execute right_joint_follow");
    }
}

/****************************** 心跳与重连机制 ******************************/

// 心跳机制回调函数
void LBot::heartbeat_timer_callback()
{
    if (conn_state_ != GlobalConnState::CONNECTED)
        return;

    std::string model, version;
    // bool ok = lbot_api.lbot_get_controller_info(lbot_handle, model, version);
    bool ok = true;

    if (!ok)
    {
        heartbeat_fail_count_++;

        RCLCPP_WARN(get_logger(), "Heartbeat failed (%d/3)", heartbeat_fail_count_);

        if (heartbeat_fail_count_ >= 3)
        {
            RCLCPP_ERROR(get_logger(), "Heartbeat lost. Disconnecting...");
            disconnect_robot();
        }
    }
    else
    {
        heartbeat_fail_count_ = 0;
    }
}

// 自动重连机制回调函数
void LBot::reconnect_timer_callback()
{
    if (shutting_down_) return;
    if (conn_state_ != GlobalConnState::DISCONNECTED) return;

    std::lock_guard<std::mutex> lock(reconnect_thread_mutex_);

    if (reconnect_thread_running_)
        return;  // 线程正在运行，不重复启动

    reconnect_thread_running_ = true;

    reconnect_thread_ = std::thread([this]() {
        if (shutting_down_) {
            reconnect_thread_running_ = false;
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Trying to reconnect to LBot...");

        bool success = connect_robot();

        {
            std::lock_guard<std::mutex> lock(conn_mutex_);
            if (success) {
                conn_state_ = GlobalConnState::CONNECTED;
                heartbeat_fail_count_ = 0;

                if (!shutting_down_)
                    RCLCPP_INFO(this->get_logger(), "Arm reconnected successfully");
            } else {
                conn_state_ = GlobalConnState::DISCONNECTED;

                if (!shutting_down_)
                    RCLCPP_WARN(this->get_logger(), "Reconnection failed. Will retry...");
            }
        }

        reconnect_thread_running_ = false;
    });
}

// ========== 左臂服务节点实现 ==========
LeftArmServiceNode::LeftArmServiceNode(const std::string& node_name) : rclcpp::Node(node_name) {
    // 设置独特的logger名称
    this->get_logger() = rclcpp::get_logger(node_name);

    // 创建互斥回调组确保串行执行
    callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    callback_group_subscribers_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    create_services();
    RCLCPP_INFO(this->get_logger(), "Left arm service node initialized");
}

void LeftArmServiceNode::create_services() {
    auto service_qos = rclcpp::QoS(rclcpp::ServicesQoS());
    auto sub_opt = rclcpp::SubscriptionOptions();
    sub_opt.callback_group = callback_group_subscribers_;

    // 运动控制服务
    move_joint_service_ = this->create_service<lbot_arm_interfaces::srv::MoveJ>(
        "left_arm/move_joint",
        std::bind(&LeftArmServiceNode::move_joint_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    move_pose_service_ = this->create_service<lbot_arm_interfaces::srv::MoveJP>(
        "left_arm/move_pose",
        std::bind(&LeftArmServiceNode::move_pose_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    move_linear_service_ = this->create_service<lbot_arm_interfaces::srv::MoveL>(
        "left_arm/move_linear",
        std::bind(&LeftArmServiceNode::move_linear_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 运动学计算服务
    forward_kinematics_service_ = this->create_service<lbot_arm_interfaces::srv::ForwardKinematics>(
        "left_arm/forward_kinematics",
        std::bind(&LeftArmServiceNode::forward_kinematics_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    inverse_kinematics_service_ = this->create_service<lbot_arm_interfaces::srv::InverseKinematics>(
        "left_arm/inverse_kinematics",
        std::bind(&LeftArmServiceNode::inverse_kinematics_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 工具坐标系管理服务
    set_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::SetFrame>(
        "left_arm/set_tool_frame",
        std::bind(&LeftArmServiceNode::set_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    get_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::GetFrame>(
        "left_arm/get_tool_frame",
        std::bind(&LeftArmServiceNode::get_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    get_current_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::GetCurrentFrame>(
        "left_arm/get_current_tool_frame",
        std::bind(&LeftArmServiceNode::get_current_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    change_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::ChangeFrame>(
        "left_arm/change_tool_frame",
        std::bind(&LeftArmServiceNode::change_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    delete_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::DeleteFrame>(
        "left_arm/delete_tool_frame",
        std::bind(&LeftArmServiceNode::delete_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    get_all_tool_frames_service_ = this->create_service<lbot_arm_interfaces::srv::GetAllFrames>(
        "left_arm/get_all_tool_frames",
        std::bind(&LeftArmServiceNode::get_all_tool_frames_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 零点设置服务
    set_zero_service_ = this->create_service<lbot_arm_interfaces::srv::SetZero>(
        "left_arm/set_zero",
        std::bind(&LeftArmServiceNode::set_zero_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);
    set_enable_service_ = this->create_service<lbot_arm_interfaces::srv::SetEnable>(
        "left_arm/set_enable",
        std::bind(&LeftArmServiceNode::set_enable_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);
    set_emergency_service_ = this->create_service<lbot_arm_interfaces::srv::SetEmergency>(
        "left_arm/set_emergency_stop",
        std::bind(&LeftArmServiceNode::set_emergency_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 灵巧手设置Topic回调函数
    left_hand_l6_joint_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_joint", 10,
        std::bind(&LeftArmServiceNode::left_hand_l6_set_joint_callback, this, std::placeholders::_1),
        sub_opt);
    left_hand_l6_force_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_force", 10,
        std::bind(&LeftArmServiceNode::left_hand_l6_set_force_callback, this, std::placeholders::_1),
        sub_opt);
    left_hand_l6_speed_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l6_speed", 10,
        std::bind(&LeftArmServiceNode::left_hand_l6_set_speed_callback, this, std::placeholders::_1),
        sub_opt);
    left_hand_l10_joint_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l10_joint", 10,
        std::bind(&LeftArmServiceNode::left_hand_l10_set_joint_callback, this, std::placeholders::_1),
        sub_opt);
    left_hand_l10_force_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l10_force", 10,
        std::bind(&LeftArmServiceNode::left_hand_l10_set_force_callback, this, std::placeholders::_1),
        sub_opt);
    left_hand_l10_speed_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "left_hand/set_l10_speed", 10,
        std::bind(&LeftArmServiceNode::left_hand_l10_set_speed_callback, this, std::placeholders::_1),
        sub_opt);
}

/****************************** 左臂服务回调函数实现 ******************************/

// 关节运动回调函数
void LeftArmServiceNode::move_joint_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Response> response)
{
    auto start_time = std::chrono::steady_clock::now();
    RCLCPP_INFO(this->get_logger(), "[%ld ms] Left arm move_joint started", 
            std::chrono::duration_cast<std::chrono::milliseconds>(start_time.time_since_epoch()).count());

    if (!check_connection_state("left_arm/move_joint")) {
        response->success = false;
        return;
    }

    if (request->joints.size() != 7) {
        response->success = false;
        RCLCPP_ERROR(this->get_logger(), "Left arm move_joint: size must be 7");
        return;
    }
    double joints[7];
    std::copy(request->joints.begin(), request->joints.end(), joints);
    response->success = lbot_api.lbot_move_joint(lbot_handle, LBOT_LEFT_ARM, joints, request->speed, request->acce, request->block);
    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm move_joint executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Left arm move_joint failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    RCLCPP_INFO(this->get_logger(), "[%ld ms] Left arm move_joint returned after %ld ms", 
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
            duration.count());
}

// 位姿运动回调函数
void LeftArmServiceNode::move_pose_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Response> response)
{
    if (!check_connection_state("left_arm/move_pose")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    response->success = lbot_api.lbot_move_pose(lbot_handle, LBOT_LEFT_ARM, &position, &euler, request->speed, request->acce, request->block);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm move_pose executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Left arm move_pose failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 线性运动回调函数
void LeftArmServiceNode::move_linear_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Response> response)
{
    if (!check_connection_state("left_arm/move_linear")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    response->success = lbot_api.lbot_move_linear(lbot_handle, LBOT_LEFT_ARM, &position, &euler, request->speed, request->acce, request->block);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm move_linear executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Left arm move_linear failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 正运动学回调函数
void LeftArmServiceNode::forward_kinematics_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Response> response)
{
    if (!check_connection_state("left_arm/forward_kinematics")) {
        response->success = false;
        return;
    }

    if (request->joints.size() != 7) {
        response->success = false;
        RCLCPP_ERROR(this->get_logger(), "Left arm forward_kinematics: Joint array must contain exactly 7 values");
        return;
    }

    double joints[7];
    std::copy(request->joints.begin(), request->joints.end(), joints);

    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_forward_kinematics(lbot_handle, LBOT_LEFT_ARM, joints, &position, &euler);

    if (response->success) {
        response->position.x = position.x;
        response->position.y = position.y;
        response->position.z = position.z;

        response->euler.x = euler.x;
        response->euler.y = euler.y;
        response->euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Left arm forward kinematics calculated successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Left arm forward kinematics calculation failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 逆运动学回调函数
void LeftArmServiceNode::inverse_kinematics_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Response> response)
{
    if (!check_connection_state("left_arm/inverse_kinematics")) {
        response->success = false;
        return;
    }

    double initial_joints[7];
    double* initial_ptr = nullptr;

    if (request->joints.empty()) {
        initial_ptr = nullptr;
    } else {
        if (request->joints.size() != 7) {
            response->success = false;
            RCLCPP_ERROR(this->get_logger(), "Left arm inverse_kinematics: Initial joint array must contain exactly 7 values");
            return;
        }
        std::copy(request->joints.begin(), request->joints.end(), initial_joints);
        initial_ptr = initial_joints;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    double result_joints[7];
    response->success = lbot_api.lbot_inverse_kinematics(lbot_handle, LBOT_LEFT_ARM, initial_ptr, &position, &euler, result_joints);

    if (response->success) {
        response->joints.resize(7);
        std::copy(result_joints, result_joints + 7, response->joints.begin());
        RCLCPP_INFO(this->get_logger(), "Left arm inverse kinematics calculated successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Left arm inverse kinematics calculation failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 设置工具坐标系回调函数
void LeftArmServiceNode::set_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Response> response)
{
    if (!check_connection_state("left_arm/set_tool_frame")) {
        response->success = false;
        return;
    }

    const auto &frame = request->frame;

    lbot_position_t position;
    position.x = frame.position.x;
    position.y = frame.position.y;
    position.z = frame.position.z;

    lbot_euler_t euler;
    euler.x = frame.euler.x;
    euler.y = frame.euler.y;
    euler.z = frame.euler.z;

    response->success = lbot_api.lbot_set_tool_frame(lbot_handle, LBOT_LEFT_ARM, frame.name.c_str(), &position, &euler);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm tool frame '%s' set successfully", frame.name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 获取指定工具坐标系回调函数
void LeftArmServiceNode::get_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Response> response)
{
    if (!check_connection_state("left_arm/get_tool_frame")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_get_tool_frame(lbot_handle, LBOT_LEFT_ARM, request->name.c_str(), &position, &euler);

    if (response->success) {
        response->frame.name = request->name;
        response->frame.position.x = position.x;
        response->frame.position.y = position.y;
        response->frame.position.z = position.z;
        response->frame.euler.x = euler.x;
        response->frame.euler.y = euler.y;
        response->frame.euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Left arm tool frame '%s' retrieved successfully", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get left arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 获取当前工具坐标系回调函数
void LeftArmServiceNode::get_current_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Response> response)
{
    (void)request;  

    if (!check_connection_state("left_arm/get_current_tool_frame")) {
        response->success = false;
        return;
    }

    std::string name = ""; 
    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_get_current_tool_frame(
        lbot_handle,
        LBOT_LEFT_ARM,
        name,
        position,
        euler
    );

    if (response->success) {
        response->frame.name = name;
        response->frame.position.x = position.x;
        response->frame.position.y = position.y;
        response->frame.position.z = position.z;
        response->frame.euler.x = euler.x;
        response->frame.euler.y = euler.y;
        response->frame.euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Left arm current tool frame '%s' retrieved successfully", response->name.c_str()
        );
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get current tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str()
        );
    }
}

// 切换工具坐标系回调函数
void LeftArmServiceNode::change_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Response> response)
{
    if (!check_connection_state("left_arm/change_tool_frame")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_change_tool_frame(lbot_handle, LBOT_LEFT_ARM, request->name.c_str());

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm changed to tool frame '%s'", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to change left arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 删除工具坐标系回调函数
void LeftArmServiceNode::delete_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Response> response)
{
    if (!check_connection_state("left_arm/delete_tool_frame")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_delete_tool_frame(lbot_handle, LBOT_LEFT_ARM, request->name.c_str());

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm tool frame '%s' deleted successfully", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to delete left arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 获取所有工具坐标系回调函数
void LeftArmServiceNode::get_all_tool_frames_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Response> response)
{
    (void)request;

    if (!check_connection_state("left_arm/get_all_tool_frames")) {
        response->success = false;
        return;
    }

    std::vector<std::string> frame_names;
    response->success = lbot_api.lbot_get_all_tool_frames(lbot_handle, LBOT_LEFT_ARM, frame_names);

    if (response->success) {
        response->names = frame_names;
        RCLCPP_INFO(this->get_logger(), "Left arm retrieved %zu tool frames", frame_names.size());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get left arm tool frames: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 零点设置回调函数
void LeftArmServiceNode::set_zero_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Response> response)
{
    (void)request;

    if (!check_connection_state("left_arm/set_zero")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_set_zero(lbot_handle, LBOT_LEFT_ARM);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Left arm joint zero set successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left arm zero: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 使能设置回调函数
void LeftArmServiceNode::set_enable_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Response> response)
{
    if (!check_connection_state("left_arm/set_enable")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_enable_arm(lbot_handle, LBOT_LEFT_ARM, request->enable);

    if (response->success) {
        if (request->enable) {
            RCLCPP_INFO(this->get_logger(), "Left arm enable ON successfully");
        } else {
            RCLCPP_INFO(this->get_logger(), "Left arm enable OFF successfully");
        }
    } else {
        if (request->enable) {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to enable (ON) left arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to disable (OFF) left arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        }
    }

}

// 急停设置回调函数
void LeftArmServiceNode::set_emergency_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Response> response)
{
    (void)request;

    if (!check_connection_state("left_arm/set_emergency_stop")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_emergency_stop(lbot_handle, LBOT_LEFT_ARM, request->emergency);

    if (response->success) {
        if (request->emergency) {
            RCLCPP_INFO(this->get_logger(), "Left arm emergency stop activated successfully");
        } else {
            RCLCPP_INFO(this->get_logger(), "Left arm emergency stop released successfully");
        }
    } else {
        if (request->emergency) {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to activate emergency stop for left arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to release emergency stop for left arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        }
    }
}

void LeftArmServiceNode::left_hand_l6_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l6_joint")) {
        return;
    }
    std::vector<uint8_t> hand_joints(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_position(lbot_handle, LBOT_LEFT_ARM, hand_joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand joint");
    }
}

void LeftArmServiceNode::left_hand_l6_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l6_force")) {
        return;
    }
    std::vector<uint8_t> hand_force(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_effort(lbot_handle, LBOT_LEFT_ARM, hand_force)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand force");
    }
}

void LeftArmServiceNode::left_hand_l6_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l6_speed")) {
        return;
    }
    std::vector<uint8_t> hand_speed(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_velocity(lbot_handle, LBOT_LEFT_ARM, hand_speed)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand speed");
    }
}

void LeftArmServiceNode::left_hand_l10_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l10_joint")) {
        return;
    }
    std::vector<uint8_t> hand_joints(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_position(lbot_handle, LBOT_LEFT_ARM, hand_joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand joint");
    }
}

void LeftArmServiceNode::left_hand_l10_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l10_force")) {
        return;
    }
    std::vector<uint8_t> hand_force(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_effort(lbot_handle, LBOT_LEFT_ARM, hand_force)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand force");
    }
}

void LeftArmServiceNode::left_hand_l10_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("left_hand/set_l10_speed")) {
        return;
    }
    std::vector<uint8_t> hand_speed(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_velocity(lbot_handle, LBOT_LEFT_ARM, hand_speed)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set left hand speed");
    }
}

// ========== 右臂服务节点实现 ==========
RightArmServiceNode::RightArmServiceNode(const std::string& node_name) : rclcpp::Node(node_name) {
    // 设置独特的logger名称
    this->get_logger() = rclcpp::get_logger(node_name);
 
    // 创建互斥回调组确保串行执行
    callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    callback_group_subscribers_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    create_services();
    RCLCPP_INFO(this->get_logger(), "Right arm service node initialized");
}

void RightArmServiceNode::create_services() {
    auto service_qos = rclcpp::QoS(rclcpp::ServicesQoS());
    auto sub_opt = rclcpp::SubscriptionOptions();
    sub_opt.callback_group = callback_group_subscribers_;

    // 运动控制服务
    move_joint_service_ = this->create_service<lbot_arm_interfaces::srv::MoveJ>(
        "right_arm/move_joint",
        std::bind(&RightArmServiceNode::move_joint_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    move_pose_service_ = this->create_service<lbot_arm_interfaces::srv::MoveJP>(
        "right_arm/move_pose",
        std::bind(&RightArmServiceNode::move_pose_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    move_linear_service_ = this->create_service<lbot_arm_interfaces::srv::MoveL>(
        "right_arm/move_linear",
        std::bind(&RightArmServiceNode::move_linear_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 运动学计算服务
    forward_kinematics_service_ = this->create_service<lbot_arm_interfaces::srv::ForwardKinematics>(
        "right_arm/forward_kinematics",
        std::bind(&RightArmServiceNode::forward_kinematics_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    inverse_kinematics_service_ = this->create_service<lbot_arm_interfaces::srv::InverseKinematics>(
        "right_arm/inverse_kinematics",
        std::bind(&RightArmServiceNode::inverse_kinematics_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 工具坐标系管理服务
    set_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::SetFrame>(
        "right_arm/set_tool_frame",
        std::bind(&RightArmServiceNode::set_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    get_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::GetFrame>(
        "right_arm/get_tool_frame",
        std::bind(&RightArmServiceNode::get_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);
    
    get_current_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::GetCurrentFrame>(
        "right_arm/get_current_tool_frame",
        std::bind(&RightArmServiceNode::get_current_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    change_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::ChangeFrame>(
        "right_arm/change_tool_frame",
        std::bind(&RightArmServiceNode::change_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    delete_tool_frame_service_ = this->create_service<lbot_arm_interfaces::srv::DeleteFrame>(
        "right_arm/delete_tool_frame",
        std::bind(&RightArmServiceNode::delete_tool_frame_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    get_all_tool_frames_service_ = this->create_service<lbot_arm_interfaces::srv::GetAllFrames>(
        "right_arm/get_all_tool_frames",
        std::bind(&RightArmServiceNode::get_all_tool_frames_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 零点设置服务
    set_zero_service_ = this->create_service<lbot_arm_interfaces::srv::SetZero>(
        "right_arm/set_zero",
        std::bind(&RightArmServiceNode::set_zero_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);
    set_enable_service_ = this->create_service<lbot_arm_interfaces::srv::SetEnable>(
        "right_arm/set_enable",
        std::bind(&RightArmServiceNode::set_enable_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);
    set_emergency_service_ = this->create_service<lbot_arm_interfaces::srv::SetEmergency>(
        "right_arm/set_emergency_stop",
        std::bind(&RightArmServiceNode::set_emergency_callback, this, std::placeholders::_1, std::placeholders::_2),
        service_qos, callback_group_);

    // 灵巧手设置Topic回调函数
    right_hand_l6_joint_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_joint", 10,
        std::bind(&RightArmServiceNode::right_hand_l6_set_joint_callback, this, std::placeholders::_1),
        sub_opt);
    right_hand_l6_force_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_force", 10,
        std::bind(&RightArmServiceNode::right_hand_l6_set_force_callback, this, std::placeholders::_1),
        sub_opt);
    right_hand_l6_speed_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l6_speed", 10,
        std::bind(&RightArmServiceNode::right_hand_l6_set_speed_callback, this, std::placeholders::_1),
        sub_opt);
    
    right_hand_l10_joint_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l10_joint", 10,
        std::bind(&RightArmServiceNode::right_hand_l10_set_joint_callback, this, std::placeholders::_1),
        sub_opt);
    right_hand_l10_force_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l10_force", 10,
        std::bind(&RightArmServiceNode::right_hand_l10_set_force_callback, this, std::placeholders::_1),
        sub_opt);
    right_hand_l10_speed_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
        "right_hand/set_l10_speed", 10,
        std::bind(&RightArmServiceNode::right_hand_l10_set_speed_callback, this, std::placeholders::_1),
        sub_opt);
}

/****************************** 右臂服务回调函数实现 ******************************/

// 关节运动回调函数
void RightArmServiceNode::move_joint_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ::Response> response)
{
    if (!check_connection_state("right_arm/move_joint")) {
        response->success = false;
        return;
    }

    if (request->joints.size() != 7) {
        response->success = false;
        RCLCPP_ERROR(this->get_logger(), "Right arm move_joint: size must be 7");
        return;
    }
    double joints[7];
    std::copy(request->joints.begin(), request->joints.end(), joints);
    response->success = lbot_api.lbot_move_joint(lbot_handle, LBOT_RIGHT_ARM, joints, request->speed, request->acce, request->block);
    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm move_joint executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Right arm move_joint failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 位姿运动回调函数
void RightArmServiceNode::move_pose_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJP::Response> response)
{
    if (!check_connection_state("right_arm/move_pose")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    response->success = lbot_api.lbot_move_pose(lbot_handle, LBOT_RIGHT_ARM, &position, &euler, request->speed, request->acce, request->block);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm move_pose executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Right arm move_pose failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 线性运动回调函数
void RightArmServiceNode::move_linear_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::MoveL::Response> response)
{
    if (!check_connection_state("right_arm/move_linear")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    response->success = lbot_api.lbot_move_linear(lbot_handle, LBOT_RIGHT_ARM, &position, &euler, request->speed, request->acce, request->block);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm move_linear executed successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Right arm move_linear failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 正运动学回调函数
void RightArmServiceNode::forward_kinematics_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::ForwardKinematics::Response> response)
{
    if (!check_connection_state("right_arm/forward_kinematics")) {
        response->success = false;
        return;
    }

    if (request->joints.size() != 7) {
        response->success = false;
        RCLCPP_ERROR(this->get_logger(), "Right arm forward_kinematics: Joint array must contain exactly 7 values");
        return;
    }

    double joints[7];
    std::copy(request->joints.begin(), request->joints.end(), joints);

    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_forward_kinematics(lbot_handle, LBOT_RIGHT_ARM, joints, &position, &euler);

    if (response->success) {
        response->position.x = position.x;
        response->position.y = position.y;
        response->position.z = position.z;

        response->euler.x = euler.x;
        response->euler.y = euler.y;
        response->euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Right arm forward kinematics calculated successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Right arm forward kinematics calculation failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 逆运动学回调函数
void RightArmServiceNode::inverse_kinematics_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::InverseKinematics::Response> response)
{
    if (!check_connection_state("right_arm/inverse_kinematics")) {
        response->success = false;
        return;
    }

    double initial_joints[7];
    double* initial_ptr = nullptr;

    if (request->joints.empty()) {
        initial_ptr = nullptr;
    } else {
        if (request->joints.size() != 7) {
            response->success = false;
            RCLCPP_ERROR(this->get_logger(), "Right arm inverse_kinematics: Initial joint array must contain exactly 7 values");
            return;
        }
        std::copy(request->joints.begin(), request->joints.end(), initial_joints);
        initial_ptr = initial_joints;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    position.x = request->position.x;
    position.y = request->position.y;
    position.z = request->position.z;

    euler.x = request->euler.x;
    euler.y = request->euler.y;
    euler.z = request->euler.z;

    double result_joints[7];
    response->success = lbot_api.lbot_inverse_kinematics(lbot_handle, LBOT_RIGHT_ARM, initial_ptr, &position, &euler, result_joints);

    if (response->success) {
        response->joints.resize(7);
        std::copy(result_joints, result_joints + 7, response->joints.begin());
        RCLCPP_INFO(this->get_logger(), "Right arm inverse kinematics calculated successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Right arm inverse kinematics calculation failed: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 设置工具坐标系回调函数
void RightArmServiceNode::set_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetFrame::Response> response)
{
    if (!check_connection_state("right_arm/set_tool_frame")) {
        response->success = false;
        return;
    }

    const auto &frame = request->frame;

    lbot_position_t position;
    position.x = frame.position.x;
    position.y = frame.position.y;
    position.z = frame.position.z;

    lbot_euler_t euler;
    euler.x = frame.euler.x;
    euler.y = frame.euler.y;
    euler.z = frame.euler.z;

    response->success = lbot_api.lbot_set_tool_frame(lbot_handle, LBOT_RIGHT_ARM, frame.name.c_str(), &position, &euler);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm tool frame '%s' set successfully", frame.name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 获取指定工具坐标系回调函数
void RightArmServiceNode::get_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetFrame::Response> response)
{
    if (!check_connection_state("right_arm/get_tool_frame")) {
        response->success = false;
        return;
    }

    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_get_tool_frame(lbot_handle, LBOT_RIGHT_ARM, request->name.c_str(), &position, &euler);

    if (response->success) {
        response->frame.name = request->name;
        response->frame.position.x = position.x;
        response->frame.position.y = position.y;
        response->frame.position.z = position.z;
        response->frame.euler.x = euler.x;
        response->frame.euler.y = euler.y;
        response->frame.euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Right arm tool frame '%s' retrieved successfully", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get right arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}


// 获取当前工具坐标系回调函数
void RightArmServiceNode::get_current_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetCurrentFrame::Response> response)
{
    (void)request;

    if (!check_connection_state("right_arm/get_current_tool_frame")) {
        response->success = false;
        return;
    }

    std::string name = ""; 
    lbot_position_t position;
    lbot_euler_t euler;

    response->success = lbot_api.lbot_get_current_tool_frame(
        lbot_handle,
        LBOT_RIGHT_ARM,
        name,
        position,
        euler
    );

    if (response->success) {
        response->frame.name = name;
        response->frame.position.x = position.x;
        response->frame.position.y = position.y;
        response->frame.position.z = position.z;
        response->frame.euler.x = euler.x;
        response->frame.euler.y = euler.y;
        response->frame.euler.z = euler.z;

        RCLCPP_INFO(this->get_logger(), "Right arm current tool frame '%s' retrieved successfully", response->name.c_str()
        );
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get current tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str()
        );
    }
}

// 切换工具坐标系回调函数
void RightArmServiceNode::change_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::ChangeFrame::Response> response)
{
    if (!check_connection_state("right_arm/change_tool_frame")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_change_tool_frame(lbot_handle, LBOT_RIGHT_ARM, request->name.c_str());

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm changed to tool frame '%s'", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to change right arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 删除工具坐标系回调函数
void RightArmServiceNode::delete_tool_frame_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::DeleteFrame::Response> response)
{
    if (!check_connection_state("right_arm/delete_tool_frame")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_delete_tool_frame(lbot_handle, LBOT_RIGHT_ARM, request->name.c_str());

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm tool frame '%s' deleted successfully", request->name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to delete right arm tool frame: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 获取所有工具坐标系回调函数
void RightArmServiceNode::get_all_tool_frames_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::GetAllFrames::Response> response)
{
    (void)request;

    if (!check_connection_state("right_arm/get_all_tool_frames")) {
        response->success = false;
        return;
    }

    std::vector<std::string> frame_names;
    response->success = lbot_api.lbot_get_all_tool_frames(lbot_handle, LBOT_RIGHT_ARM, frame_names);

    if (response->success) {
        response->names = frame_names;
        RCLCPP_INFO(this->get_logger(), "Right arm retrieved %zu tool frames", frame_names.size());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to get right arm tool frames: %s", lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 零点设置回调函数
void RightArmServiceNode::set_zero_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetZero::Response> response)
{
    (void)request;

    if (!check_connection_state("right_arm/set_zero")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_set_zero(lbot_handle, LBOT_RIGHT_ARM);

    if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Right arm joint zero set successfully");
    } else {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right arm zero: %s",
                     lbot_api.lbot_get_last_error(lbot_handle).c_str());
    }
}

// 使能设置回调函数
void RightArmServiceNode::set_enable_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetEnable::Response> response)
{
    if (!check_connection_state("right_arm/set_enable")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_enable_arm(lbot_handle, LBOT_RIGHT_ARM, request->enable);

    if (response->success) {
        if (request->enable) {
            RCLCPP_INFO(this->get_logger(), "Right arm enable ON successfully");
        } else {
            RCLCPP_INFO(this->get_logger(), "Right arm enable OFF successfully");
        }
    } else {
        if (request->enable) {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to enable (ON) right arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to disable (OFF) right arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        }
    }
}

// 急停设置回调函数
void RightArmServiceNode::set_emergency_callback(
    const std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Request> request,
    std::shared_ptr<lbot_arm_interfaces::srv::SetEmergency::Response> response)
{
    (void)request;

    if (!check_connection_state("right_arm/set_emergency_stop")) {
        response->success = false;
        return;
    }

    response->success = lbot_api.lbot_emergency_stop(lbot_handle, LBOT_RIGHT_ARM, request->emergency);

    if (response->success) {
        if (request->emergency) {
            RCLCPP_INFO(this->get_logger(), "Right arm emergency stop activated successfully");
        } else {
            RCLCPP_INFO(this->get_logger(), "Right arm emergency stop released successfully");
        }
    } else {
        if (request->emergency) {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to activate emergency stop for right arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(),
                        "Failed to release emergency stop for right arm: %s",
                        lbot_api.lbot_get_last_error(lbot_handle).c_str());
        }
    }
}

void RightArmServiceNode::right_hand_l6_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l6_joint")) {
        return;
    }
    std::vector<uint8_t> hand_joints(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_position(lbot_handle, LBOT_RIGHT_ARM, hand_joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand joint");
    }
}

void RightArmServiceNode::right_hand_l6_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l6_force")) {
        return;
    }
    std::vector<uint8_t> hand_force(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_effort(lbot_handle, LBOT_RIGHT_ARM, hand_force)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand force");
    }
}

void RightArmServiceNode::right_hand_l6_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l6_speed")) {
        return;
    }
    std::vector<uint8_t> hand_speed(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l6_set_velocity(lbot_handle, LBOT_RIGHT_ARM, hand_speed)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand speed");
    }
}

void RightArmServiceNode::right_hand_l10_set_joint_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l10_joint")) {
        return;
    }
    std::vector<uint8_t> hand_joints(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_position(lbot_handle, LBOT_RIGHT_ARM, hand_joints)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand joint");
    }
}

void RightArmServiceNode::right_hand_l10_set_force_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l10_force")) {
        return;
    }
    std::vector<uint8_t> hand_force(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_effort(lbot_handle, LBOT_RIGHT_ARM, hand_force)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand force");
    }
}

void RightArmServiceNode::right_hand_l10_set_speed_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    if (!msg || msg->data.empty()) return;
    if (!check_connection_state("right_hand/set_l10_speed")) {
        return;
    }
    std::vector<uint8_t> hand_speed(msg->data.begin(), msg->data.end());

    if (!lbot_api.lbot_l10_set_velocity(lbot_handle, LBOT_RIGHT_ARM, hand_speed)) {
        RCLCPP_ERROR(this->get_logger(), "Failed to set right hand speed");
    }
}

}  // namespace lbot_driver

/****************************** 主函数 ******************************/

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    
    RCLCPP_INFO(rclcpp::get_logger("lbot_driver"), "Starting LBot Driver with separate service nodes...");
    
    // 创建三个节点
    auto main_node = std::make_shared<lbot_driver::LBot>("lbot_main_node");                       // 主节点：连接管理+状态发布+关节跟随
    auto left_arm_node = std::make_shared<lbot_driver::LeftArmServiceNode>("lbot_left_arm_node");    // 左臂服务节点
    auto right_arm_node = std::make_shared<lbot_driver::RightArmServiceNode>("lbot_right_arm_node");  // 右臂服务节点
    
    rclcpp::on_shutdown([&]() {
        RCLCPP_INFO(rclcpp::get_logger("lbot_driver"), "Shutdown signal received, stopping...");
        if (main_node) {
            main_node->shutting_down_ = true;
            main_node->disconnect_robot();
        }
        RCLCPP_INFO(rclcpp::get_logger("lbot_driver"), "Cleanup completed");
    });
    
    // 创建三个单线程执行器
    rclcpp::executors::SingleThreadedExecutor main_executor;        // 主节点执行器
    rclcpp::executors::SingleThreadedExecutor left_arm_executor;    // 左臂服务执行器
    rclcpp::executors::SingleThreadedExecutor right_arm_executor;   // 右臂服务执行器
    
    // 将节点添加到对应执行器
    main_executor.add_node(main_node);
    left_arm_executor.add_node(left_arm_node);
    right_arm_executor.add_node(right_arm_node);
    
    // 创建三个线程分别运行执行器
    std::thread main_thread([&main_executor]() {
        try {
            RCLCPP_INFO(rclcpp::get_logger("main_executor"), "Main node executor spinning...");
            main_executor.spin();
        } catch (const std::exception& e) {
            RCLCPP_ERROR(rclcpp::get_logger("main_executor"), "Exception in main executor: %s", e.what());
        }
    });
    
    std::thread left_arm_thread([&left_arm_executor]() {
        try {
            RCLCPP_INFO(rclcpp::get_logger("left_arm_executor"), "Left arm service executor spinning...");
            left_arm_executor.spin();
        } catch (const std::exception& e) {
            RCLCPP_ERROR(rclcpp::get_logger("left_arm_executor"), "Exception in left arm executor: %s", e.what());
        }
    });
    
    std::thread right_arm_thread([&right_arm_executor]() {
        try {
            RCLCPP_INFO(rclcpp::get_logger("right_arm_executor"), "Right arm service executor spinning...");
            right_arm_executor.spin();
        } catch (const std::exception& e) {
            RCLCPP_ERROR(rclcpp::get_logger("right_arm_executor"), "Exception in right arm executor: %s", e.what());
        }
    });
    
    RCLCPP_INFO(rclcpp::get_logger("lbot_driver"), "All nodes ready, spinning...");
    
    // 等待所有线程结束
    main_thread.join();
    left_arm_thread.join();
    right_arm_thread.join();
    
    RCLCPP_INFO(rclcpp::get_logger("lbot_driver"), "LBot Driver stopped");
    return 0;
}
