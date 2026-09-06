// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:srv/GetCurrentFrame.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
#include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _GetCurrentFrame_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetCurrentFrame_Request_type_support_ids_t;

static const _GetCurrentFrame_Request_type_support_ids_t _GetCurrentFrame_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetCurrentFrame_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetCurrentFrame_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetCurrentFrame_Request_type_support_symbol_names_t _GetCurrentFrame_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Request)),
  }
};

typedef struct _GetCurrentFrame_Request_type_support_data_t
{
  void * data[2];
} _GetCurrentFrame_Request_type_support_data_t;

static _GetCurrentFrame_Request_type_support_data_t _GetCurrentFrame_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetCurrentFrame_Request_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetCurrentFrame_Request_message_typesupport_ids.typesupport_identifier[0],
  &_GetCurrentFrame_Request_message_typesupport_symbol_names.symbol_name[0],
  &_GetCurrentFrame_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetCurrentFrame_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetCurrentFrame_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Request>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetCurrentFrame_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Request)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _GetCurrentFrame_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetCurrentFrame_Response_type_support_ids_t;

static const _GetCurrentFrame_Response_type_support_ids_t _GetCurrentFrame_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetCurrentFrame_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetCurrentFrame_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetCurrentFrame_Response_type_support_symbol_names_t _GetCurrentFrame_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Response)),
  }
};

typedef struct _GetCurrentFrame_Response_type_support_data_t
{
  void * data[2];
} _GetCurrentFrame_Response_type_support_data_t;

static _GetCurrentFrame_Response_type_support_data_t _GetCurrentFrame_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetCurrentFrame_Response_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetCurrentFrame_Response_message_typesupport_ids.typesupport_identifier[0],
  &_GetCurrentFrame_Response_message_typesupport_symbol_names.symbol_name[0],
  &_GetCurrentFrame_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetCurrentFrame_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetCurrentFrame_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Response>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetCurrentFrame_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Response)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _GetCurrentFrame_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetCurrentFrame_Event_type_support_ids_t;

static const _GetCurrentFrame_Event_type_support_ids_t _GetCurrentFrame_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetCurrentFrame_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetCurrentFrame_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetCurrentFrame_Event_type_support_symbol_names_t _GetCurrentFrame_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Event)),
  }
};

typedef struct _GetCurrentFrame_Event_type_support_data_t
{
  void * data[2];
} _GetCurrentFrame_Event_type_support_data_t;

static _GetCurrentFrame_Event_type_support_data_t _GetCurrentFrame_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetCurrentFrame_Event_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetCurrentFrame_Event_message_typesupport_ids.typesupport_identifier[0],
  &_GetCurrentFrame_Event_message_typesupport_symbol_names.symbol_name[0],
  &_GetCurrentFrame_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetCurrentFrame_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetCurrentFrame_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Event>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetCurrentFrame_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetCurrentFrame_Event)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lbot_arm_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _GetCurrentFrame_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetCurrentFrame_type_support_ids_t;

static const _GetCurrentFrame_type_support_ids_t _GetCurrentFrame_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetCurrentFrame_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetCurrentFrame_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetCurrentFrame_type_support_symbol_names_t _GetCurrentFrame_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetCurrentFrame)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetCurrentFrame)),
  }
};

typedef struct _GetCurrentFrame_type_support_data_t
{
  void * data[2];
} _GetCurrentFrame_type_support_data_t;

static _GetCurrentFrame_type_support_data_t _GetCurrentFrame_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetCurrentFrame_service_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetCurrentFrame_service_typesupport_ids.typesupport_identifier[0],
  &_GetCurrentFrame_service_typesupport_symbol_names.symbol_name[0],
  &_GetCurrentFrame_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t GetCurrentFrame_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetCurrentFrame_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<lbot_arm_interfaces::srv::GetCurrentFrame>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<lbot_arm_interfaces::srv::GetCurrentFrame>,
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_hash,
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_description,
  &lbot_arm_interfaces__srv__GetCurrentFrame__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetCurrentFrame_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetCurrentFrame)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<lbot_arm_interfaces::srv::GetCurrentFrame>();
}

#ifdef __cplusplus
}
#endif
