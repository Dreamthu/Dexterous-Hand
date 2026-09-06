// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from lbot_arm_interfaces:srv/MoveJ.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/move_j.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__STRUCT_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Request __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Request __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct MoveJ_Request_
{
  using Type = MoveJ_Request_<ContainerAllocator>;

  explicit MoveJ_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->speed = 0.0f;
      this->acce = 0.0f;
      this->block = false;
    }
  }

  explicit MoveJ_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->speed = 0.0f;
      this->acce = 0.0f;
      this->block = false;
    }
  }

  // field types and members
  using _joints_type =
    std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>>;
  _joints_type joints;
  using _speed_type =
    float;
  _speed_type speed;
  using _acce_type =
    float;
  _acce_type acce;
  using _block_type =
    bool;
  _block_type block;

  // setters for named parameter idiom
  Type & set__joints(
    const std::vector<float, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<float>> & _arg)
  {
    this->joints = _arg;
    return *this;
  }
  Type & set__speed(
    const float & _arg)
  {
    this->speed = _arg;
    return *this;
  }
  Type & set__acce(
    const float & _arg)
  {
    this->acce = _arg;
    return *this;
  }
  Type & set__block(
    const bool & _arg)
  {
    this->block = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Request
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Request
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MoveJ_Request_ & other) const
  {
    if (this->joints != other.joints) {
      return false;
    }
    if (this->speed != other.speed) {
      return false;
    }
    if (this->acce != other.acce) {
      return false;
    }
    if (this->block != other.block) {
      return false;
    }
    return true;
  }
  bool operator!=(const MoveJ_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MoveJ_Request_

// alias to use template instance with default allocator
using MoveJ_Request =
  lbot_arm_interfaces::srv::MoveJ_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace lbot_arm_interfaces


#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Response __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Response __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct MoveJ_Response_
{
  using Type = MoveJ_Response_<ContainerAllocator>;

  explicit MoveJ_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  explicit MoveJ_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Response
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Response
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MoveJ_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    return true;
  }
  bool operator!=(const MoveJ_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MoveJ_Response_

// alias to use template instance with default allocator
using MoveJ_Response =
  lbot_arm_interfaces::srv::MoveJ_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace lbot_arm_interfaces


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Event __attribute__((deprecated))
#else
# define DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Event __declspec(deprecated)
#endif

namespace lbot_arm_interfaces
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct MoveJ_Event_
{
  using Type = MoveJ_Event_<ContainerAllocator>;

  explicit MoveJ_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit MoveJ_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<lbot_arm_interfaces::srv::MoveJ_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<lbot_arm_interfaces::srv::MoveJ_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Event
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lbot_arm_interfaces__srv__MoveJ_Event
    std::shared_ptr<lbot_arm_interfaces::srv::MoveJ_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MoveJ_Event_ & other) const
  {
    if (this->info != other.info) {
      return false;
    }
    if (this->request != other.request) {
      return false;
    }
    if (this->response != other.response) {
      return false;
    }
    return true;
  }
  bool operator!=(const MoveJ_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MoveJ_Event_

// alias to use template instance with default allocator
using MoveJ_Event =
  lbot_arm_interfaces::srv::MoveJ_Event_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace lbot_arm_interfaces
{

namespace srv
{

struct MoveJ
{
  using Request = lbot_arm_interfaces::srv::MoveJ_Request;
  using Response = lbot_arm_interfaces::srv::MoveJ_Response;
  using Event = lbot_arm_interfaces::srv::MoveJ_Event;
};

}  // namespace srv

}  // namespace lbot_arm_interfaces

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__MOVE_J__STRUCT_HPP_
