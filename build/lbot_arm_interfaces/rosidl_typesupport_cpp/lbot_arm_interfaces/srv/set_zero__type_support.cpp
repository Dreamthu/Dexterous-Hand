// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from lbot_arm_interfaces:srv/SetZero.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "lbot_arm_interfaces/srv/detail/set_zero__functions.h"
#include "lbot_arm_interfaces/srv/detail/set_zero__struct.hpp"
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

typedef struct _SetZero_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetZero_Request_type_support_ids_t;

static const _SetZero_Request_type_support_ids_t _SetZero_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetZero_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetZero_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetZero_Request_type_support_symbol_names_t _SetZero_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, SetZero_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, SetZero_Request)),
  }
};

typedef struct _SetZero_Request_type_support_data_t
{
  void * data[2];
} _SetZero_Request_type_support_data_t;

static _SetZero_Request_type_support_data_t _SetZero_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetZero_Request_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_SetZero_Request_message_typesupport_ids.typesupport_identifier[0],
  &_SetZero_Request_message_typesupport_symbol_names.symbol_name[0],
  &_SetZero_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetZero_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetZero_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__SetZero_Request__get_type_hash,
  &lbot_arm_interfaces__srv__SetZero_Request__get_type_description,
  &lbot_arm_interfaces__srv__SetZero_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Request>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::SetZero_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, SetZero_Request)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Request>();
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
// #include "lbot_arm_interfaces/srv/detail/set_zero__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/set_zero__struct.hpp"
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

typedef struct _SetZero_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetZero_Response_type_support_ids_t;

static const _SetZero_Response_type_support_ids_t _SetZero_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetZero_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetZero_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetZero_Response_type_support_symbol_names_t _SetZero_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, SetZero_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, SetZero_Response)),
  }
};

typedef struct _SetZero_Response_type_support_data_t
{
  void * data[2];
} _SetZero_Response_type_support_data_t;

static _SetZero_Response_type_support_data_t _SetZero_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetZero_Response_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_SetZero_Response_message_typesupport_ids.typesupport_identifier[0],
  &_SetZero_Response_message_typesupport_symbol_names.symbol_name[0],
  &_SetZero_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetZero_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetZero_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__SetZero_Response__get_type_hash,
  &lbot_arm_interfaces__srv__SetZero_Response__get_type_description,
  &lbot_arm_interfaces__srv__SetZero_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Response>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::SetZero_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, SetZero_Response)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Response>();
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
// #include "lbot_arm_interfaces/srv/detail/set_zero__functions.h"
// already included above
// #include "lbot_arm_interfaces/srv/detail/set_zero__struct.hpp"
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

typedef struct _SetZero_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetZero_Event_type_support_ids_t;

static const _SetZero_Event_type_support_ids_t _SetZero_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetZero_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetZero_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetZero_Event_type_support_symbol_names_t _SetZero_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, SetZero_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, SetZero_Event)),
  }
};

typedef struct _SetZero_Event_type_support_data_t
{
  void * data[2];
} _SetZero_Event_type_support_data_t;

static _SetZero_Event_type_support_data_t _SetZero_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetZero_Event_message_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_SetZero_Event_message_typesupport_ids.typesupport_identifier[0],
  &_SetZero_Event_message_typesupport_symbol_names.symbol_name[0],
  &_SetZero_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetZero_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetZero_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lbot_arm_interfaces__srv__SetZero_Event__get_type_hash,
  &lbot_arm_interfaces__srv__SetZero_Event__get_type_description,
  &lbot_arm_interfaces__srv__SetZero_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Event>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::SetZero_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, SetZero_Event)() {
  return get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Event>();
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
// #include "lbot_arm_interfaces/srv/detail/set_zero__struct.hpp"
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

typedef struct _SetZero_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetZero_type_support_ids_t;

static const _SetZero_type_support_ids_t _SetZero_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetZero_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetZero_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetZero_type_support_symbol_names_t _SetZero_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lbot_arm_interfaces, srv, SetZero)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lbot_arm_interfaces, srv, SetZero)),
  }
};

typedef struct _SetZero_type_support_data_t
{
  void * data[2];
} _SetZero_type_support_data_t;

static _SetZero_type_support_data_t _SetZero_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetZero_service_typesupport_map = {
  2,
  "lbot_arm_interfaces",
  &_SetZero_service_typesupport_ids.typesupport_identifier[0],
  &_SetZero_service_typesupport_symbol_names.symbol_name[0],
  &_SetZero_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t SetZero_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetZero_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lbot_arm_interfaces::srv::SetZero_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<lbot_arm_interfaces::srv::SetZero>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<lbot_arm_interfaces::srv::SetZero>,
  &lbot_arm_interfaces__srv__SetZero__get_type_hash,
  &lbot_arm_interfaces__srv__SetZero__get_type_description,
  &lbot_arm_interfaces__srv__SetZero__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<lbot_arm_interfaces::srv::SetZero>()
{
  return &::lbot_arm_interfaces::srv::rosidl_typesupport_cpp::SetZero_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, lbot_arm_interfaces, srv, SetZero)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<lbot_arm_interfaces::srv::SetZero>();
}

#ifdef __cplusplus
}
#endif
