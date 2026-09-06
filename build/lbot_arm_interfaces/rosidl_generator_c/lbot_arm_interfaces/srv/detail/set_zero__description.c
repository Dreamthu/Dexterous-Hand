// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:srv/SetZero.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/srv/detail/set_zero__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetZero__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x7e, 0x88, 0x9b, 0x82, 0xe8, 0x5a, 0xef, 0xca,
      0xc2, 0x80, 0x14, 0x7f, 0x52, 0xe4, 0x7c, 0xbf,
      0x3f, 0x06, 0xf2, 0x3d, 0x80, 0x94, 0x86, 0xcc,
      0xb5, 0x44, 0xba, 0x61, 0x91, 0xab, 0x14, 0x56,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetZero_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x49, 0x64, 0x2e, 0xee, 0x96, 0x9b, 0x58, 0x8d,
      0x99, 0xd2, 0x35, 0x27, 0xf2, 0x7b, 0x76, 0x45,
      0x77, 0xbe, 0xbd, 0x24, 0xc8, 0x5d, 0xe5, 0x6a,
      0x96, 0x0a, 0xf6, 0xa6, 0xb0, 0x4b, 0xb9, 0xb4,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetZero_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x9e, 0x62, 0x01, 0x9f, 0xd3, 0x80, 0x94, 0x01,
      0xe9, 0x7d, 0xef, 0x6b, 0x24, 0x48, 0x2f, 0x71,
      0xab, 0x68, 0x91, 0xe0, 0x16, 0x5e, 0xb1, 0x7d,
      0xc8, 0x6e, 0x26, 0xfd, 0x08, 0x2a, 0xf7, 0xb1,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetZero_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x36, 0x19, 0x38, 0xf2, 0x51, 0xab, 0x04, 0xe1,
      0x25, 0xa7, 0xae, 0xd9, 0x98, 0x37, 0x40, 0x4c,
      0xa4, 0xe4, 0x43, 0x69, 0xd8, 0xe6, 0x7e, 0xf1,
      0x52, 0x94, 0x28, 0x50, 0x03, 0x39, 0x66, 0x0b,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char lbot_arm_interfaces__srv__SetZero__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetZero";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char lbot_arm_interfaces__srv__SetZero_Event__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetZero_Event";
static char lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetZero_Request";
static char lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetZero_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetZero__FIELD_NAME__request_message[] = "request_message";
static char lbot_arm_interfaces__srv__SetZero__FIELD_NAME__response_message[] = "response_message";
static char lbot_arm_interfaces__srv__SetZero__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetZero__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetZero__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetZero_Event__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__SetZero__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Event__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetZero__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetZero__TYPE_NAME, 31, 31},
      {lbot_arm_interfaces__srv__SetZero__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__SetZero__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = lbot_arm_interfaces__srv__SetZero_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__SetZero_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__SetZero_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetZero_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetZero_Request__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetZero_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetZero_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
      {lbot_arm_interfaces__srv__SetZero_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetZero_Response__FIELD_NAME__success[] = "success";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetZero_Response__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetZero_Response__FIELD_NAME__success, 7, 7},
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
lbot_arm_interfaces__srv__SetZero_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
      {lbot_arm_interfaces__srv__SetZero_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__info[] = "info";
static char lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__request[] = "request";
static char lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetZero_Event__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__SetZero_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetZero_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetZero_Event__TYPE_NAME, 37, 37},
      {lbot_arm_interfaces__srv__SetZero_Event__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__SetZero_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = lbot_arm_interfaces__srv__SetZero_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__srv__SetZero_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "\n"
  "---\n"
  "bool success";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetZero__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetZero__TYPE_NAME, 31, 31},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 18, 18},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetZero_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetZero_Request__TYPE_NAME, 39, 39},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetZero_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetZero_Response__TYPE_NAME, 40, 40},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetZero_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetZero_Event__TYPE_NAME, 37, 37},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetZero__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetZero__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lbot_arm_interfaces__srv__SetZero_Event__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__SetZero_Request__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__SetZero_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetZero_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetZero_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetZero_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetZero_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetZero_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetZero_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lbot_arm_interfaces__srv__SetZero_Request__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__srv__SetZero_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
