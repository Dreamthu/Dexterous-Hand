// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/follow_joint.h"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_H_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_H_

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

/// Struct defined in msg/FollowJoint in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__msg__FollowJoint
{
  rosidl_runtime_c__float__Sequence joints;
  bool follow;
} lbot_arm_interfaces__msg__FollowJoint;

// Struct for a sequence of lbot_arm_interfaces__msg__FollowJoint.
typedef struct lbot_arm_interfaces__msg__FollowJoint__Sequence
{
  lbot_arm_interfaces__msg__FollowJoint * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__msg__FollowJoint__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__FOLLOW_JOINT__STRUCT_H_
