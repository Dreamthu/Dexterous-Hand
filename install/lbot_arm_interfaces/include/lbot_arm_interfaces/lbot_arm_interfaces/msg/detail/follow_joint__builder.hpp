// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/follow_joint.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__BUILDER_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lbot_arm_interfaces/msg/detail/follow_joint__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lbot_arm_interfaces
{

namespace msg
{

namespace builder
{

class Init_FollowJoint_follow
{
public:
  explicit Init_FollowJoint_follow(::lbot_arm_interfaces::msg::FollowJoint & msg)
  : msg_(msg)
  {}
  ::lbot_arm_interfaces::msg::FollowJoint follow(::lbot_arm_interfaces::msg::FollowJoint::_follow_type arg)
  {
    msg_.follow = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::FollowJoint msg_;
};

class Init_FollowJoint_joints
{
public:
  Init_FollowJoint_joints()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_FollowJoint_follow joints(::lbot_arm_interfaces::msg::FollowJoint::_joints_type arg)
  {
    msg_.joints = std::move(arg);
    return Init_FollowJoint_follow(msg_);
  }

private:
  ::lbot_arm_interfaces::msg::FollowJoint msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::lbot_arm_interfaces::msg::FollowJoint>()
{
  return lbot_arm_interfaces::msg::builder::Init_FollowJoint_joints();
}

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__BUILDER_HPP_
