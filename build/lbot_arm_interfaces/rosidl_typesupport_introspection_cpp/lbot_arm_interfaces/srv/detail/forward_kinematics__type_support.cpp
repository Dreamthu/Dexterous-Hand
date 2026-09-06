// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:srv/ForwardKinematics.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/srv/detail/forward_kinematics__functions.h"
#include "lbot_arm_interfaces/srv/detail/forward_kinematics__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void ForwardKinematics_Request_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::srv::ForwardKinematics_Request(_init);
}

void ForwardKinematics_Request_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::srv::ForwardKinematics_Request *>(message_memory);
  typed_message->~ForwardKinematics_Request();
}

size_t size_function__ForwardKinematics_Request__joints(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__ForwardKinematics_Request__joints(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__ForwardKinematics_Request__joints(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__ForwardKinematics_Request__joints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__ForwardKinematics_Request__joints(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__ForwardKinematics_Request__joints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__ForwardKinematics_Request__joints(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__ForwardKinematics_Request__joints(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember ForwardKinematics_Request_message_member_array[1] = {
  {
    "joints",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Request, joints),  // bytes offset in struct
    nullptr,  // default value
    size_function__ForwardKinematics_Request__joints,  // size() function pointer
    get_const_function__ForwardKinematics_Request__joints,  // get_const(index) function pointer
    get_function__ForwardKinematics_Request__joints,  // get(index) function pointer
    fetch_function__ForwardKinematics_Request__joints,  // fetch(index, &value) function pointer
    assign_function__ForwardKinematics_Request__joints,  // assign(index, value) function pointer
    resize_function__ForwardKinematics_Request__joints  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers ForwardKinematics_Request_message_members = {
  "lbot_arm_interfaces::srv",  // message namespace
  "ForwardKinematics_Request",  // message name
  1,  // number of fields
  sizeof(lbot_arm_interfaces::srv::ForwardKinematics_Request),
  false,  // has_any_key_member_
  ForwardKinematics_Request_message_member_array,  // message members
  ForwardKinematics_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  ForwardKinematics_Request_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t ForwardKinematics_Request_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ForwardKinematics_Request_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__ForwardKinematics_Request__get_type_hash,
  &lbot_arm_interfaces__srv__ForwardKinematics_Request__get_type_description,
  &lbot_arm_interfaces__srv__ForwardKinematics_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Request>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Request_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, ForwardKinematics_Request)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "array"
// already included above
// #include "cstddef"
// already included above
// #include "string"
// already included above
// #include "vector"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/field_types.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void ForwardKinematics_Response_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::srv::ForwardKinematics_Response(_init);
}

void ForwardKinematics_Response_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::srv::ForwardKinematics_Response *>(message_memory);
  typed_message->~ForwardKinematics_Response();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember ForwardKinematics_Response_message_member_array[3] = {
  {
    "position",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Vector3>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Response, position),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
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
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Response, euler),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "success",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Response, success),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers ForwardKinematics_Response_message_members = {
  "lbot_arm_interfaces::srv",  // message namespace
  "ForwardKinematics_Response",  // message name
  3,  // number of fields
  sizeof(lbot_arm_interfaces::srv::ForwardKinematics_Response),
  false,  // has_any_key_member_
  ForwardKinematics_Response_message_member_array,  // message members
  ForwardKinematics_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  ForwardKinematics_Response_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t ForwardKinematics_Response_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ForwardKinematics_Response_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__ForwardKinematics_Response__get_type_hash,
  &lbot_arm_interfaces__srv__ForwardKinematics_Response__get_type_description,
  &lbot_arm_interfaces__srv__ForwardKinematics_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Response>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Response_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, ForwardKinematics_Response)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "array"
// already included above
// #include "cstddef"
// already included above
// #include "string"
// already included above
// #include "vector"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/field_types.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void ForwardKinematics_Event_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lbot_arm_interfaces::srv::ForwardKinematics_Event(_init);
}

void ForwardKinematics_Event_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lbot_arm_interfaces::srv::ForwardKinematics_Event *>(message_memory);
  typed_message->~ForwardKinematics_Event();
}

size_t size_function__ForwardKinematics_Event__request(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Request> *>(untyped_member);
  return member->size();
}

const void * get_const_function__ForwardKinematics_Event__request(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Request> *>(untyped_member);
  return &member[index];
}

void * get_function__ForwardKinematics_Event__request(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Request> *>(untyped_member);
  return &member[index];
}

void fetch_function__ForwardKinematics_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const lbot_arm_interfaces::srv::ForwardKinematics_Request *>(
    get_const_function__ForwardKinematics_Event__request(untyped_member, index));
  auto & value = *reinterpret_cast<lbot_arm_interfaces::srv::ForwardKinematics_Request *>(untyped_value);
  value = item;
}

void assign_function__ForwardKinematics_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<lbot_arm_interfaces::srv::ForwardKinematics_Request *>(
    get_function__ForwardKinematics_Event__request(untyped_member, index));
  const auto & value = *reinterpret_cast<const lbot_arm_interfaces::srv::ForwardKinematics_Request *>(untyped_value);
  item = value;
}

void resize_function__ForwardKinematics_Event__request(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Request> *>(untyped_member);
  member->resize(size);
}

size_t size_function__ForwardKinematics_Event__response(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Response> *>(untyped_member);
  return member->size();
}

const void * get_const_function__ForwardKinematics_Event__response(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Response> *>(untyped_member);
  return &member[index];
}

void * get_function__ForwardKinematics_Event__response(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Response> *>(untyped_member);
  return &member[index];
}

void fetch_function__ForwardKinematics_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const lbot_arm_interfaces::srv::ForwardKinematics_Response *>(
    get_const_function__ForwardKinematics_Event__response(untyped_member, index));
  auto & value = *reinterpret_cast<lbot_arm_interfaces::srv::ForwardKinematics_Response *>(untyped_value);
  value = item;
}

void assign_function__ForwardKinematics_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<lbot_arm_interfaces::srv::ForwardKinematics_Response *>(
    get_function__ForwardKinematics_Event__response(untyped_member, index));
  const auto & value = *reinterpret_cast<const lbot_arm_interfaces::srv::ForwardKinematics_Response *>(untyped_value);
  item = value;
}

void resize_function__ForwardKinematics_Event__response(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<lbot_arm_interfaces::srv::ForwardKinematics_Response> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember ForwardKinematics_Event_message_member_array[3] = {
  {
    "info",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<service_msgs::msg::ServiceEventInfo>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Event, info),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "request",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Request>(),  // members of sub message
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Event, request),  // bytes offset in struct
    nullptr,  // default value
    size_function__ForwardKinematics_Event__request,  // size() function pointer
    get_const_function__ForwardKinematics_Event__request,  // get_const(index) function pointer
    get_function__ForwardKinematics_Event__request,  // get(index) function pointer
    fetch_function__ForwardKinematics_Event__request,  // fetch(index, &value) function pointer
    assign_function__ForwardKinematics_Event__request,  // assign(index, value) function pointer
    resize_function__ForwardKinematics_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Response>(),  // members of sub message
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(lbot_arm_interfaces::srv::ForwardKinematics_Event, response),  // bytes offset in struct
    nullptr,  // default value
    size_function__ForwardKinematics_Event__response,  // size() function pointer
    get_const_function__ForwardKinematics_Event__response,  // get_const(index) function pointer
    get_function__ForwardKinematics_Event__response,  // get(index) function pointer
    fetch_function__ForwardKinematics_Event__response,  // fetch(index, &value) function pointer
    assign_function__ForwardKinematics_Event__response,  // assign(index, value) function pointer
    resize_function__ForwardKinematics_Event__response  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers ForwardKinematics_Event_message_members = {
  "lbot_arm_interfaces::srv",  // message namespace
  "ForwardKinematics_Event",  // message name
  3,  // number of fields
  sizeof(lbot_arm_interfaces::srv::ForwardKinematics_Event),
  false,  // has_any_key_member_
  ForwardKinematics_Event_message_member_array,  // message members
  ForwardKinematics_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  ForwardKinematics_Event_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t ForwardKinematics_Event_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ForwardKinematics_Event_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__ForwardKinematics_Event__get_type_hash,
  &lbot_arm_interfaces__srv__ForwardKinematics_Event__get_type_description,
  &lbot_arm_interfaces__srv__ForwardKinematics_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Event>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Event_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, ForwardKinematics_Event)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/forward_kinematics__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/service_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/service_type_support_decl.hpp"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

// this is intentionally not const to allow initialization later to prevent an initialization race
static ::rosidl_typesupport_introspection_cpp::ServiceMembers ForwardKinematics_service_members = {
  "lbot_arm_interfaces::srv",  // service namespace
  "ForwardKinematics",  // service name
  // the following fields are initialized below on first access
  // see get_service_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics>()
  nullptr,  // request message
  nullptr,  // response message
  nullptr,  // event message
};

static const rosidl_service_type_support_t ForwardKinematics_service_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ForwardKinematics_service_members,
  get_service_typesupport_handle_function,
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Request>(),
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Response>(),
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<lbot_arm_interfaces::srv::ForwardKinematics>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<lbot_arm_interfaces::srv::ForwardKinematics>,
  &lbot_arm_interfaces__srv__ForwardKinematics__get_type_hash,
  &lbot_arm_interfaces__srv__ForwardKinematics__get_type_description,
  &lbot_arm_interfaces__srv__ForwardKinematics__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics>()
{
  // get a handle to the value to be returned
  auto service_type_support =
    &::lbot_arm_interfaces::srv::rosidl_typesupport_introspection_cpp::ForwardKinematics_service_type_support_handle;
  // get a non-const and properly typed version of the data void *
  auto service_members = const_cast<::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
    static_cast<const ::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
      service_type_support->data));
  // make sure all of the service_members are initialized
  // if they are not, initialize them
  if (
    service_members->request_members_ == nullptr ||
    service_members->response_members_ == nullptr ||
    service_members->event_members_ == nullptr)
  {
    // initialize the request_members_ with the static function from the external library
    service_members->request_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::lbot_arm_interfaces::srv::ForwardKinematics_Request
      >()->data
      );
    // initialize the response_members_ with the static function from the external library
    service_members->response_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::lbot_arm_interfaces::srv::ForwardKinematics_Response
      >()->data
      );
    // initialize the event_members_ with the static function from the external library
    service_members->event_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::lbot_arm_interfaces::srv::ForwardKinematics_Event
      >()->data
      );
  }
  // finally return the properly initialized service_type_support handle
  return service_type_support;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, ForwardKinematics)() {
  return ::rosidl_typesupport_introspection_cpp::get_service_type_support_handle<lbot_arm_interfaces::srv::ForwardKinematics>();
}

#ifdef __cplusplus
}
#endif
