// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/ChangeFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/change_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__CHANGE_FRAME__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__CHANGE_FRAME__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/change_frame__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ChangeFrame_Request_name
{
public:
  Init_ChangeFrame_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::ChangeFrame_Request name(::lbot_arm_interfaces::srv::ChangeFrame_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ChangeFrame_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ChangeFrame_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_ChangeFrame_Request_name();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ChangeFrame_Response_success
{
public:
  Init_ChangeFrame_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::ChangeFrame_Response success(::lbot_arm_interfaces::srv::ChangeFrame_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ChangeFrame_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ChangeFrame_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_ChangeFrame_Response_success();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ChangeFrame_Event_response
{
public:
  explicit Init_ChangeFrame_Event_response(::lbot_arm_interfaces::srv::ChangeFrame_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::ChangeFrame_Event response(::lbot_arm_interfaces::srv::ChangeFrame_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ChangeFrame_Event msg_;
};

class Init_ChangeFrame_Event_request
{
public:
  explicit Init_ChangeFrame_Event_request(::lbot_arm_interfaces::srv::ChangeFrame_Event & msg)
  : msg_(msg)
  {}
  Init_ChangeFrame_Event_response request(::lbot_arm_interfaces::srv::ChangeFrame_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_ChangeFrame_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ChangeFrame_Event msg_;
};

class Init_ChangeFrame_Event_info
{
public:
  Init_ChangeFrame_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ChangeFrame_Event_request info(::lbot_arm_interfaces::srv::ChangeFrame_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_ChangeFrame_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ChangeFrame_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ChangeFrame_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_ChangeFrame_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__CHANGE_FRAME__BUILDER_HPP_
