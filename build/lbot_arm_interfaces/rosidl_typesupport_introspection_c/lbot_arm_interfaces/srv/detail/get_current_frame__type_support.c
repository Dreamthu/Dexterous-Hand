// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from lbot_arm_interfaces:srv/GetCurrentFrame.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "lbot_arm_interfaces/srv/detail/get_current_frame__rosidl_typesupport_introspection_c.h"
#include "lbot_arm_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
#include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__init(message_memory);
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_fini_function(void * message_memory)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_member_array[1] = {
  {
    "structure_needs_at_least_one_member",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Request, structure_needs_at_least_one_member),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_members = {
  "lbot_arm_interfaces__srv",  // message namespace
  "GetCurrentFrame_Request",  // message name
  1,  // number of fields
  sizeof(lbot_arm_interfaces__srv__GetCurrentFrame_Request),
  false,  // has_any_key_member_
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_member_array,  // message members
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle = {
  0,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lbot_arm_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Request)() {
  if (!lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle.typesupport_identifier) {
    lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__rosidl_typesupport_introspection_c.h"
// already included above
// #include "lbot_arm_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `frame`
#include "lbot_arm_interfaces/msg/lbot_frame.h"
// Member `frame`
#include "lbot_arm_interfaces/msg/detail/lbot_frame__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__init(message_memory);
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_fini_function(void * message_memory)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_member_array[3] = {
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Response, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "frame",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Response, frame),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_members = {
  "lbot_arm_interfaces__srv",  // message namespace
  "GetCurrentFrame_Response",  // message name
  3,  // number of fields
  sizeof(lbot_arm_interfaces__srv__GetCurrentFrame_Response),
  false,  // has_any_key_member_
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_member_array,  // message members
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle = {
  0,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lbot_arm_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Response)() {
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, msg, LbotFrame)();
  if (!lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle.typesupport_identifier) {
    lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__rosidl_typesupport_introspection_c.h"
// already included above
// #include "lbot_arm_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "lbot_arm_interfaces/srv/get_current_frame.h"
// Member `request`
// Member `response`
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__init(message_memory);
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_fini_function(void * message_memory)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__fini(message_memory);
}

size_t lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__size_function__GetCurrentFrame_Event__request(
  const void * untyped_member)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence * member =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__request(
  const void * untyped_member, size_t index)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence * member =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__request(
  void * untyped_member, size_t index)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence * member =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__fetch_function__GetCurrentFrame_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Request * item =
    ((const lbot_arm_interfaces__srv__GetCurrentFrame_Request *)
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__request(untyped_member, index));
  lbot_arm_interfaces__srv__GetCurrentFrame_Request * value =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Request *)(untyped_value);
  *value = *item;
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__assign_function__GetCurrentFrame_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Request * item =
    ((lbot_arm_interfaces__srv__GetCurrentFrame_Request *)
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__request(untyped_member, index));
  const lbot_arm_interfaces__srv__GetCurrentFrame_Request * value =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Request *)(untyped_value);
  *item = *value;
}

bool lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__resize_function__GetCurrentFrame_Event__request(
  void * untyped_member, size_t size)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence * member =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence *)(untyped_member);
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__fini(member);
  return lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__init(member, size);
}

size_t lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__size_function__GetCurrentFrame_Event__response(
  const void * untyped_member)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence * member =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__response(
  const void * untyped_member, size_t index)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence * member =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__response(
  void * untyped_member, size_t index)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence * member =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__fetch_function__GetCurrentFrame_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const lbot_arm_interfaces__srv__GetCurrentFrame_Response * item =
    ((const lbot_arm_interfaces__srv__GetCurrentFrame_Response *)
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__response(untyped_member, index));
  lbot_arm_interfaces__srv__GetCurrentFrame_Response * value =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Response *)(untyped_value);
  *value = *item;
}

void lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__assign_function__GetCurrentFrame_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Response * item =
    ((lbot_arm_interfaces__srv__GetCurrentFrame_Response *)
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__response(untyped_member, index));
  const lbot_arm_interfaces__srv__GetCurrentFrame_Response * value =
    (const lbot_arm_interfaces__srv__GetCurrentFrame_Response *)(untyped_value);
  *item = *value;
}

bool lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__resize_function__GetCurrentFrame_Event__response(
  void * untyped_member, size_t size)
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence * member =
    (lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence *)(untyped_member);
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__fini(member);
  return lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Event, request),  // bytes offset in struct
    NULL,  // default value
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__size_function__GetCurrentFrame_Event__request,  // size() function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__request,  // get_const(index) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__request,  // get(index) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__fetch_function__GetCurrentFrame_Event__request,  // fetch(index, &value) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__assign_function__GetCurrentFrame_Event__request,  // assign(index, value) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__resize_function__GetCurrentFrame_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(lbot_arm_interfaces__srv__GetCurrentFrame_Event, response),  // bytes offset in struct
    NULL,  // default value
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__size_function__GetCurrentFrame_Event__response,  // size() function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_const_function__GetCurrentFrame_Event__response,  // get_const(index) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__get_function__GetCurrentFrame_Event__response,  // get(index) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__fetch_function__GetCurrentFrame_Event__response,  // fetch(index, &value) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__assign_function__GetCurrentFrame_Event__response,  // assign(index, value) function pointer
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__resize_function__GetCurrentFrame_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_members = {
  "lbot_arm_interfaces__srv",  // message namespace
  "GetCurrentFrame_Event",  // message name
  3,  // number of fields
  sizeof(lbot_arm_interfaces__srv__GetCurrentFrame_Event),
  false,  // has_any_key_member_
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_member_array,  // message members
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_type_support_handle = {
  0,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_members,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lbot_arm_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Event)() {
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Request)();
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Response)();
  if (!lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_type_support_handle.typesupport_identifier) {
    lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_members = {
  "lbot_arm_interfaces__srv",  // service namespace
  "GetCurrentFrame",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle,
  NULL,  // response message
  // lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle
  NULL  // event_message
  // lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle
};


static rosidl_service_type_support_t lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_type_support_handle = {
  0,
  &lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_members,
  get_service_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__rosidl_typesupport_introspection_c__GetCurrentFrame_Request_message_type_support_handle,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__rosidl_typesupport_introspection_c__GetCurrentFrame_Response_message_type_support_handle,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__rosidl_typesupport_introspection_c__GetCurrentFrame_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    lbot_arm_interfaces,
    srv,
    GetCurrentFrame
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    lbot_arm_interfaces,
    srv,
    GetCurrentFrame
  ),
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lbot_arm_interfaces
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame)(void) {
  if (!lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_type_support_handle.typesupport_identifier) {
    lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, GetCurrentFrame_Event)()->data;
  }

  return &lbot_arm_interfaces__srv__detail__get_current_frame__rosidl_typesupport_introspection_c__GetCurrentFrame_service_type_support_handle;
}
