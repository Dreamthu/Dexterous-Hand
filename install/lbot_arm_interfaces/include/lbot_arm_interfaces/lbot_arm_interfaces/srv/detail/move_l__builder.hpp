// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/MoveL.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/move_l.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_L__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_L__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/move_l__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveL_Request_block
{
public:
  explicit Init_MoveL_Request_block(::lbot_arm_interfaces::srv::MoveL_Request & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::MoveL_Request block(::lbot_arm_interfaces::srv::MoveL_Request::_block_type arg)
  {
    msg_.block = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Request msg_;
};

class Init_MoveL_Request_acce
{
public:
  explicit Init_MoveL_Request_acce(::lbot_arm_interfaces::srv::MoveL_Request & msg)
  : msg_(msg)
  {}
  Init_MoveL_Request_block acce(::lbot_arm_interfaces::srv::MoveL_Request::_acce_type arg)
  {
    msg_.acce = std::move(arg);
    return Init_MoveL_Request_block(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Request msg_;
};

class Init_MoveL_Request_speed
{
public:
  explicit Init_MoveL_Request_speed(::lbot_arm_interfaces::srv::MoveL_Request & msg)
  : msg_(msg)
  {}
  Init_MoveL_Request_acce speed(::lbot_arm_interfaces::srv::MoveL_Request::_speed_type arg)
  {
    msg_.speed = std::move(arg);
    return Init_MoveL_Request_acce(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Request msg_;
};

class Init_MoveL_Request_euler
{
public:
  explicit Init_MoveL_Request_euler(::lbot_arm_interfaces::srv::MoveL_Request & msg)
  : msg_(msg)
  {}
  Init_MoveL_Request_speed euler(::lbot_arm_interfaces::srv::MoveL_Request::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return Init_MoveL_Request_speed(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Request msg_;
};

class Init_MoveL_Request_position
{
public:
  Init_MoveL_Request_position()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveL_Request_euler position(::lbot_arm_interfaces::srv::MoveL_Request::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_MoveL_Request_euler(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveL_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveL_Request_position();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveL_Response_success
{
public:
  Init_MoveL_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::MoveL_Response success(::lbot_arm_interfaces::srv::MoveL_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveL_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveL_Response_success();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveL_Event_response
{
public:
  explicit Init_MoveL_Event_response(::lbot_arm_interfaces::srv::MoveL_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::MoveL_Event response(::lbot_arm_interfaces::srv::MoveL_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Event msg_;
};

class Init_MoveL_Event_request
{
public:
  explicit Init_MoveL_Event_request(::lbot_arm_interfaces::srv::MoveL_Event & msg)
  : msg_(msg)
  {}
  Init_MoveL_Event_response request(::lbot_arm_interfaces::srv::MoveL_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_MoveL_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Event msg_;
};

class Init_MoveL_Event_info
{
public:
  Init_MoveL_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveL_Event_request info(::lbot_arm_interfaces::srv::MoveL_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_MoveL_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveL_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveL_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveL_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_L__BUILDER_HPP_
