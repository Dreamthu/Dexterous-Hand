// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/lbot_pose.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'euler'
// Member 'position'
#include "geometry_msgs/msg/detail/vector3__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__msg__LbotPose __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__msg__LbotPose __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct LbotPose_
{
  using Type = LbotPose_<ContainerAllocator>;

  explicit LbotPose_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : euler(_init),
    position(_init)
  {
    (void)_init;
  }

  explicit LbotPose_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : euler(_alloc, _init),
    position(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _euler_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _euler_type euler;
  using _position_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _position_type position;

  // setters for named parameter idiom
  Type & set__euler(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->euler = _arg;
    return *this;
  }
  Type & set__position(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->position = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__msg__LbotPose
    std::shared_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__msg__LbotPose
    std::shared_ptr<lbot_arm_interfaces::msg::LbotPose_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const LbotPose_ & other) const
  {
    if (this->euler != other.euler) {
      return false;
    }
    if (this->position != other.position) {
      return false;
    }
    return true;
  }
  bool operator!=(const LbotPose_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct LbotPose_

// alias to use template instance with default allocator
using LbotPose =
  lbot_arm_interfaces::msg::LbotPose_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_HPP_
