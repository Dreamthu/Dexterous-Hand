// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from lbot_arm_interfaces:msg/LbotFrame.idl
// generated code does not contain a copyright notice
#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "lbot_arm_interfaces/msg/detail/lbot_frame__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_lbot_arm_interfaces__msg__LbotFrame(
  const lbot_arm_interfaces__msg__LbotFrame * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_deserialize_lbot_arm_interfaces__msg__LbotFrame(
  eprosima::fastcdr::Cdr &,
  lbot_arm_interfaces__msg__LbotFrame * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_lbot_arm_interfaces__msg__LbotFrame(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_lbot_arm_interfaces__msg__LbotFrame(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_key_lbot_arm_interfaces__msg__LbotFrame(
  const lbot_arm_interfaces__msg__LbotFrame * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_key_lbot_arm_interfaces__msg__LbotFrame(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_key_lbot_arm_interfaces__msg__LbotFrame(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, msg, LbotFrame)();

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
