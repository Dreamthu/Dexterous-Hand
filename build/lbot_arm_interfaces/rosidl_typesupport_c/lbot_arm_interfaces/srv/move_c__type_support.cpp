// generated from rosidl_typesupport_c/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:srv/MoveC.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "lbot_arm_interfaces/srv/detail/move_c__struct.h"
#include "lbot_arm_interfaces/srv/detail/move_c__type_support.h"
#include "lbot_arm_interfaces/srv/detail/move_c__functions.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _MoveC_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveC_Request_type_support_ids_t;

static const _MoveC_Request_type_support_ids_t _MoveC_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _MoveC_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveC_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveC_Request_type_support_symbol_names_t _MoveC_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, srv, MoveC_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, MoveC_Request)),
  }
};

typedef struct _MoveC_Request_type_support_data_t
{
  void * data[2];
} _MoveC_Request_type_support_data_t;

static _MoveC_Request_type_support_data_t _MoveC_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveC_Request_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_MoveC_Request_message_typesupport_ids.typesupport_identifier[0],
  &_MoveC_Request_message_typesupport_symbol_names.symbol_name[0],
  &_MoveC_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveC_Request_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveC_Request_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__MoveC_Request__get_type_hash,
  &lbot_arm_interfaces__srv__MoveC_Request__get_type_description,
  &lbot_arm_interfaces__srv__MoveC_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace lbot_arm_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, lbot_arm_interfaces, srv, MoveC_Request)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_c::MoveC_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__type_support.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _MoveC_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveC_Response_type_support_ids_t;

static const _MoveC_Response_type_support_ids_t _MoveC_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _MoveC_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveC_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveC_Response_type_support_symbol_names_t _MoveC_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, srv, MoveC_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, MoveC_Response)),
  }
};

typedef struct _MoveC_Response_type_support_data_t
{
  void * data[2];
} _MoveC_Response_type_support_data_t;

static _MoveC_Response_type_support_data_t _MoveC_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveC_Response_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_MoveC_Response_message_typesupport_ids.typesupport_identifier[0],
  &_MoveC_Response_message_typesupport_symbol_names.symbol_name[0],
  &_MoveC_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveC_Response_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveC_Response_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__MoveC_Response__get_type_hash,
  &lbot_arm_interfaces__srv__MoveC_Response__get_type_description,
  &lbot_arm_interfaces__srv__MoveC_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace lbot_arm_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, lbot_arm_interfaces, srv, MoveC_Response)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_c::MoveC_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__type_support.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _MoveC_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveC_Event_type_support_ids_t;

static const _MoveC_Event_type_support_ids_t _MoveC_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _MoveC_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveC_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveC_Event_type_support_symbol_names_t _MoveC_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, srv, MoveC_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, MoveC_Event)),
  }
};

typedef struct _MoveC_Event_type_support_data_t
{
  void * data[2];
} _MoveC_Event_type_support_data_t;

static _MoveC_Event_type_support_data_t _MoveC_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveC_Event_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_MoveC_Event_message_typesupport_ids.typesupport_identifier[0],
  &_MoveC_Event_message_typesupport_symbol_names.symbol_name[0],
  &_MoveC_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveC_Event_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveC_Event_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__MoveC_Event__get_type_hash,
  &lbot_arm_interfaces__srv__MoveC_Event__get_type_description,
  &lbot_arm_interfaces__srv__MoveC_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace lbot_arm_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, lbot_arm_interfaces, srv, MoveC_Event)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_c::MoveC_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/move_c__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/service_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
#include "service_msgs/msg/service_event_info.h"
#include "builtin_interfaces/msg/time.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{
typedef struct _MoveC_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveC_type_support_ids_t;

static const _MoveC_type_support_ids_t _MoveC_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _MoveC_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveC_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveC_type_support_symbol_names_t _MoveC_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, lbot_arm_interfaces, srv, MoveC)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lbot_arm_interfaces, srv, MoveC)),
  }
};

typedef struct _MoveC_type_support_data_t
{
  void * data[2];
} _MoveC_type_support_data_t;

static _MoveC_type_support_data_t _MoveC_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveC_service_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_MoveC_service_typesupport_ids.typesupport_identifier[0],
  &_MoveC_service_typesupport_symbol_names.symbol_name[0],
  &_MoveC_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t MoveC_service_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveC_service_typesupport_map),
  rosidl_typesupport_c__get_service_typesupport_handle_function,
  &MoveC_Request_message_type_support_handle,
  &MoveC_Response_message_type_support_handle,
  &MoveC_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    lbot_arm_interfaces,
    srv,
    MoveC
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    lbot_arm_interfaces,
    srv,
    MoveC
  ),
  &lbot_arm_interfaces__srv__MoveC__get_type_hash,
  &lbot_arm_interfaces__srv__MoveC__get_type_description,
  &lbot_arm_interfaces__srv__MoveC__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace lbot_arm_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_c, lbot_arm_interfaces, srv, MoveC)() {
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_c::MoveC_service_type_support_handle;
}

#ifdef __cplusplus
}
#endif
