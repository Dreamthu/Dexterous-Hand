#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "lbot_arm_interfaces/srv/move_jp.hpp"
#include "lbot_arm_interfaces/srv/move_l.hpp"
#include <chrono>
#include <thread>

using MoveJP = lbot_arm_interfaces::srv::MoveJP;
using MoveL  = lbot_arm_interfaces::srv::MoveL;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_lkrs_movel_node");

    auto left_movejp_client  = node->create_client<MoveJP>("left_arm/move_pose");
    auto right_movejp_client = node->create_client<MoveJP>("right_arm/move_pose");

    auto left_movel_client  = node->create_client<MoveL>("left_arm/move_linear");
    auto right_movel_client = node->create_client<MoveL>("right_arm/move_linear");

    // =============================
    // 等待 moveJP 服务上线
    // =============================
    RCLCPP_INFO(node->get_logger(), "Waiting for MoveJP service...");
    while (!left_movejp_client->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_INFO(node->get_logger(), "Waiting for left MoveJP...");
    }
    while (!right_movejp_client->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_INFO(node->get_logger(), "Waiting for right MoveJP...");
    }

    // =============================
    // 等待 moveL 服务上线
    // =============================
    RCLCPP_INFO(node->get_logger(), "Waiting for MoveL service...");
    while (!left_movel_client->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_INFO(node->get_logger(), "Waiting for left MoveL...");
    }
    while (!right_movel_client->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_INFO(node->get_logger(), "Waiting for right MoveL...");
    }

    // ==========================================
    // 封装 MoveJP
    // ==========================================
    auto send_movejp = [&](auto client,
                           float px, float py, float pz,
                           float rx, float ry, float rz)
    {
        auto req = std::make_shared<MoveJP::Request>();
        req->position.x = px;
        req->position.y = py;
        req->position.z = pz;
        req->euler.x = rx;
        req->euler.y = ry;
        req->euler.z = rz;
        req->speed = 1;
        req->acce  = 1;
        req->block = true;

        auto future = client->async_send_request(req);
        rclcpp::spin_until_future_complete(node, future);
        rclcpp::sleep_for(std::chrono::milliseconds(200));
    };

    // ==========================================
    // 封装 MoveL
    // ==========================================
    auto send_movel = [&](auto client,
                          float px, float py, float pz,
                          float rx, float ry, float rz)
    {
        auto req = std::make_shared<MoveL::Request>();
        req->position.x = px;
        req->position.y = py;
        req->position.z = pz;
        req->euler.x = rx;
        req->euler.y = ry;
        req->euler.z = rz;
        req->speed = 1;
        req->acce  = 1;
        req->block = true;

        auto future = client->async_send_request(req);
        rclcpp::spin_until_future_complete(node, future);
    };

    // 固定姿态
    float ex = 0.0f, ey = -1.57f, ez = 0.0f;

    // 左臂初始位置
    float lx = 0.3f,  ly = 0.3f,  lz = -0.3f;

    // 右臂初始位置
    float rx0 = 0.3f, ry0 = -0.2f, rz0 = -0.3f;

    // ==========================================
    // 1. MoveJP 先移动到初始位姿
    // ==========================================
    RCLCPP_INFO(node->get_logger(), "Running MoveJP...");

    send_movejp(left_movejp_client,  lx,  ly,  lz,  ex, ey, ez);
    send_movejp(right_movejp_client, rx0, ry0, rz0, ex, ey, ez);

    RCLCPP_INFO(node->get_logger(), "MoveJP done. Running MoveL...");

    // ==========================================
    // 2. MoveL 六个方向直线运动
    // ==========================================

    float delta = 0.03f;  // 移动 5 cm

    // 左臂 MoveL
    send_movel(left_movel_client, lx + delta, ly,        lz,        ex, ey, ez);
    send_movel(left_movel_client, lx - delta, ly,        lz,        ex, ey, ez); 
    send_movel(left_movel_client, lx,        ly + delta, lz,        ex, ey, ez); 
    send_movel(left_movel_client, lx,        ly - delta, lz,        ex, ey, ez); 
    send_movel(left_movel_client, lx,        ly,         lz + delta, ex, ey, ez); 
    send_movel(left_movel_client, lx,        ly,         lz - delta, ex, ey, ez); 

    // 右臂 MoveL
    send_movel(right_movel_client, rx0 + delta, ry0,        rz0,        ex, ey, ez);
    send_movel(right_movel_client, rx0 - delta, ry0,        rz0,        ex, ey, ez);
    send_movel(right_movel_client, rx0,        ry0 + delta, rz0,        ex, ey, ez);
    send_movel(right_movel_client, rx0,        ry0 - delta, rz0,        ex, ey, ez);
    send_movel(right_movel_client, rx0,        ry0,         rz0 + delta, ex, ey, ez);
    send_movel(right_movel_client, rx0,        ry0,         rz0 - delta, ex, ey, ez);

    RCLCPP_INFO(node->get_logger(), "MoveL test finished.");

    rclcpp::shutdown();
    return 0;
}
