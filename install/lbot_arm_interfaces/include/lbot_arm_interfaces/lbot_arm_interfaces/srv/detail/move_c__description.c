// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:srv/MoveC.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/srv/detail/move_c__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__MoveC__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf4, 0xc8, 0x3b, 0xd1, 0xdf, 0x5b, 0xa5, 0xa2,
      0x89, 0x80, 0xa6, 0x70, 0x75, 0x11, 0x7c, 0x29,
      0xc5, 0xb7, 0x07, 0xb3, 0x7b, 0x29, 0xe5, 0xd2,
      0xc8, 0x01, 0xe8, 0xbc, 0x32, 0xbc, 0x59, 0xc5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__MoveC_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xef, 0x93, 0xfb, 0x39, 0xb5, 0x97, 0x14, 0xbd,
      0x95, 0xbd, 0x5a, 0x59, 0xab, 0x03, 0x4c, 0x1e,
      0x29, 0xc9, 0xf6, 0x74, 0x76, 0xaf, 0x7d, 0x25,
      0xa1, 0xac, 0x72, 0xab, 0xae, 0x1a, 0x49, 0x82,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__MoveC_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xcc, 0xf7, 0x07, 0x87, 0x8c, 0x3e, 0xc7, 0x08,
      0x7b, 0xc6, 0x54, 0xfd, 0x11, 0x52, 0xb5, 0xb3,
      0x4e, 0x95, 0x0b, 0x1b, 0x6f, 0xc3, 0x0f, 0x52,
      0xac, 0x54, 0x59, 0x4a, 0x40, 0xf7, 0x29, 0x87,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__MoveC_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5f, 0xa6, 0x8c, 0x01, 0x64, 0x31, 0xa0, 0x6b,
      0xbe, 0xf8, 0xdc, 0x34, 0x29, 0x44, 0x8e, 0x83,
      0xc5, 0x19, 0x69, 0xe4, 0xad, 0x93, 0xdd, 0x53,
      0xc1, 0xf3, 0xd8, 0x90, 0x45, 0x03, 0xa3, 0x51,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "geometry_msgs/msg/detail/vector3__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char lbot_arm_interfaces__srv__MoveC__TYPE_NAME[] = "lbot_arm_interfaces/srv/MoveC";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";
static char lbot_arm_interfaces__srv__MoveC_Event__TYPE_NAME[] = "lbot_arm_interfaces/srv/MoveC_Event";
static char lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME[] = "lbot_arm_interfaces/srv/MoveC_Request";
static char lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME[] = "lbot_arm_interfaces/srv/MoveC_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__MoveC__FIELD_NAME__request_message[] = "request_message";
static char lbot_arm_interfaces__srv__MoveC__FIELD_NAME__response_message[] = "response_message";
static char lbot_arm_interfaces__srv__MoveC__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__MoveC__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__MoveC__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__MoveC_Event__TYPE_NAME, 35, 35},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__MoveC__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Event__TYPE_NAME, 35, 35},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__MoveC__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__MoveC__TYPE_NAME, 29, 29},
      {lbot_arm_interfaces__srv__MoveC__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__MoveC__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__MoveC_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__MoveC_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lbot_arm_interfaces__srv__MoveC_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__position[] = "position";
static char lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__euler[] = "euler";
static char lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__speed[] = "speed";
static char lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__acce[] = "acce";
static char lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__block[] = "block";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__MoveC_Request__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__position, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__euler, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__speed, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__acce, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__FIELD_NAME__block, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__MoveC_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__MoveC_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
      {lbot_arm_interfaces__srv__MoveC_Request__FIELDS, 5, 5},
    },
    {lbot_arm_interfaces__srv__MoveC_Request__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__MoveC_Response__FIELD_NAME__success[] = "success";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__MoveC_Response__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__MoveC_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__MoveC_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
      {lbot_arm_interfaces__srv__MoveC_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__info[] = "info";
static char lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__request[] = "request";
static char lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__MoveC_Event__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__MoveC_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__MoveC_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__MoveC_Event__TYPE_NAME, 35, 35},
      {lbot_arm_interfaces__srv__MoveC_Event__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__MoveC_Event__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__MoveC_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__MoveC_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "geometry_msgs/Vector3 position\n"
  "geometry_msgs/Vector3 euler\n"
  "float32 speed\n"
  "float32 acce\n"
  "bool block\n"
  "\n"
  "---\n"
  "\n"
  "bool success";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__MoveC__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__MoveC__TYPE_NAME, 29, 29},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 115, 115},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__MoveC_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__MoveC_Request__TYPE_NAME, 37, 37},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__MoveC_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__MoveC_Response__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__MoveC_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__MoveC_Event__TYPE_NAME, 35, 35},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__MoveC__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__MoveC__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__MoveC_Event__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__MoveC_Request__get_individual_type_description_source(NULL);
    sources[5] = *lbot_arm_interfaces__srv__MoveC_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__MoveC_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__MoveC_Request__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__MoveC_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__MoveC_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__MoveC_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__MoveC_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__MoveC_Request__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__MoveC_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
