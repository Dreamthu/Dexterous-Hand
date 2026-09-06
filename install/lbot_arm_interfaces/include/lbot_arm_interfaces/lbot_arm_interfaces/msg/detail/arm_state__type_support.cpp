// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/msg/detail/arm_state__functions.h"
#include "lbot_arm_interfaces/msg/detail/arm_state__struct.hpp"
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

void ArmState_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::msg::ArmState(_init);
}

void ArmState_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::msg::ArmState *>(message_memory);
  typed_message->~ArmState();
}

size_t size_function__ArmState__joints(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__ArmState__joints(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__ArmState__joints(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__ArmState__joints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__ArmState__joints(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__ArmState__joints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__ArmState__joints(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__ArmState__joints(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember ArmState_message_member_array[3] = {
  {
    "joints",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::ArmState, joints),  // bytes offset in struct
    nullptr,  // default value
    size_function__ArmState__joints,  // size() function pointer
    get_const_function__ArmState__joints,  // get_const(index) function pointer
    get_function__ArmState__joints,  // get(index) function pointer
    fetch_function__ArmState__joints,  // fetch(index, &value) function pointer
    assign_function__ArmState__joints,  // assign(index, value) function pointer
    resize_function__ArmState__joints  // resize(index) function pointer
  },
  {
    "euler",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Vector3>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::ArmState, euler),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "pose",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Pose>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::ArmState, pose),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers ArmState_message_members = {
  "lbot_arm_interfaces::msg",  // message namespace
  "ArmState",  // message name
  3,  // number of fields
  sizeof(lbot_arm_interfaces::msg::ArmState),
  false,  // has_any_key_member_
  ArmState_message_member_array,  // message members
  ArmState_init_function,  // function to initialize message memory (memory has to be allocated)
  ArmState_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t ArmState_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ArmState_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__ArmState__get_type_hash,
  &lbot_arm_interfaces__msg__ArmState__get_type_description,
  &lbot_arm_interfaces__msg__ArmState__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::msg::ArmState>()
{
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::ArmState_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, msg, ArmState)() {
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::ArmState_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
