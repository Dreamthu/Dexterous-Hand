#include <rclcpp/rclcpp.hpp>
#include <lbot_arm_interfaces/srv/forward_kinematics.hpp>
#include <geometry_msgs/msg/vector3.hpp>

using namespace std::chrono_literals;

class FkClientNode : public rclcpp::Node
{
public:
  FkClientNode() : Node("fk_client_node")
  {
    client_ = this->create_client<lbot_arm_interfaces::srv::ForwardKinematics>("left_arm/forward_kinematics");
  }

  void call_service()
  {
    // 等待服务可用
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for service.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
    }

    // 创建请求
    auto request = std::make_shared<lbot_arm_interfaces::srv::ForwardKinematics::Request>();
    request->joints = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    // 发送异步请求
    client_->async_send_request(
      request,
      [this](rclcpp::Client<lbot_arm_interfaces::srv::ForwardKinematics>::SharedFuture future) {
        auto response = future.get();
        RCLCPP_INFO(this->get_logger(), "Response:");
        RCLCPP_INFO(this->get_logger(), "Position: x=%.5f, y=%.5f, z=%.5f",
                   response->position.x, response->position.y, response->position.z);
        RCLCPP_INFO(this->get_logger(), "Euler Angles: x=%.1f, y=%.1f, z=%.1f",
                   response->euler.x, response->euler.y, response->euler.z);
        RCLCPP_INFO(this->get_logger(), "Success: %s", response->success ? "true" : "false");

        rclcpp::shutdown();
      });

  }

private:
  rclcpp::Client<lbot_arm_interfaces::srv::ForwardKinematics>::SharedPtr client_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<FkClientNode>();
  node->call_service();
  rclcpp::spin(node);
  return 0;
}