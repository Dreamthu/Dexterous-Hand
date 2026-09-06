// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/arm_state.h"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_H_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'joints'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'euler'
#include "geometry_msgs/msg/detail/vector3__struct.h"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in msg/ArmState in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__msg__ArmState
{
  rosidl_runtime_c__float__Sequence joints;
  geometry_msgs__msg__Vector3 euler;
  geometry_msgs__msg__Pose pose;
} lbot_arm_interfaces__msg__ArmState;

// Struct for a sequence of lbot_arm_interfaces__msg__ArmState.
typedef struct lbot_arm_interfaces__msg__ArmState__Sequence
{
  lbot_arm_interfaces__msg__ArmState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__msg__ArmState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__STRUCT_H_
