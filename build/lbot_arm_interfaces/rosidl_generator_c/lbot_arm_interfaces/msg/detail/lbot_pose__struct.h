// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/lbot_pose.h"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_H_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'euler'
// Member 'position'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/LbotPose in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__msg__LbotPose
{
  geometry_msgs__msg__Vector3 euler;
  geometry_msgs__msg__Vector3 position;
} lbot_arm_interfaces__msg__LbotPose;

// Struct for a sequence of lbot_arm_interfaces__msg__LbotPose.
typedef struct lbot_arm_interfaces__msg__LbotPose__Sequence
{
  lbot_arm_interfaces__msg__LbotPose * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__msg__LbotPose__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_POSE__STRUCT_H_
