// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/SetEnable.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/set_enable.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__SET_ENABLE__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__SET_ENABLE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/set_enable__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetEnable_Request_enable
{
public:
  Init_SetEnable_Request_enable()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::SetEnable_Request enable(::lbot_arm_interfaces::srv::SetEnable_Request::_enable_type arg)
  {
    msg_.enable = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::SetEnable_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::SetEnable_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_SetEnable_Request_enable();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetEnable_Response_success
{
public:
  Init_SetEnable_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::SetEnable_Response success(::lbot_arm_interfaces::srv::SetEnable_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::SetEnable_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::SetEnable_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_SetEnable_Response_success();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetEnable_Event_response
{
public:
  explicit Init_SetEnable_Event_response(::lbot_arm_interfaces::srv::SetEnable_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::SetEnable_Event response(::lbot_arm_interfaces::srv::SetEnable_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::SetEnable_Event msg_;
};

class Init_SetEnable_Event_request
{
public:
  explicit Init_SetEnable_Event_request(::lbot_arm_interfaces::srv::SetEnable_Event & msg)
  : msg_(msg)
  {}
  Init_SetEnable_Event_response request(::lbot_arm_interfaces::srv::SetEnable_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SetEnable_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::SetEnable_Event msg_;
};

class Init_SetEnable_Event_info
{
public:
  Init_SetEnable_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetEnable_Event_request info(::lbot_arm_interfaces::srv::SetEnable_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SetEnable_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::SetEnable_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::SetEnable_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_SetEnable_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__SET_ENABLE__BUILDER_HPP_
