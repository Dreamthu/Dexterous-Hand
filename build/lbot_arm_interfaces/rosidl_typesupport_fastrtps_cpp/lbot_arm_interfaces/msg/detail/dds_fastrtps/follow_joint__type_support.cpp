// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/follow_joint__rosidl_typesupport_fastrtps_cpp.hpp"
#include "lbot_arm_interfaces/msg/detail/follow_joint__functions.h"
#include "lbot_arm_interfaces/msg/detail/follow_joint__struct.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_fastrtps_cpp/identifier.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"
#include "fastcdr/Cdr.h"


// forward declaration of message dependencies and their conversion functions

namespace lbot_arm_interfaces
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{


bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
cdr_serialize(
  const lbot_arm_interfaces::msg::FollowJoint & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: joints
  {
    cdr << ros_message.joints;
  }

  // Member: follow
  cdr << (ros_message.follow ? true : false);

  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  lbot_arm_interfaces::msg::FollowJoint & ros_message)
{
  // Member: joints
  {
    cdr >> ros_message.joints;
  }

  // Member: follow
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message.follow = tmp ? true : false;
  }

  return true;
}  // NOLINT(readability/fn_size)


size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
get_serialized_size(
  const lbot_arm_interfaces::msg::FollowJoint & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: joints
  {
    size_t array_size = ros_message.joints.size();
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    size_t item_size = sizeof(ros_message.joints[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Member: follow
  {
    size_t item_size = sizeof(ros_message.follow);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}


size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
max_serialized_size_FollowJoint(
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

  // Member: joints
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
  // Member: follow
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
    using DataType = lbot_arm_interfaces::msg::FollowJoint;
    is_plain =
      (
      offsetof(DataType, follow) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
cdr_serialize_key(
  const lbot_arm_interfaces::msg::FollowJoint & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: joints
  {
    cdr << ros_message.joints;
  }

  // Member: follow
  cdr << (ros_message.follow ? true : false);

  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
get_serialized_size_key(
  const lbot_arm_interfaces::msg::FollowJoint & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: joints
  {
    size_t array_size = ros_message.joints.size();
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    size_t item_size = sizeof(ros_message.joints[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Member: follow
  {
    size_t item_size = sizeof(ros_message.follow);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lbot_arm_interfaces
max_serialized_size_key_FollowJoint(
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

  // Member: joints
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

  // Member: follow
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
    using DataType = lbot_arm_interfaces::msg::FollowJoint;
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
  auto typed_message =
    static_cast<const lbot_arm_interfaces::msg::FollowJoint *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _FollowJoint__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<lbot_arm_interfaces::msg::FollowJoint *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _FollowJoint__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const lbot_arm_interfaces::msg::FollowJoint *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _FollowJoint__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_FollowJoint(full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

static message_type_support_callbacks_t _FollowJoint__callbacks = {
  "lbot_arm_interfaces::msg",
  "FollowJoint",
  _FollowJoint__cdr_serialize,
  _FollowJoint__cdr_deserialize,
  _FollowJoint__get_serialized_size,
  _FollowJoint__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _FollowJoint__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_FollowJoint__callbacks,
  get_message_typesupport_handle_function,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_hash,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description,
  &lbot_arm_interfaces__msg__FollowJoint__get_type_description_sources,
};

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_lbot_arm_interfaces
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::msg::FollowJoint>()
{
  return &lbot_arm_interfaces::msg::typesupport_fastrtps_cpp::_FollowJoint__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, msg, FollowJoint)() {
  return &lbot_arm_interfaces::msg::typesupport_fastrtps_cpp::_FollowJoint__handle;
}

#ifdef __cplusplus
}
#endif
