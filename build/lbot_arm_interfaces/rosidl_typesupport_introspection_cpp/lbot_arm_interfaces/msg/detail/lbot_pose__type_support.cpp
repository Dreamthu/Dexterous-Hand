// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/msg/detail/lbot_pose__functions.h"
#include "lbot_arm_interfaces/msg/detail/lbot_pose__struct.hpp"
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

void LbotPose_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::msg::LbotPose(_init);
}

void LbotPose_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::msg::LbotPose *>(message_memory);
  typed_message->~LbotPose();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember LbotPose_message_member_array[2] = {
  {
    "euler",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Vector3>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::LbotPose, euler),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "position",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Vector3>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::msg::LbotPose, position),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers LbotPose_message_members = {
  "lbot_arm_interfaces::msg",  // message namespace
  "LbotPose",  // message name
  2,  // number of fields
  sizeof(lbot_arm_interfaces::msg::LbotPose),
  false,  // has_any_key_member_
  LbotPose_message_member_array,  // message members
  LbotPose_init_function,  // function to initialize message memory (memory has to be allocated)
  LbotPose_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t LbotPose_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &LbotPose_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__LbotPose__get_type_hash,
  &lbot_arm_interfaces__msg__LbotPose__get_type_description,
  &lbot_arm_interfaces__msg__LbotPose__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::msg::LbotPose>()
{
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::LbotPose_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, msg, LbotPose)() {
  return &::lbot_arm_interfaces::msg::rosidl_typesupport_introspection_cpp::LbotPose_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
