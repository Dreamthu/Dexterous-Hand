// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/arm_state.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/msg/detail/arm_state__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace msg
{

namespace builder
{

class Init_ArmState_pose
{
public:
  explicit Init_ArmState_pose(::lbot_arm_interfaces::msg::ArmState & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::msg::ArmState pose(::lbot_arm_interfaces::msg::ArmState::_pose_type arg)
  {
    msg_.pose = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::ArmState msg_;
};

class Init_ArmState_euler
{
public:
  explicit Init_ArmState_euler(::lbot_arm_interfaces::msg::ArmState & msg)
  : msg_(msg)
  {}
  Init_ArmState_pose euler(::lbot_arm_interfaces::msg::ArmState::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return Init_ArmState_pose(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::ArmState msg_;
};

class Init_ArmState_joints
{
public:
  Init_ArmState_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ArmState_euler joints(::lbot_arm_interfaces::msg::ArmState::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return Init_ArmState_euler(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::ArmState msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::msg::ArmState>()
{
  return lbot_arm_interfaces::msg::builder::Init_ArmState_joints();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__BUILDER_HPP_
