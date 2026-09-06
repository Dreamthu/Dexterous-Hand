// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice
#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "lbot_arm_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "lbot_arm_interfaces/msg/detail/lbot_pose__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_lbot_arm_interfaces__msg__LbotPose(
  const lbot_arm_interfaces__msg__LbotPose * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_deserialize_lbot_arm_interfaces__msg__LbotPose(
  eprosima::fastcdr::Cdr &,
  lbot_arm_interfaces__msg__LbotPose * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_lbot_arm_interfaces__msg__LbotPose(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_lbot_arm_interfaces__msg__LbotPose(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
bool cdr_serialize_key_lbot_arm_interfaces__msg__LbotPose(
  const lbot_arm_interfaces__msg__LbotPose * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t get_serialized_size_key_lbot_arm_interfaces__msg__LbotPose(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
size_t max_serialized_size_key_lbot_arm_interfaces__msg__LbotPose(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_lbot_arm_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, msg, LbotPose)();

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
