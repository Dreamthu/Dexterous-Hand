// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/GetCurrentFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/get_current_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{


}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetCurrentFrame_Request>()
{
  return ::lbot_arm_interfaces::srv::GetCurrentFrame_Request(rosidl_runtime_cpp::MessageInitialization::ZERO);
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_GetCurrentFrame_Response_success
{
public:
  explicit Init_GetCurrentFrame_Response_success(::lbot_arm_interfaces::srv::GetCurrentFrame_Response & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Response success(::lbot_arm_interfaces::srv::GetCurrentFrame_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Response msg_;
};

class Init_GetCurrentFrame_Response_frame
{
public:
  explicit Init_GetCurrentFrame_Response_frame(::lbot_arm_interfaces::srv::GetCurrentFrame_Response & msg)
  : msg_(msg)
  {}
  Init_GetCurrentFrame_Response_success frame(::lbot_arm_interfaces::srv::GetCurrentFrame_Response::_frame_type arg)
  {
    msg_.frame = std::move(arg);
    return Init_GetCurrentFrame_Response_success(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Response msg_;
};

class Init_GetCurrentFrame_Response_name
{
public:
  Init_GetCurrentFrame_Response_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetCurrentFrame_Response_frame name(::lbot_arm_interfaces::srv::GetCurrentFrame_Response::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_GetCurrentFrame_Response_frame(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetCurrentFrame_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_GetCurrentFrame_Response_name();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_GetCurrentFrame_Event_response
{
public:
  explicit Init_GetCurrentFrame_Event_response(::lbot_arm_interfaces::srv::GetCurrentFrame_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Event response(::lbot_arm_interfaces::srv::GetCurrentFrame_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Event msg_;
};

class Init_GetCurrentFrame_Event_request
{
public:
  explicit Init_GetCurrentFrame_Event_request(::lbot_arm_interfaces::srv::GetCurrentFrame_Event & msg)
  : msg_(msg)
  {}
  Init_GetCurrentFrame_Event_response request(::lbot_arm_interfaces::srv::GetCurrentFrame_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_GetCurrentFrame_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Event msg_;
};

class Init_GetCurrentFrame_Event_info
{
public:
  Init_GetCurrentFrame_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetCurrentFrame_Event_request info(::lbot_arm_interfaces::srv::GetCurrentFrame_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_GetCurrentFrame_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::GetCurrentFrame_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::GetCurrentFrame_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_GetCurrentFrame_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__BUILDER_HPP_
