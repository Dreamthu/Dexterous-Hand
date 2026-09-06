// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:srv/InverseKinematics.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/srv/detail/inverse_kinematics__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__InverseKinematics__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x86, 0xb9, 0x47, 0xab, 0xe6, 0x87, 0x9e, 0x6f,
      0xcf, 0xea, 0xf8, 0x93, 0xd1, 0xe1, 0x26, 0xd6,
      0x3d, 0xc1, 0x8c, 0xa7, 0x9b, 0xfc, 0x83, 0x00,
      0x0e, 0x7c, 0xa8, 0xd7, 0x04, 0x1a, 0x71, 0x4e,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__InverseKinematics_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x34, 0x86, 0x3f, 0xc5, 0xfd, 0x3f, 0x67, 0x2d,
      0xcc, 0x5a, 0x32, 0x9c, 0x15, 0x4c, 0x40, 0xb8,
      0x83, 0x27, 0x29, 0xf7, 0x2f, 0xbe, 0x1a, 0xa1,
      0xdd, 0x6e, 0xfa, 0xb9, 0x98, 0x6f, 0x95, 0xc1,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__InverseKinematics_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x9f, 0x49, 0x13, 0xe9, 0x02, 0x45, 0x45, 0xaf,
      0x06, 0xad, 0x88, 0x95, 0x4b, 0x26, 0xdf, 0x66,
      0xde, 0x7f, 0x51, 0xba, 0xf8, 0xb7, 0x0f, 0x89,
      0xfc, 0x25, 0x5e, 0xa9, 0xff, 0x1f, 0x3f, 0x9e,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__InverseKinematics_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x86, 0xf9, 0xca, 0x85, 0xdc, 0xa5, 0x80, 0x4f,
      0x8c, 0x9c, 0xc4, 0xc7, 0xc6, 0xe8, 0x2e, 0x63,
      0xef, 0xcc, 0xe3, 0x2a, 0x58, 0x11, 0x3f, 0xa3,
      0x9f, 0xaf, 0x14, 0xd4, 0x2f, 0x4e, 0xca, 0x30,
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

static char lbot_arm_interfaces__srv__InverseKinematics__TYPE_NAME[] = "lbot_arm_interfaces/srv/InverseKinematics";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";
static char lbot_arm_interfaces__srv__InverseKinematics_Event__TYPE_NAME[] = "lbot_arm_interfaces/srv/InverseKinematics_Event";
static char lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME[] = "lbot_arm_interfaces/srv/InverseKinematics_Request";
static char lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME[] = "lbot_arm_interfaces/srv/InverseKinematics_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__request_message[] = "request_message";
static char lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__response_message[] = "response_message";
static char lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__InverseKinematics__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__InverseKinematics_Event__TYPE_NAME, 47, 47},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__InverseKinematics__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Event__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__InverseKinematics__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__InverseKinematics__TYPE_NAME, 41, 41},
      {lbot_arm_interfaces__srv__InverseKinematics__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__InverseKinematics__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__InverseKinematics_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__InverseKinematics_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lbot_arm_interfaces__srv__InverseKinematics_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__joints[] = "joints";
static char lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__position[] = "position";
static char lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__euler[] = "euler";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__InverseKinematics_Request__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__joints, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__position, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__FIELD_NAME__euler, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__InverseKinematics_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__InverseKinematics_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
      {lbot_arm_interfaces__srv__InverseKinematics_Request__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__InverseKinematics_Request__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__InverseKinematics_Response__FIELD_NAME__joints[] = "joints";
static char lbot_arm_interfaces__srv__InverseKinematics_Response__FIELD_NAME__success[] = "success";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__InverseKinematics_Response__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Response__FIELD_NAME__joints, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Response__FIELD_NAME__success, 7, 7},
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
lbot_arm_interfaces__srv__InverseKinematics_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
      {lbot_arm_interfaces__srv__InverseKinematics_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__info[] = "info";
static char lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__request[] = "request";
static char lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__InverseKinematics_Event__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__InverseKinematics_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__InverseKinematics_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__InverseKinematics_Event__TYPE_NAME, 47, 47},
      {lbot_arm_interfaces__srv__InverseKinematics_Event__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__InverseKinematics_Event__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__InverseKinematics_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__InverseKinematics_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float32[] joints                      # \\xe6\\xad\\xa4\\xe5\\x85\\xb3\\xe8\\x8a\\x82\\xe8\\xa7\\x92\\xe5\\xba\\xa6\\xe4\\xb8\\x8d\\xe8\\xae\\xbe\\xe7\\xbd\\xae\\xe4\\xbc\\x9a\\xe9\\xbb\\x98\\xe8\\xae\\xa4\\xe4\\xbb\\x8e\\xe6\\x9c\\xba\\xe6\\xa2\\xb0\\xe8\\x87\\x82\\xe8\\xaf\\xbb\\xe5\\x8f\\x96\\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe8\\xa7\\x92\\xe5\\xba\\xa6\\xef\\xbc\\x8c\\xe5\\xa6\\x82\\xe6\\x9e\\x9c\\xe8\\xae\\xbe\\xe7\\xbd\\xae\\xe5\\x88\\x99\\xe5\\x9f\\xba\\xe4\\xba\\x8e\\xe6\\xad\\xa4\\xe5\\x80\\xbc\\xe4\\xb8\\xba\\xe5\\x88\\x9d\\xe5\\xa7\\x8b\\xe8\\xa7\\x92\\xe5\\xba\\xa6\\xe8\\xbf\\x9b\\xe8\\xa1\\x8c\\xe9\\x80\\x86\\xe8\\xa7\\xa3\n"
  "geometry_msgs/Vector3 position\n"
  "geometry_msgs/Vector3 euler\n"
  "---\n"
  "float32[] joints\n"
  "bool success";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__InverseKinematics__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__InverseKinematics__TYPE_NAME, 41, 41},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 173, 173},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__InverseKinematics_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__InverseKinematics_Request__TYPE_NAME, 49, 49},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__InverseKinematics_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__InverseKinematics_Response__TYPE_NAME, 50, 50},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__InverseKinematics_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__InverseKinematics_Event__TYPE_NAME, 47, 47},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__InverseKinematics__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__InverseKinematics__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__InverseKinematics_Event__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__InverseKinematics_Request__get_individual_type_description_source(NULL);
    sources[5] = *lbot_arm_interfaces__srv__InverseKinematics_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__InverseKinematics_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__InverseKinematics_Request__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__InverseKinematics_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__InverseKinematics_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__InverseKinematics_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__InverseKinematics_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__InverseKinematics_Request__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__InverseKinematics_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
