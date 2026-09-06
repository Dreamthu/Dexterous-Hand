// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/follow_joint.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__msg__FollowJoint __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__msg__FollowJoint __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct FollowJoint_
{
  using Type = FollowJoint_<ContainerAllocator>;

  explicit FollowJoint_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->follow = false;
    }
  }

  explicit FollowJoint_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->follow = false;
    }
  }

  // field types and members
  using _joints_type =
    std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>>;
  _joints_type joints;
  using _follow_type =
    bool;
  _follow_type follow;

  // setters for named parameter idiom
  Type & set__joints(
    const std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>> & _arg)
  {
    this->joints = _arg;
    return *this;
  }
  Type & set__follow(
    const bool & _arg)
  {
    this->follow = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__msg__FollowJoint
    std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__msg__FollowJoint
    std::shared_ptr<lbot_arm_interfaces::msg::FollowJoint_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const FollowJoint_ & other) const
  {
    if (this->joints != other.joints) {
      return false;
    }
    if (this->follow != other.follow) {
      return false;
    }
    return true;
  }
  bool operator!=(const FollowJoint_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct FollowJoint_

// alias to use template instance with default allocator
using FollowJoint =
  lbot_arm_interfaces::msg::FollowJoint_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_HPP_
