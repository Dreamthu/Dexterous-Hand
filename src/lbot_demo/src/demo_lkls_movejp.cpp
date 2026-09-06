#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "lbot_arm_interfaces/srv/move_jp.hpp"

using MoveJP = lbot_arm_interfaces::srv::MoveJP;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_lkls_movejp_node");

    auto left_client  = node->create_client<MoveJP>("left_arm/move_pose");
    auto right_client = node->create_client<MoveJP>("right_arm/move_pose");

    // ================
    // 等待两个服务上线
    // ================
    for (auto client : {left_client, right_client}) {
        while (!client->wait_for_service(std::chrono::seconds(1))) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for service.");
                return 0;
            }
            RCLCPP_INFO(node->get_logger(), "Service not available, waiting...");
        }
    }

    // =============================
    // 发送 MoveJP 的封装函数
    // =============================
    auto send_pose = [&](auto client,
                         float px, float py, float pz,
                         float rx, float ry, float rz,
                         bool block)
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
        req->block = block;

        auto future = client->async_send_request(req);

        auto ret = rclcpp::spin_until_future_complete(node, future);
        if (ret != rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_WARN(node->get_logger(), "MoveJP failed or timeout.");
        }

        // 稍微停顿确保机器人状态稳定
        rclcpp::sleep_for(std::chrono::milliseconds(200));
    };

    // =============================
    // 左臂 MoveJP
    // =============================
    send_pose(left_client,
              0.3f, 0.3f, -0.3f,
              0.0f, -1.57f, 0.0f, false);

    // =============================
    // 右臂 MoveJP
    // =============================
    send_pose(right_client,
              0.3f, -0.3f, -0.3f,
              0.0f, -1.57f, 0.0f, true);

    rclcpp::shutdown();
    return 0;
}
