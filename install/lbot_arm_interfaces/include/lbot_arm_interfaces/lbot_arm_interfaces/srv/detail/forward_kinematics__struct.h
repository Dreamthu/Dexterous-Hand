// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lbot_arm_interfaces:srv/ForwardKinematics.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/forward_kinematics.h"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__STRUCT_H_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__STRUCT_H_

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

/// Struct defined in srv/ForwardKinematics in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Request
{
  rosidl_runtime_c__float__Sequence joints;
} lbot_arm_interfaces__srv__ForwardKinematics_Request;

// Struct for a sequence of lbot_arm_interfaces__srv__ForwardKinematics_Request.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence
{
  lbot_arm_interfaces__srv__ForwardKinematics_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'position'
// Member 'euler'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in srv/ForwardKinematics in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Response
{
  geometry_msgs__msg__Vector3 position;
  geometry_msgs__msg__Vector3 euler;
  bool success;
} lbot_arm_interfaces__srv__ForwardKinematics_Response;

// Struct for a sequence of lbot_arm_interfaces__srv__ForwardKinematics_Response.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence
{
  lbot_arm_interfaces__srv__ForwardKinematics_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  lbot_arm_interfaces__srv__ForwardKinematics_Event__request__MAX_SIZE = 1
};
// response
enum
{
  lbot_arm_interfaces__srv__ForwardKinematics_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/ForwardKinematics in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Event
{
  service_msgs__msg__ServiceEventInfo info;
  lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence request;
  lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence response;
} lbot_arm_interfaces__srv__ForwardKinematics_Event;

// Struct for a sequence of lbot_arm_interfaces__srv__ForwardKinematics_Event.
typedef struct lbot_arm_interfaces__srv__ForwardKinematics_Event__Sequence
{
  lbot_arm_interfaces__srv__ForwardKinematics_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__ForwardKinematics_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__FORWARD_KINEMATICS__STRUCT_H_
