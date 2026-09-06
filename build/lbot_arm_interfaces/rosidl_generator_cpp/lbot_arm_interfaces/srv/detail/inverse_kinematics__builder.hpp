// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:srv/InverseKinematics.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/inverse_kinematics.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__INVERSE_KINEMATICS__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__INVERSE_KINEMATICS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/srv/detail/inverse_kinematics__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_InverseKinematics_Request_euler
{
public:
  explicit Init_InverseKinematics_Request_euler(::lbot_arm_interfaces::srv::InverseKinematics_Request & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::InverseKinematics_Request euler(::lbot_arm_interfaces::srv::InverseKinematics_Request::_euler_type arg)
  {
    msg_.euler = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Request msg_;
};

class Init_InverseKinematics_Request_position
{
public:
  explicit Init_InverseKinematics_Request_position(::lbot_arm_interfaces::srv::InverseKinematics_Request & msg)
  : msg_(msg)
  {}
  Init_InverseKinematics_Request_euler position(::lbot_arm_interfaces::srv::InverseKinematics_Request::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_InverseKinematics_Request_euler(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Request msg_;
};

class Init_InverseKinematics_Request_joints
{
public:
  Init_InverseKinematics_Request_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_InverseKinematics_Request_position joints(::lbot_arm_interfaces::srv::InverseKinematics_Request::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return Init_InverseKinematics_Request_position(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::InverseKinematics_Request>()
{
  return lbot_arm_interfaces::srv::builder::Init_InverseKinematics_Request_joints();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_InverseKinematics_Response_success
{
public:
  explicit Init_InverseKinematics_Response_success(::lbot_arm_interfaces::srv::InverseKinematics_Response & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::InverseKinematics_Response success(::lbot_arm_interfaces::srv::InverseKinematics_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Response msg_;
};

class Init_InverseKinematics_Response_joints
{
public:
  Init_InverseKinematics_Response_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_InverseKinematics_Response_success joints(::lbot_arm_interfaces::srv::InverseKinematics_Response::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return Init_InverseKinematics_Response_success(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::InverseKinematics_Response>()
{
  return lbot_arm_interfaces::srv::builder::Init_InverseKinematics_Response_joints();
}

}  // namespace lbot_arm_interfaces


namespace lbot_arm_interfaces
{

namespace srv
{

namespace builder
{

class Init_InverseKinematics_Event_response
{
public:
  explicit Init_InverseKinematics_Event_response(::lbot_arm_interfaces::srv::InverseKinematics_Event & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::srv::InverseKinematics_Event response(::lbot_arm_interfaces::srv::InverseKinematics_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Event msg_;
};

class Init_InverseKinematics_Event_request
{
public:
  explicit Init_InverseKinematics_Event_request(::lbot_arm_interfaces::srv::InverseKinematics_Event & msg)
  : msg_(msg)
  {}
  Init_InverseKinematics_Event_response request(::lbot_arm_interfaces::srv::InverseKinematics_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_InverseKinematics_Event_response(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Event msg_;
};

class Init_InverseKinematics_Event_info
{
public:
  Init_InverseKinematics_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_InverseKinematics_Event_request info(::lbot_arm_interfaces::srv::InverseKinematics_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_InverseKinematics_Event_request(msg_);
  }

private:
  ::lbot_arm_interfaces::srv::InverseKinematics_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::srv::InverseKinematics_Event>()
{
  return lbot_arm_interfaces::srv::builder::Init_InverseKinematics_Event_info();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__INVERSE_KINEMATICS__BUILDER_HPP_
