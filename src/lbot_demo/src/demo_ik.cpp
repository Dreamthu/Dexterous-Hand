#include <rclcpp/rclcpp.hpp>
#include <lbot_arm_interfaces/srv/inverse_kinematics.hpp>
#include <geometry_msgs/msg/vector3.hpp>

using namespace std::chrono_literals;

class IKServiceClient : public rclcpp::Node
{
public:
  IKServiceClient() : Node("ik_service_client")
  {
    // 创建服务客户端
    client_ = this->create_client<lbot_arm_interfaces::srv::InverseKinematics>("left_arm/inverse_kinematics");
    
    // 等待服务可用
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for service.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
    }
    
    // 发送请求
    auto request = std::make_shared<lbot_arm_interfaces::srv::InverseKinematics::Request>();
    
    // 设置请求参数
    request->joints = {0,0,0,0,0,0,0};  // 空关节数组
    request->position.x = 0.3;
    request->position.y = 0.3;
    request->position.z = -0.3;
    request->euler.x = 0.0;
    request->euler.y = -1.57;
    request->euler.z = 0.0;

    
    // 异步发送请求
    auto result_future = client_->async_send_request(request);
    
    // 等待响应
    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), result_future) ==
        rclcpp::FutureReturnCode::SUCCESS)
    {
      auto response = result_future.get();
      RCLCPP_INFO(this->get_logger(), "Service response received");
      RCLCPP_INFO(this->get_logger(), "Success: %s", response->success ? "true" : "false");
      
      if (response->success) {
        RCLCPP_INFO(this->get_logger(), "Joints:");
        for (size_t i = 0; i < response->joints.size(); ++i) {
          RCLCPP_INFO(this->get_logger(), "  Joint %zu: %f", i, response->joints[i]);
        }
      } else {
        RCLCPP_ERROR(this->get_logger(), "IK calculation failed");
      }
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to call service");
    }
  }

private:
  rclcpp::Client<lbot_arm_interfaces::srv::InverseKinematics>::SharedPtr client_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<IKServiceClient>();
  rclcpp::shutdown();
  return 0;
}