// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:srv/GetFrame.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "lbot_arm_interfaces/srv/detail/get_frame__functions.h"
#include "lbot_arm_interfaces/srv/detail/get_frame__struct.hpp"
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

typedef struct _GetFrame_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetFrame_Request_type_support_ids_t;

static const _GetFrame_Request_type_support_ids_t _GetFrame_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetFrame_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetFrame_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetFrame_Request_type_support_symbol_names_t _GetFrame_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetFrame_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetFrame_Request)),
  }
};

typedef struct _GetFrame_Request_type_support_data_t
{
  void * data[2];
} _GetFrame_Request_type_support_data_t;

static _GetFrame_Request_type_support_data_t _GetFrame_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetFrame_Request_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetFrame_Request_message_typesupport_ids.typesupport_identifier[0],
  &_GetFrame_Request_message_typesupport_symbol_names.symbol_name[0],
  &_GetFrame_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetFrame_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetFrame_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetFrame_Request__get_type_hash,
  &lbot_arm_interfaces__srv__GetFrame_Request__get_type_description,
  &lbot_arm_interfaces__srv__GetFrame_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Request>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetFrame_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetFrame_Request)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Request>();
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
// #include "lbot_arm_interfaces/srv/detail/get_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_frame__struct.hpp"
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

typedef struct _GetFrame_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetFrame_Response_type_support_ids_t;

static const _GetFrame_Response_type_support_ids_t _GetFrame_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetFrame_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetFrame_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetFrame_Response_type_support_symbol_names_t _GetFrame_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetFrame_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetFrame_Response)),
  }
};

typedef struct _GetFrame_Response_type_support_data_t
{
  void * data[2];
} _GetFrame_Response_type_support_data_t;

static _GetFrame_Response_type_support_data_t _GetFrame_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetFrame_Response_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetFrame_Response_message_typesupport_ids.typesupport_identifier[0],
  &_GetFrame_Response_message_typesupport_symbol_names.symbol_name[0],
  &_GetFrame_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetFrame_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetFrame_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetFrame_Response__get_type_hash,
  &lbot_arm_interfaces__srv__GetFrame_Response__get_type_description,
  &lbot_arm_interfaces__srv__GetFrame_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Response>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetFrame_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetFrame_Response)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Response>();
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
// #include "lbot_arm_interfaces/srv/detail/get_frame__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/get_frame__struct.hpp"
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

typedef struct _GetFrame_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetFrame_Event_type_support_ids_t;

static const _GetFrame_Event_type_support_ids_t _GetFrame_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetFrame_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetFrame_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetFrame_Event_type_support_symbol_names_t _GetFrame_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetFrame_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetFrame_Event)),
  }
};

typedef struct _GetFrame_Event_type_support_data_t
{
  void * data[2];
} _GetFrame_Event_type_support_data_t;

static _GetFrame_Event_type_support_data_t _GetFrame_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetFrame_Event_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetFrame_Event_message_typesupport_ids.typesupport_identifier[0],
  &_GetFrame_Event_message_typesupport_symbol_names.symbol_name[0],
  &_GetFrame_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t GetFrame_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetFrame_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__GetFrame_Event__get_type_hash,
  &lbot_arm_interfaces__srv__GetFrame_Event__get_type_description,
  &lbot_arm_interfaces__srv__GetFrame_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Event>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetFrame_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetFrame_Event)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Event>();
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
// #include "lbot_arm_interfaces/srv/detail/get_frame__struct.hpp"
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

typedef struct _GetFrame_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _GetFrame_type_support_ids_t;

static const _GetFrame_type_support_ids_t _GetFrame_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _GetFrame_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _GetFrame_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _GetFrame_type_support_symbol_names_t _GetFrame_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, GetFrame)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, GetFrame)),
  }
};

typedef struct _GetFrame_type_support_data_t
{
  void * data[2];
} _GetFrame_type_support_data_t;

static _GetFrame_type_support_data_t _GetFrame_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _GetFrame_service_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_GetFrame_service_typesupport_ids.typesupport_identifier[0],
  &_GetFrame_service_typesupport_symbol_names.symbol_name[0],
  &_GetFrame_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t GetFrame_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_GetFrame_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::GetFrame_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<lbot_arm_interfaces::srv::GetFrame>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<lbot_arm_interfaces::srv::GetFrame>,
  &lbot_arm_interfaces__srv__GetFrame__get_type_hash,
  &lbot_arm_interfaces__srv__GetFrame__get_type_description,
  &lbot_arm_interfaces__srv__GetFrame__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<lbot_arm_interfaces::srv::GetFrame>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::GetFrame_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, GetFrame)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<lbot_arm_interfaces::srv::GetFrame>();
}

#ifdef __cplusplus
}
#endif
