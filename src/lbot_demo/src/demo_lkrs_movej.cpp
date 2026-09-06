#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "lbot_arm_interfaces/srv/move_j.hpp"

using MoveJ = lbot_arm_interfaces::srv::MoveJ;

int main(int argc, char** argv) 
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_lkrs_movej_node");

    auto left_movej_client  = node->create_client<MoveJ>("left_arm/move_joint");
    auto right_movej_client = node->create_client<MoveJ>("right_arm/move_joint");

    left_movej_client->wait_for_service();
    right_movej_client->wait_for_service();

    auto send = [&](auto client, std::vector<float> joints, bool block)
    {
        auto req = std::make_shared<MoveJ::Request>();
        req->speed = 1;
        req->acce  = 1;
        req->block = block;
        req->joints = joints;  

        auto future = client->async_send_request(req);

        auto status = rclcpp::spin_until_future_complete(node->get_node_base_interface(), future, std::chrono::seconds(5));
        if(status != rclcpp::FutureReturnCode::SUCCESS)
            RCLCPP_WARN(node->get_logger(), "MoveJ failed or timeout");

        rclcpp::sleep_for(std::chrono::milliseconds(200));
    };

    // ============ Your motions ============
    std::cout << "left start move 0" << std::endl;
    send(left_movej_client,  {0,0,0,0,0,0,0}, true);
    std::cout << "right start move 0" << std::endl;
    send(right_movej_client, {0,0,0,0,0,0,0}, true);
    
    std::cout << "left start move 1" << std::endl;
    send(left_movej_client, {1.2227f, -0.1693f, -0.511546f, 1.5f, 1.2f, -0.0105f, -0.042f}, true);
    std::cout << "left start move 2" << std::endl;
    send(left_movej_client, {1.2227f, -0.1693f,  0.01860f,  1.5f, 1.2f, -0.0105f, -0.042f}, false);

    std::cout << "right start move 1" << std::endl;
    send(right_movej_client, {-1.21f, 0.318f, -0.19f, -1.5f, -1.576f, 0.025f, -0.027f}, true);
    std::cout << "right start move 2" << std::endl;
    send(right_movej_client, {-1.21f, 0.319f,  0.407f, -1.5f, -1.576f, 0.025f, -0.027f}, true);
    

    std::cout << "left start move 3" << std::endl;
    send(left_movej_client,  {0,0,0,0,0,0,0}, false);
    std::cout << "right start move 3" << std::endl;
    send(right_movej_client, {0,0,0,0,0,0,0}, false);

    rclcpp::shutdown();
}
