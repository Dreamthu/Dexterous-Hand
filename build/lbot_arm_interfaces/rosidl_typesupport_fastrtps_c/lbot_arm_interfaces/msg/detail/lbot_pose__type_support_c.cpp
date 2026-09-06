// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/lbot_pose__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "lbot_arm_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "lbot_arm_interfaces/msg/detail/lbot_pose__struct.h"
#include "lbot_arm_interfaces/msg/detail/lbot_pose__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "geometry_msgs/msg/detail/vector3__functions.h"  // euler, position

// forward declare type support functions

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
bool cdr_serialize_geometry_msgs__msg__Vector3(
  const geometry_msgs__msg__Vector3 * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
bool cdr_deserialize_geometry_msgs__msg__Vector3(
  eprosima::fastcdr::Cdr & cdr,
  geometry_msgs__msg__Vector3 * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
size_t get_serialized_size_geometry_msgs__msg__Vector3(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
size_t max_serialized_size_geometry_msgs__msg__Vector3(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
bool cdr_serialize_key_geometry_msgs__msg__Vector3(
  const geometry_msgs__msg__Vector3 * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
size_t get_serialized_size_key_geometry_msgs__msg__Vector3(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
size_t max_serialized_size_key_geometry_msgs__msg__Vector3(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_lbot_arm_interfaces
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3)();


using _LbotPose__ros_msg_type = lbot_arm_interfaces__msg__LbotPose;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_lbot_arm_interfaces__msg__LbotPose(
  const lbot_arm_interfaces__msg__LbotPose * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: euler
  {
    cdr_serialize_geometry_msgs__msg__Vector3(
      &ros_message->euler, cdr);
  }

  // Field name: position
  {
    cdr_serialize_geometry_msgs__msg__Vector3(
      &ros_message->position, cdr);
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_deserialize_lbot_arm_interfaces__msg__LbotPose(
  eprosima::fastcdr::Cdr & cdr,
  lbot_arm_interfaces__msg__LbotPose * ros_message)
{
  // Field name: euler
  {
    cdr_deserialize_geometry_msgs__msg__Vector3(cdr, &ros_message->euler);
  }

  // Field name: position
  {
    cdr_deserialize_geometry_msgs__msg__Vector3(cdr, &ros_message->position);
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_lbot_arm_interfaces__msg__LbotPose(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _LbotPose__ros_msg_type * ros_message = static_cast<const _LbotPose__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: euler
  current_alignment += get_serialized_size_geometry_msgs__msg__Vector3(
    &(ros_message->euler), current_alignment);

  // Field name: position
  current_alignment += get_serialized_size_geometry_msgs__msg__Vector3(
    &(ros_message->position), current_alignment);

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_lbot_arm_interfaces__msg__LbotPose(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // Field name: euler
  {
    size_t array_size = 1;
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }

  // Field name: position
  {
    size_t array_size = 1;
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }


  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = lbot_arm_interfaces__msg__LbotPose;
    is_plain =
      (
      offsetof(DataType, position) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_key_lbot_arm_interfaces__msg__LbotPose(
  const lbot_arm_interfaces__msg__LbotPose * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: euler
  {
    cdr_serialize_key_geometry_msgs__msg__Vector3(
      &ros_message->euler, cdr);
  }

  // Field name: position
  {
    cdr_serialize_key_geometry_msgs__msg__Vector3(
      &ros_message->position, cdr);
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_key_lbot_arm_interfaces__msg__LbotPose(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _LbotPose__ros_msg_type * ros_message = static_cast<const _LbotPose__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: euler
  current_alignment += get_serialized_size_key_geometry_msgs__msg__Vector3(
    &(ros_message->euler), current_alignment);

  // Field name: position
  current_alignment += get_serialized_size_key_geometry_msgs__msg__Vector3(
    &(ros_message->position), current_alignment);

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_key_lbot_arm_interfaces__msg__LbotPose(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;
  // Field name: euler
  {
    size_t array_size = 1;
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_key_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }

  // Field name: position
  {
    size_t array_size = 1;
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_key_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = lbot_arm_interfaces__msg__LbotPose;
    is_plain =
      (
      offsetof(DataType, position) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _LbotPose__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const lbot_arm_interfaces__msg__LbotPose * ros_message = static_cast<const lbot_arm_interfaces__msg__LbotPose *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_lbot_arm_interfaces__msg__LbotPose(ros_message, cdr);
}

static bool _LbotPose__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  lbot_arm_interfaces__msg__LbotPose * ros_message = static_cast<lbot_arm_interfaces__msg__LbotPose *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_lbot_arm_interfaces__msg__LbotPose(cdr, ros_message);
}

static uint32_t _LbotPose__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_lbot_arm_interfaces__msg__LbotPose(
      untyped_ros_message, 0));
}

static size_t _LbotPose__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_lbot_arm_interfaces__msg__LbotPose(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_LbotPose = {
  "lbot_arm_interfaces::msg",
  "LbotPose",
  _LbotPose__cdr_serialize,
  _LbotPose__cdr_deserialize,
  _LbotPose__get_serialized_size,
  _LbotPose__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _LbotPose__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_LbotPose,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__LbotPose__get_type_hash,
  &lbot_arm_interfaces__msg__LbotPose__get_type_description,
  &lbot_arm_interfaces__msg__LbotPose__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, msg, LbotPose)() {
  return &_LbotPose__type_support;
}

#if defined(__cplusplus)
}
#endif
