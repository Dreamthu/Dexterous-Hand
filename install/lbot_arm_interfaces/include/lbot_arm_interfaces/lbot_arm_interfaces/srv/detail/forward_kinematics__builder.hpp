// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/ForwardKinematics.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/forward_kinematics.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/forward_kinematics__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ForwardKinematics_Request_joints
{
public:
  Init_ForwardKinematics_Request_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lbot_arm_interfaces::srv::ForwardKinematics_Request joints(::lbot_arm_interfaces::srv::ForwardKinematics_Request::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ForwardKinematics_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_ForwardKinematics_Request_joints();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ForwardKinematics_Response_success
{
public:
  explicit Init_ForwardKinematics_Response_success(::lbot_arm_interfaces::srv::ForwardKinematics_Response & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::ForwardKinematics_Response success(::lbot_arm_interfaces::srv::ForwardKinematics_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Response msg_;
};

class Init_ForwardKinematics_Response_euler
{
public:
  explicit Init_ForwardKinematics_Response_euler(::lbot_arm_interfaces::srv::ForwardKinematics_Response & msg)
  : msg_(msg)
  {}
  Init_ForwardKinematics_Response_success euler(::lbot_arm_interfaces::srv::ForwardKinematics_Response::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return Init_ForwardKinematics_Response_success(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Response msg_;
};

class Init_ForwardKinematics_Response_position
{
public:
  Init_ForwardKinematics_Response_position()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ForwardKinematics_Response_euler position(::lbot_arm_interfaces::srv::ForwardKinematics_Response::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_ForwardKinematics_Response_euler(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ForwardKinematics_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_ForwardKinematics_Response_position();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_ForwardKinematics_Event_response
{
public:
  explicit Init_ForwardKinematics_Event_response(::lbot_arm_interfaces::srv::ForwardKinematics_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::ForwardKinematics_Event response(::lbot_arm_interfaces::srv::ForwardKinematics_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Event msg_;
};

class Init_ForwardKinematics_Event_request
{
public:
  explicit Init_ForwardKinematics_Event_request(::lbot_arm_interfaces::srv::ForwardKinematics_Event & msg)
  : msg_(msg)
  {}
  Init_ForwardKinematics_Event_response request(::lbot_arm_interfaces::srv::ForwardKinematics_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_ForwardKinematics_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Event msg_;
};

class Init_ForwardKinematics_Event_info
{
public:
  Init_ForwardKinematics_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ForwardKinematics_Event_request info(::lbot_arm_interfaces::srv::ForwardKinematics_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_ForwardKinematics_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::ForwardKinematics_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::ForwardKinematics_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_ForwardKinematics_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__BUILDER_HPP_
