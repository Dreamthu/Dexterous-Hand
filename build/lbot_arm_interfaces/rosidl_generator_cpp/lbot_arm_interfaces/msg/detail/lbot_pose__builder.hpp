// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/lbot_pose.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/msg/detail/lbot_pose__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace msg
{

namespace builder
{

class Init_LbotPose_position
{
public:
  explicit Init_LbotPose_position(::lbot_arm_interfaces::msg::LbotPose & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::msg::LbotPose position(::lbot_arm_interfaces::msg::LbotPose::_position_type arg)
  {
    msg_.position = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::LbotPose msg_;
};

class Init_LbotPose_euler
{
public:
  Init_LbotPose_euler()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_LbotPose_position euler(::lbot_arm_interfaces::msg::LbotPose::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return Init_LbotPose_position(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::LbotPose msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::msg::LbotPose>()
{
  return lbot_arm_interfaces::msg::builder::Init_LbotPose_euler();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__BUILDER_HPP_
