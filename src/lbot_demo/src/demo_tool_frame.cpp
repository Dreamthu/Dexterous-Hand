#include <rclcpp/rclcpp.hpp>

#include "lbot_arm_interfaces/srv/set_frame.hpp"
#include "lbot_arm_interfaces/srv/get_frame.hpp"
#include "lbot_arm_interfaces/srv/get_current_frame.hpp"
#include "lbot_arm_interfaces/srv/get_all_frames.hpp"
#include "lbot_arm_interfaces/srv/change_frame.hpp"
#include "lbot_arm_interfaces/srv/delete_frame.hpp"

#include "lbot_arm_interfaces/msg/lbot_frame.hpp"

#include <chrono>
#include <memory>
#include <vector>

using namespace std::chrono_literals;

/* ================== helpers ================== */

lbot_arm_interfaces::msg::LbotFrame make_frame(
    const std::string &name,
    double x, double y, double z,
    double roll, double pitch, double yaw)
{
    lbot_arm_interfaces::msg::LbotFrame frame;
    frame.name = name;

    frame.position.x = x;
    frame.position.y = y;
    frame.position.z = z;

    frame.euler.x = roll;
    frame.euler.y = pitch;
    frame.euler.z = yaw;

    return frame;
}

void wait_for_services(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::SetFrame>::SharedPtr &set_frame_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetFrame>::SharedPtr &get_frame_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetCurrentFrame>::SharedPtr &get_current_frame_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetAllFrames>::SharedPtr &get_all_frames_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::ChangeFrame>::SharedPtr &change_frame_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::DeleteFrame>::SharedPtr &delete_frame_client)
{
    RCLCPP_INFO(node->get_logger(), "Waiting for tool frame services...");

    set_frame_client->wait_for_service();
    get_frame_client->wait_for_service();
    get_current_frame_client->wait_for_service();
    get_all_frames_client->wait_for_service();
    change_frame_client->wait_for_service();
    delete_frame_client->wait_for_service();

    RCLCPP_INFO(node->get_logger(), "All services are available.");
}

/* ================== service calls ================== */

void set_frame(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::SetFrame>::SharedPtr &client,
    const lbot_arm_interfaces::msg::LbotFrame &frame)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::SetFrame::Request>();
    req->frame = frame;

    auto future = client->async_send_request(req);
    auto ret = rclcpp::spin_until_future_complete(node, future);

    if (ret != rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(node->get_logger(), "Set frame [%s] timeout", frame.name.c_str());
        return;
    }

    auto resp = future.get();

    if (resp->success)
        RCLCPP_INFO(node->get_logger(), "Set frame [%s] success", frame.name.c_str());
    else
        RCLCPP_ERROR(node->get_logger(), "Set frame [%s] failed", frame.name.c_str());
}

void get_current_frame(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetCurrentFrame>::SharedPtr &client)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::GetCurrentFrame::Request>();
    auto future = client->async_send_request(req);

    auto ret = rclcpp::spin_until_future_complete(node, future);
    if (ret != rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(node->get_logger(), "Get current frame timeout");
        return;
    }

    auto resp = future.get(); 

    if (!resp->success)
    {
        RCLCPP_ERROR(node->get_logger(), "Get current frame failed");
        return;
    }

    const auto &f = resp->frame;
    RCLCPP_INFO(node->get_logger(),
                "Current frame: [%s] pos(%.3f %.3f %.3f) euler(%.3f %.3f %.3f)",
                resp->name.c_str(),
                f.position.x, f.position.y, f.position.z,
                f.euler.x, f.euler.y, f.euler.z);
}

void get_all_frames(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetAllFrames>::SharedPtr &get_all_client,
    const rclcpp::Client<lbot_arm_interfaces::srv::GetFrame>::SharedPtr &get_frame_client)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::GetAllFrames::Request>();
    auto future = get_all_client->async_send_request(req);

    auto ret = rclcpp::spin_until_future_complete(node, future);
    if (ret != rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(node->get_logger(), "Get all frames timeout");
        return;
    }

    auto resp = future.get();
    if (!resp->success)
    {
        RCLCPP_ERROR(node->get_logger(), "Get all frames failed");
        return;
    }

    RCLCPP_INFO(node->get_logger(), "All tool frames (%zu):", resp->names.size());

    for (const auto &name : resp->names)
    {
        auto get_req = std::make_shared<lbot_arm_interfaces::srv::GetFrame::Request>();
        get_req->name = name;

        auto f = get_frame_client->async_send_request(get_req);
        auto ret2 = rclcpp::spin_until_future_complete(node, f);

        if (ret2 != rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_WARN(node->get_logger(), "  get frame [%s] timeout", name.c_str());
            continue;
        }

        auto fr = f.get();
        if (!fr->success)
        {
            RCLCPP_WARN(node->get_logger(), "  get frame [%s] failed", name.c_str());
            continue;
        }

        const auto &frame = fr->frame;
        RCLCPP_INFO(node->get_logger(),
                    "  [%s] pos(%.3f %.3f %.3f) euler(%.3f %.3f %.3f)",
                    name.c_str(),
                    frame.position.x, frame.position.y, frame.position.z,
                    frame.euler.x, frame.euler.y, frame.euler.z);
    }
}

void change_frame(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::ChangeFrame>::SharedPtr &client,
    const std::string &name)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::ChangeFrame::Request>();
    req->name = name;

    auto future = client->async_send_request(req);
    auto ret = rclcpp::spin_until_future_complete(node, future);

    if (ret != rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(node->get_logger(), "Change frame [%s] timeout", name.c_str());
        return;
    }

    auto resp = future.get();  
    if (resp->success)
        RCLCPP_INFO(node->get_logger(), "Changed current frame to [%s]", name.c_str());
    else
        RCLCPP_ERROR(node->get_logger(), "Change frame [%s] failed", name.c_str());
}

void delete_frame(
    const rclcpp::Node::SharedPtr &node,
    const rclcpp::Client<lbot_arm_interfaces::srv::DeleteFrame>::SharedPtr &client,
    const std::string &name)
{
    auto req = std::make_shared<lbot_arm_interfaces::srv::DeleteFrame::Request>();
    req->name = name;

    auto future = client->async_send_request(req);
    auto ret = rclcpp::spin_until_future_complete(node, future);

    if (ret != rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(node->get_logger(), "Delete frame [%s] timeout", name.c_str());
        return;
    }

    auto resp = future.get();  
    if (resp->success)
        RCLCPP_INFO(node->get_logger(), "Deleted frame [%s]", name.c_str());
    else
        RCLCPP_ERROR(node->get_logger(), "Delete frame [%s] failed", name.c_str());
}

/* ================== main ================== */

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("demo_tool_frame");

    auto set_frame_client =
        node->create_client<lbot_arm_interfaces::srv::SetFrame>("left_arm/set_tool_frame");
    auto get_frame_client =
        node->create_client<lbot_arm_interfaces::srv::GetFrame>("left_arm/get_tool_frame");
    auto get_current_frame_client =
        node->create_client<lbot_arm_interfaces::srv::GetCurrentFrame>("left_arm/get_current_tool_frame");
    auto get_all_frames_client =
        node->create_client<lbot_arm_interfaces::srv::GetAllFrames>("left_arm/get_all_tool_frames");
    auto change_frame_client =
        node->create_client<lbot_arm_interfaces::srv::ChangeFrame>("left_arm/change_tool_frame");
    auto delete_frame_client =
        node->create_client<lbot_arm_interfaces::srv::DeleteFrame>("left_arm/delete_tool_frame");

    wait_for_services(
        node,
        set_frame_client,
        get_frame_client,
        get_current_frame_client,
        get_all_frames_client,
        change_frame_client,
        delete_frame_client);

    /* ========== demo sequence ========== */

    set_frame(node, set_frame_client, make_frame("tool1", 0.1, 0.0, 0.2, 0, 0, 0));
    set_frame(node, set_frame_client, make_frame("tool2", 0.2, 0.0, 0.25, 0, 0, 1.57));

    get_current_frame(node, get_current_frame_client);
    get_all_frames(node, get_all_frames_client, get_frame_client);

    change_frame(node, change_frame_client, "Arm_Tip");
    get_current_frame(node, get_current_frame_client);

    delete_frame(node, delete_frame_client, "tool1");
    delete_frame(node, delete_frame_client, "tool2");

    get_all_frames(node, get_all_frames_client, get_frame_client);

    RCLCPP_INFO(node->get_logger(), "Tool frame demo finished.");

    rclcpp::shutdown();
    return 0;
}
