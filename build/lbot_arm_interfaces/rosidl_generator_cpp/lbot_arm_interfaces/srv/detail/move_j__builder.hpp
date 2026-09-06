// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/MoveJ.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/move_j.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/move_j__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveJ_Request_block
{
public:
  explicit Init_MoveJ_Request_block(::lbot_arm_interfaces::srv::MoveJ_Request & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::MoveJ_Request block(::lbot_arm_interfaces::srv::MoveJ_Request::_block_type arg)
  {
    msg_.block = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Request msg_;
};

class Init_MoveJ_Request_acce
{
public:
  explicit Init_MoveJ_Request_acce(::lbot_arm_interfaces::srv::MoveJ_Request & msg)
  : msg_(msg)
  {}
  Init_MoveJ_Request_block acce(::lbot_arm_interfaces::srv::MoveJ_Request::_acce_type arg)
  {
    msg_.acce = std::move(arg);
    return Init_MoveJ_Request_block(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Request msg_;
};

class Init_MoveJ_Request_speed
{
public:
  explicit Init_MoveJ_Request_speed(::lbot_arm_interfaces::srv::MoveJ_Request & msg)
  : msg_(msg)
  {}
  Init_MoveJ_Request_acce speed(::lbot_arm_interfaces::srv::MoveJ_Request::_speed_type arg)
  {
    msg_.speed = std::move(arg);
    return Init_MoveJ_Request_acce(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Request msg_;
};

class Init_MoveJ_Request_joints
{
public:
  Init_MoveJ_Request_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveJ_Request_speed joints(::lbot_arm_interfaces::srv::MoveJ_Request::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return Init_MoveJ_Request_speed(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveJ_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveJ_Request_joints();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveJ_Response_success
{
public:
  Init_MoveJ_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::MoveJ_Response success(::lbot_arm_interfaces::srv::MoveJ_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveJ_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveJ_Response_success();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_MoveJ_Event_response
{
public:
  explicit Init_MoveJ_Event_response(::lbot_arm_interfaces::srv::MoveJ_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::MoveJ_Event response(::lbot_arm_interfaces::srv::MoveJ_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Event msg_;
};

class Init_MoveJ_Event_request
{
public:
  explicit Init_MoveJ_Event_request(::lbot_arm_interfaces::srv::MoveJ_Event & msg)
  : msg_(msg)
  {}
  Init_MoveJ_Event_response request(::lbot_arm_interfaces::srv::MoveJ_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_MoveJ_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Event msg_;
};

class Init_MoveJ_Event_info
{
public:
  Init_MoveJ_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveJ_Event_request info(::lbot_arm_interfaces::srv::MoveJ_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_MoveJ_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::MoveJ_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::MoveJ_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_MoveJ_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__BUILDER_HPP_
