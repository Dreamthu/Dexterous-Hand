// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/msg/detail/follow_joint__functions.h"
#include "lbot_arm_interfaces/msg/detail/follow_joint__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace lbot_arm_interfaces
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void FollowJoint_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::msg::FollowJoint(_init);
}

void FollowJoint_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::msg::FollowJoint *>(message_memory);
  typed_message->~FollowJoint();
}

size_t size_function__FollowJoint__joints(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__FollowJoint__joints(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__FollowJoint__joints(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__FollowJoint__joints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__FollowJoint__joints(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__FollowJoint__joints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__FollowJoint__joints(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__FollowJoint__joints(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember FollowJoint_message_member_array[2] = {
  {
    "joints",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::FollowJoint, joints),  // bytes offset in struct
    nullptr,  // default value
    size_function__FollowJoint__joints,  // size() function pointer
    get_const_function__FollowJoint__joints,  // get_const(index) function pointer
    get_function__FollowJoint__joints,  // get(index) function pointer
    fetch_function__FollowJoint__joints,  // fetch(index, &value) function pointer
    assign_function__FollowJoint__joints,  // assign(index, value) function pointer
    resize_function__FollowJoint__joints  // resize(index) function pointer
  },
  {
    "follow",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::FollowJoint, follow),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers FollowJoint_message_members = {
  "lbot_arm_interfaces::msg",  // message namespace
  "FollowJoint",  // message name
  2,  // number of fields
  sizeof(lbot_arm_interfaces::msg::FollowJoint),
  false,  // has_any_key_member_
  FollowJoint_message_member_array,  // message members
  FollowJoint_init_function,  // function to initialize message memory (memory has to be allocated)
  FollowJoint_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t FollowJoint_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &FollowJoint_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_hash,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::msg::FollowJoint>()
{
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::FollowJoint_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, msg, FollowJoint)() {
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::FollowJoint_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
