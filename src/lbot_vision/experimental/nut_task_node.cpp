#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "cv_bridge/cv_bridge.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "lbot_arm_interfaces/srv/move_jp.hpp"
#include "lbot_arm_interfaces/srv/move_l.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/camera_info.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

using MoveJP = lbot_arm_interfaces::srv::MoveJP;
using MoveL = lbot_arm_interfaces::srv::MoveL;
using namespace std::chrono_literals;

struct Pose6 { double x{}, y{}, z{}, rx{}, ry{}, rz{}; };
struct Nut { geometry_msgs::msg::Point p; double size_m{}; std::string size_class; };

class NutTaskNode final : public rclcpp::Node {
public:
  NutTaskNode() : Node("nut_task_node") {
    color_topic_ = declare_parameter("color_topic", "/camera/color/image_raw");
    depth_topic_ = declare_parameter("depth_topic", "/camera/depth/image_rect_raw");
    camera_info_topic_ = declare_parameter("camera_info_topic", "/camera/color/camera_info");
    left_joint_topic_ = declare_parameter("left_joint_topic", "/robot1/left_arm/joint_states");
    right_joint_topic_ = declare_parameter("right_joint_topic", "/robot1/right_arm/joint_states");
    base_frame_ = declare_parameter("base_frame", "base_torso_root");
    camera_frame_ = declare_parameter("camera_frame", "camera_color_optical_frame");
    pregrasp_h_ = declare_parameter("pregrasp_height_m", 0.08);
    lift_h_ = declare_parameter("lift_height_m", 0.15);
    place_clearance_ = declare_parameter("place_clearance_m", 0.04);
    speed_ = declare_parameter("action_speed", 0.15);
    accel_ = declare_parameter("action_accel", 0.15);
    // Observation pose is deliberately a parameter: measure it after mounting the wrist camera.
    auto obs = declare_parameter<std::vector<double>>("camera_observe_xyzrpy",
                                                       {0.35, -0.35, 0.45, 0.0, -1.57, 0.0});
    if (obs.size() == 6) camera_pose_ = to_pose(obs);
    auto safe = declare_parameter<std::vector<double>>("left_safe_xyzrpy",
                                                        {0.25, 0.25, 0.20, 0.0, -1.57, 0.0});
    if (safe.size() == 6) left_safe_ = to_pose(safe);

    color_sub_ = create_subscription<sensor_msgs::msg::Image>(
      color_topic_, 10, [this](sensor_msgs::msg::Image::ConstSharedPtr msg) { last_color_ = msg; });
    depth_sub_ = create_subscription<sensor_msgs::msg::Image>(
      depth_topic_, 10, [this](sensor_msgs::msg::Image::ConstSharedPtr msg) { last_depth_ = msg; });
    info_sub_ = create_subscription<sensor_msgs::msg::CameraInfo>(
      camera_info_topic_, 10, [this](sensor_msgs::msg::CameraInfo::ConstSharedPtr msg) { camera_info_ = msg; });
    left_joint_sub_ = create_subscription<sensor_msgs::msg::JointState>(
      left_joint_topic_, 10, [this](sensor_msgs::msg::JointState::ConstSharedPtr msg) { left_joints_ = msg; });
    right_joint_sub_ = create_subscription<sensor_msgs::msg::JointState>(
      right_joint_topic_, 10, [this](sensor_msgs::msg::JointState::ConstSharedPtr msg) { right_joints_ = msg; });

    left_movejp_ = create_client<MoveJP>("/robot1/left_arm/move_pose");
    right_movejp_ = create_client<MoveJP>("/robot1/right_arm/move_pose");
    left_movel_ = create_client<MoveL>("/robot1/left_arm/move_linear");
    left_hand_ = create_publisher<std_msgs::msg::UInt8MultiArray>(
      "/robot1/left_hand/set_l6_joint", 10);
    timer_ = create_wall_timer(100ms, [this]() { tick(); });
  }

private:
  enum class State { INIT, CAMERA_POSE, DETECT, PICK_LARGE, PICK_MEDIUM, PICK_SMALL, DONE, FAULT };
  State state_{State::INIT};
  std::string color_topic_, depth_topic_, camera_info_topic_, left_joint_topic_, right_joint_topic_, base_frame_, camera_frame_;
  double pregrasp_h_{}, lift_h_{}, place_clearance_{}, speed_{}, accel_{};
  Pose6 camera_pose_{}, left_safe_{};
  std::vector<Nut> nuts_;
  sensor_msgs::msg::Image::ConstSharedPtr last_color_, last_depth_;
  sensor_msgs::msg::CameraInfo::ConstSharedPtr camera_info_;
  sensor_msgs::msg::JointState::ConstSharedPtr left_joints_, right_joints_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_sub_, depth_sub_;
  rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_joint_sub_, right_joint_sub_;
  rclcpp::Client<MoveJP>::SharedPtr left_movejp_, right_movejp_;
  rclcpp::Client<MoveL>::SharedPtr left_movel_;
  rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr left_hand_;
  rclcpp::TimerBase::SharedPtr timer_;

  static Pose6 to_pose(const std::vector<double>& v) { return {v[0],v[1],v[2],v[3],v[4],v[5]}; }

  void tick() {
    switch (state_) {
      case State::INIT:
        if (!left_joints_ || !right_joints_) return;
        // TODO: compare both arms with calibrated natural-down joint vectors.
        state_ = State::CAMERA_POSE; break;
      case State::CAMERA_POSE:
        if (!call_movejp(right_movejp_, camera_pose_)) return fault("right camera pose failed");
        state_ = State::DETECT; break;
      case State::DETECT:
        // TODO: run detector; it must return exactly 3 legal nuts in base_torso_root.
        nuts_ = detect_scene();
        if (nuts_.size() != 3) return fault("scene does not contain exactly 3 valid nuts");
        state_ = State::PICK_LARGE; break;
      case State::PICK_LARGE: if (pick_place("large", 0)) state_ = State::PICK_MEDIUM; break;
      case State::PICK_MEDIUM: if (pick_place("medium", 1)) state_ = State::PICK_SMALL; break;
      case State::PICK_SMALL: if (pick_place("small", 2)) state_ = State::DONE; break;
      case State::DONE: RCLCPP_INFO(get_logger(), "nut task complete"); timer_->cancel(); break;
      case State::FAULT: break;
    }
  }

  std::vector<Nut> detect_scene() {
    // TODO: cv::Mat conversion, black/blue contour geometry, depth-plane intersection,
    // contour clearance checks, and size sorting. Return points already transformed to base_frame_.
    return {};
  }

  bool pick_place(const std::string& size_class, int slot_index) {
    auto it = std::find_if(nuts_.begin(), nuts_.end(), [&](const Nut& n){ return n.size_class == size_class; });
    if (it == nuts_.end()) { fault("missing " + size_class + " nut"); return false; }
    Pose6 pre{it->p.x, it->p.y, it->p.z + pregrasp_h_, 0.0, -1.57, 0.0};
    Pose6 grasp{it->p.x, it->p.y, it->p.z + 0.01, 0.0, -1.57, 0.0};
    if (!call_movejp(left_movejp_, pre) || !call_movel(grasp)) return false;
    command_hand(/*closed=*/true);
    if (!call_movel(Pose6{it->p.x, it->p.y, it->p.z + lift_h_, 0.0, -1.57, 0.0})) return false;
    // TODO: replace with detected slot center from blue-basket geometry.
    Pose6 slot{0.0 + 0.08 * slot_index, -0.25, it->p.z + place_clearance_, 0.0, -1.57, 0.0};
    if (!call_movejp(left_movejp_, Pose6{slot.x, slot.y, slot.z + pregrasp_h_, slot.rx, slot.ry, slot.rz}) ||
        !call_movel(slot)) return false;
    command_hand(/*closed=*/false);
    if (!call_movel(Pose6{slot.x, slot.y, slot.z + pregrasp_h_, slot.rx, slot.ry, slot.rz})) return false;
    // TODO: verify source disappearance and slot occupancy before returning true.
    return true;
  }

  bool call_movejp(const rclcpp::Client<MoveJP>::SharedPtr& client, const Pose6& p) {
    if (!client->wait_for_service(500ms)) return false;
    auto req = std::make_shared<MoveJP::Request>(); fill(req->position, req->euler, p);
    req->speed = speed_; req->acce = accel_; req->block = true;
    auto future = client->async_send_request(req);
    return rclcpp::spin_until_future_complete(get_node_base_interface(), future) ==
           rclcpp::FutureReturnCode::SUCCESS && future.get()->success;
  }
  bool call_movel(const Pose6& p) {
    if (!left_movel_->wait_for_service(500ms)) return false;
    auto req = std::make_shared<MoveL::Request>(); fill(req->position, req->euler, p);
    req->speed = speed_; req->acce = accel_; req->block = true;
    auto future = left_movel_->async_send_request(req);
    return rclcpp::spin_until_future_complete(get_node_base_interface(), future) ==
           rclcpp::FutureReturnCode::SUCCESS && future.get()->success;
  }
  static void fill(geometry_msgs::msg::Vector3& pos, geometry_msgs::msg::Vector3& e, const Pose6& p) {
    pos.x=p.x; pos.y=p.y; pos.z=p.z; e.x=p.rx; e.y=p.ry; e.z=p.rz;
  }
  void command_hand(bool closed) {
    std_msgs::msg::UInt8MultiArray msg; msg.data = closed ?
      std::vector<uint8_t>{250,128,10,10,10,10} : std::vector<uint8_t>{128,128,128,128,128,128};
    left_hand_->publish(msg);
  }
  void fault(const std::string& why) { RCLCPP_ERROR(get_logger(), "%s", why.c_str()); state_ = State::FAULT; }
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NutTaskNode>());
  rclcpp::shutdown();
  return 0;
}
