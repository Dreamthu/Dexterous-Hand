#include <chrono>
#include <iostream>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "lbot_arm_interfaces/srv/move_j.hpp"
#include "lbot_arm_interfaces/srv/set_enable.hpp"
#include "lbot_arm_interfaces/srv/set_emergency.hpp"

using namespace std::chrono_literals;

using MoveJ        = lbot_arm_interfaces::srv::MoveJ;
using SetEnable    = lbot_arm_interfaces::srv::SetEnable;
using SetEmergency = lbot_arm_interfaces::srv::SetEmergency;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_emergency_enable_node");

    /* ---------- clients ---------- */
    auto movej_client =
        node->create_client<MoveJ>("left_arm/move_joint");

    auto enable_client =
        node->create_client<SetEnable>("left_arm/set_enable");

    auto emergency_client =
        node->create_client<SetEmergency>("left_arm/set_emergency_stop");

    /* ---------- wait for services ---------- */
    RCLCPP_INFO(node->get_logger(), "Waiting for services...");
    movej_client->wait_for_service();
    enable_client->wait_for_service();
    emergency_client->wait_for_service();
    RCLCPP_INFO(node->get_logger(), "All services ready.");

    /* ---------- helper lambdas ---------- */

    auto send_movej = [&](const std::vector<float>& joints, bool block)
    {
        auto req = std::make_shared<MoveJ::Request>();
        req->speed  = 1.0;
        req->acce   = 1.0;
        req->block  = block;
        req->joints = joints;

        auto future = movej_client->async_send_request(req);
        auto ret = rclcpp::spin_until_future_complete(
            node->get_node_base_interface(), future, 10s);

        if (ret != rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_WARN(node->get_logger(), "MoveJ call failed or timeout");
        }
    };

    auto set_emergency = [&](bool emergency)
    {
        auto req = std::make_shared<SetEmergency::Request>();
        req->emergency = emergency;

        auto future = emergency_client->async_send_request(req);
        auto ret = rclcpp::spin_until_future_complete(
            node->get_node_base_interface(), future, 5s);

        if (ret == rclcpp::FutureReturnCode::SUCCESS &&
            future.get()->success)
        {
            RCLCPP_INFO(node->get_logger(),
                        "Emergency set to [%s]",
                        emergency ? "TRUE (STOP)" : "FALSE (RECOVER)");
        }
        else
        {
            RCLCPP_ERROR(node->get_logger(), "SetEmergency failed");
        }
    };

    auto set_enable = [&](bool enable)
    {
        auto req = std::make_shared<SetEnable::Request>();
        req->enable = enable;

        auto future = enable_client->async_send_request(req);
        auto ret = rclcpp::spin_until_future_complete(
            node->get_node_base_interface(), future, 5s);

        if (ret == rclcpp::FutureReturnCode::SUCCESS &&
            future.get()->success)
        {
            RCLCPP_INFO(node->get_logger(),
                        "SetEnable [%s]",
                        enable ? "ENABLE" : "DISABLE");
        }
        else
        {
            RCLCPP_ERROR(node->get_logger(), "SetEnable failed");
        }
    };

    /* ========== Demo sequence ========== */

    std::cout << "\n[1] Move to zero (blocking)\n";
    send_movej({0,0,0,0,0,0,0}, true);

    std::cout << "\n[2] Move to (3.14,0,0,0,0,0,0) (non-blocking)\n";
    send_movej({1.8f, 0, 0, 0, 0, 0, 0}, false);

    std::cout << "\n[3] Wait 2 seconds\n";
    rclcpp::sleep_for(1s);

    std::cout << "\n[4] Emergency STOP\n";
    set_emergency(true);

    std::cout << "\n[5] Wait 2 seconds\n";
    rclcpp::sleep_for(2s);

    std::cout << "\n[6] Emergency RECOVER\n";
    set_emergency(false);
    rclcpp::sleep_for(3s);

    std::cout << "\n[7] Move back to zero (blocking)\n";
    send_movej({0,0,0,0,0,0,0}, true);

    std::cout << "\n[8] Disable arm\n";
    set_enable(false);

    std::cout << "\n[9] Wait 2 seconds\n";
    rclcpp::sleep_for(2s);

    std::cout << "\n[10] Enable arm\n";
    set_enable(true);

    RCLCPP_INFO(node->get_logger(), "Demo finished.");

    rclcpp::shutdown();
    return 0;
}
