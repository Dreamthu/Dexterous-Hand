// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/arm_state.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_HPP_

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
#include "geometry_msgs/msg/detail/vector3__struct.hpp"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__msg__ArmState __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__msg__ArmState __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ArmState_
{
  using Type = ArmState_<ContainerAllocator>;

  explicit ArmState_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : euler(_init),
    pose(_init)
  {
    (void)_init;
  }

  explicit ArmState_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : euler(_alloc, _init),
    pose(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _joints_type =
    std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>>;
  _joints_type joints;
  using _euler_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _euler_type euler;
  using _pose_type =
    geometry_msgs::msg::Pose_<ContainerAllocator>;
  _pose_type pose;

  // setters for named parameter idiom
  Type & set__joints(
    const std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>> & _arg)
  {
    this->joints = _arg;
    return *this;
  }
  Type & set__euler(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->euler = _arg;
    return *this;
  }
  Type & set__pose(
    const geometry_msgs::msg::Pose_<ContainerAllocator> & _arg)
  {
    this->pose = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__msg__ArmState
    std::shared_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__msg__ArmState
    std::shared_ptr<lbot_arm_interfaces::msg::ArmState_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ArmState_ & other) const
  {
    if (this->joints != other.joints) {
      return false;
    }
    if (this->euler != other.euler) {
      return false;
    }
    if (this->pose != other.pose) {
      return false;
    }
    return true;
  }
  bool operator!=(const ArmState_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ArmState_

// alias to use template instance with default allocator
using ArmState =
  lbot_arm_interfaces::msg::ArmState_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_HPP_
