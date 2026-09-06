// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lbot_arm_interfaces:srv/GetCurrentFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/get_current_frame.h"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__STRUCT_H_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/GetCurrentFrame in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Request
{
  uint8_t structure_needs_at_least_one_member;
} lbot_arm_interfaces__srv__GetCurrentFrame_Request;

// Struct for a sequence of lbot_arm_interfaces__srv__GetCurrentFrame_Request.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"
// Member 'frame'
#include "lbot_arm_interfaces/msg/detail/lbot_frame__struct.h"

/// Struct defined in srv/GetCurrentFrame in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Response
{
  rosidl_runtime_c__String name;
  lbot_arm_interfaces__msg__LbotFrame frame;
  bool success;
} lbot_arm_interfaces__srv__GetCurrentFrame_Response;

// Struct for a sequence of lbot_arm_interfaces__srv__GetCurrentFrame_Response.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__request__MAX_SIZE = 1
};
// response
enum
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/GetCurrentFrame in the package lbot_arm_interfaces.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Event
{
  service_msgs__msg__ServiceEventInfo info;
  lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence request;
  lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence response;
} lbot_arm_interfaces__srv__GetCurrentFrame_Event;

// Struct for a sequence of lbot_arm_interfaces__srv__GetCurrentFrame_Event.
typedef struct lbot_arm_interfaces__srv__GetCurrentFrame_Event__Sequence
{
  lbot_arm_interfaces__srv__GetCurrentFrame_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lbot_arm_interfaces__srv__GetCurrentFrame_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__STRUCT_H_
