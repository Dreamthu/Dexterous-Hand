// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/GetFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/get_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__GET_FRAME__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__GET_FRAME__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/get_frame__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_GetFrame_Request_name
{
public:
  Init_GetFrame_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::GetFrame_Request name(::lbot_arm_interfaces::srv::GetFrame_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetFrame_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_GetFrame_Request_name();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_GetFrame_Response_success
{
public:
  explicit Init_GetFrame_Response_success(::lbot_arm_interfaces::srv::GetFrame_Response & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::GetFrame_Response success(::lbot_arm_interfaces::srv::GetFrame_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Response msg_;
};

class Init_GetFrame_Response_frame
{
public:
  Init_GetFrame_Response_frame()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetFrame_Response_success frame(::lbot_arm_interfaces::srv::GetFrame_Response::_frame_type arg)
  {
    msg_.frame = std::move(arg);
    return Init_GetFrame_Response_success(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetFrame_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_GetFrame_Response_frame();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_GetFrame_Event_response
{
public:
  explicit Init_GetFrame_Event_response(::lbot_arm_interfaces::srv::GetFrame_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::GetFrame_Event response(::lbot_arm_interfaces::srv::GetFrame_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Event msg_;
};

class Init_GetFrame_Event_request
{
public:
  explicit Init_GetFrame_Event_request(::lbot_arm_interfaces::srv::GetFrame_Event & msg)
  : msg_(msg)
  {}
  Init_GetFrame_Event_response request(::lbot_arm_interfaces::srv::GetFrame_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_GetFrame_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Event msg_;
};

class Init_GetFrame_Event_info
{
public:
  Init_GetFrame_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetFrame_Event_request info(::lbot_arm_interfaces::srv::GetFrame_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_GetFrame_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetFrame_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetFrame_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_GetFrame_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__GET_FRAME__BUILDER_HPP_
