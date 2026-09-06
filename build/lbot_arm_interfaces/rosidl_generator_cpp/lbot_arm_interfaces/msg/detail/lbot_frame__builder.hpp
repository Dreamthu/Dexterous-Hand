// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:msg/LbotFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/lbot_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/msg/detail/lbot_frame__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace msg
{

namespace builder
{

class Init_LbotFrame_position
{
public:
  explicit Init_LbotFrame_position(::lbot_arm_interfaces::msg::LbotFrame & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::msg::LbotFrame position(::lbot_arm_interfaces::msg::LbotFrame::_position_type arg)
  {
    msg_.position = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::LbotFrame msg_;
};

class Init_LbotFrame_euler
{
public:
  explicit Init_LbotFrame_euler(::lbot_arm_interfaces::msg::LbotFrame & msg)
  : msg_(msg)
  {}
  Init_LbotFrame_position euler(::lbot_arm_interfaces::msg::LbotFrame::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return Init_LbotFrame_position(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::LbotFrame msg_;
};

class Init_LbotFrame_name
{
public:
  Init_LbotFrame_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_LbotFrame_euler name(::lbot_arm_interfaces::msg::LbotFrame::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_LbotFrame_euler(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::LbotFrame msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::msg::LbotFrame>()
{
  return lbot_arm_interfaces::msg::builder::Init_LbotFrame_name();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__BUILDER_HPP_
