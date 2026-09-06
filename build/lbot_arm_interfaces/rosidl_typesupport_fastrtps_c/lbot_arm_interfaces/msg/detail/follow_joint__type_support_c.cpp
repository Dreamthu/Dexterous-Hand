// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/follow_joint__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "lbot_arm_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "lbot_arm_interfaces/msg/detail/follow_joint__struct.h"
#include "lbot_arm_interfaces/msg/detail/follow_joint__functions.h"
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

#include "rosidl_runtime_c/primitives_sequence.h"  // joints
#include "rosidl_runtime_c/primitives_sequence_functions.h"  // joints

// forward declare type support functions


using _FollowJoint__ros_msg_type = lbot_arm_interfaces__msg__FollowJoint;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_lbot_arm_interfaces__msg__FollowJoint(
  const lbot_arm_interfaces__msg__FollowJoint * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: joints
  {
    size_t size = ros_message->joints.size;
    auto array_ptr = ros_message->joints.data;
    cdr << static_cast<uint32_t>(size);
    cdr.serialize_array(array_ptr, size);
  }

  // Field name: follow
  {
    cdr << (ros_message->follow ? true : false);
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_deserialize_lbot_arm_interfaces__msg__FollowJoint(
  eprosima::fastcdr::Cdr & cdr,
  lbot_arm_interfaces__msg__FollowJoint * ros_message)
{
  // Field name: joints
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);

    // Check there are at least 'size' remaining bytes in the CDR stream before resizing
    auto old_state = cdr.get_state();
    bool correct_size = cdr.jump(size);
    cdr.set_state(old_state);
    if (!correct_size) {
      fprintf(stderr, "sequence size exceeds remaining buffer\n");
      return false;
    }

    if (ros_message->joints.data) {
      rosidl_runtime_c__float__Sequence__fini(&ros_message->joints);
    }
    if (!rosidl_runtime_c__float__Sequence__init(&ros_message->joints, size)) {
      fprintf(stderr, "failed to create array for field 'joints'");
      return false;
    }
    auto array_ptr = ros_message->joints.data;
    cdr.deserialize_array(array_ptr, size);
  }

  // Field name: follow
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message->follow = tmp ? true : false;
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_lbot_arm_interfaces__msg__FollowJoint(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _FollowJoint__ros_msg_type * ros_message = static_cast<const _FollowJoint__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: joints
  {
    size_t array_size = ros_message->joints.size;
    auto array_ptr = ros_message->joints.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: follow
  {
    size_t item_size = sizeof(ros_message->follow);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_lbot_arm_interfaces__msg__FollowJoint(
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

  // Field name: joints
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Field name: follow
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }


  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = lbot_arm_interfaces__msg__FollowJoint;
    is_plain =
      (
      offsetof(DataType, follow) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_key_lbot_arm_interfaces__msg__FollowJoint(
  const lbot_arm_interfaces__msg__FollowJoint * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: joints
  {
    size_t size = ros_message->joints.size;
    auto array_ptr = ros_message->joints.data;
    cdr << static_cast<uint32_t>(size);
    cdr.serialize_array(array_ptr, size);
  }

  // Field name: follow
  {
    cdr << (ros_message->follow ? true : false);
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_key_lbot_arm_interfaces__msg__FollowJoint(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _FollowJoint__ros_msg_type * ros_message = static_cast<const _FollowJoint__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: joints
  {
    size_t array_size = ros_message->joints.size;
    auto array_ptr = ros_message->joints.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: follow
  {
    size_t item_size = sizeof(ros_message->follow);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_key_lbot_arm_interfaces__msg__FollowJoint(
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
  // Field name: joints
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Field name: follow
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = lbot_arm_interfaces__msg__FollowJoint;
    is_plain =
      (
      offsetof(DataType, follow) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _FollowJoint__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const lbot_arm_interfaces__msg__FollowJoint * ros_message = static_cast<const lbot_arm_interfaces__msg__FollowJoint *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_lbot_arm_interfaces__msg__FollowJoint(ros_message, cdr);
}

static bool _FollowJoint__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  lbot_arm_interfaces__msg__FollowJoint * ros_message = static_cast<lbot_arm_interfaces__msg__FollowJoint *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_lbot_arm_interfaces__msg__FollowJoint(cdr, ros_message);
}

static uint32_t _FollowJoint__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_lbot_arm_interfaces__msg__FollowJoint(
      untyped_ros_message, 0));
}

static size_t _FollowJoint__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_lbot_arm_interfaces__msg__FollowJoint(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_FollowJoint = {
  "lbot_arm_interfaces::msg",
  "FollowJoint",
  _FollowJoint__cdr_serialize,
  _FollowJoint__cdr_deserialize,
  _FollowJoint__get_serialized_size,
  _FollowJoint__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _FollowJoint__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_FollowJoint,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_hash,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, msg, FollowJoint)() {
  return &_FollowJoint__type_support;
}

#if defined(__cplusplus)
}
#endif
