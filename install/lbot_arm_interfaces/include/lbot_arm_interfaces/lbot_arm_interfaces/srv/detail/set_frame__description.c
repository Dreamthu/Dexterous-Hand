// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:srv/SetFrame.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/srv/detail/set_frame__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetFrame__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3b, 0x19, 0x82, 0x63, 0x38, 0x24, 0x5d, 0x65,
      0x1d, 0x0f, 0xa5, 0x7a, 0x82, 0xe9, 0x58, 0x93,
      0x7e, 0x14, 0xdf, 0x78, 0xd9, 0x16, 0x77, 0xb5,
      0xac, 0x73, 0xb3, 0x88, 0xa1, 0x1d, 0xc2, 0x37,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetFrame_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x60, 0xa4, 0xd8, 0xa3, 0x9c, 0x94, 0xd8, 0xeb,
      0x90, 0x37, 0x7c, 0xe1, 0x64, 0xb3, 0x13, 0xa8,
      0xc3, 0x82, 0xcb, 0xe7, 0x15, 0xd6, 0x72, 0xfe,
      0x47, 0x6c, 0xbd, 0x9e, 0xdc, 0x17, 0xd5, 0x96,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetFrame_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3a, 0xcd, 0x47, 0xc4, 0x98, 0xc2, 0xaa, 0xd9,
      0x3d, 0xb3, 0xf9, 0xc3, 0x10, 0x4d, 0x8a, 0xfa,
      0xc6, 0xfb, 0x30, 0x4f, 0xc0, 0x6b, 0xa3, 0x7c,
      0x6d, 0x7e, 0xe4, 0xe1, 0x6d, 0xc3, 0x53, 0x72,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__srv__SetFrame_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa4, 0x1e, 0x1a, 0xd6, 0xda, 0x66, 0xe8, 0x06,
      0x69, 0xb3, 0x91, 0x15, 0xfb, 0x71, 0xab, 0x6a,
      0x5d, 0xff, 0x08, 0x8f, 0xe9, 0x26, 0x30, 0xa8,
      0xaa, 0x77, 0xf7, 0x4a, 0xb2, 0xed, 0xa0, 0xce,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "lbot_arm_interfaces/msg/detail/lbot_frame__functions.h"
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
static const rosidl_type_hash_t lbot_arm_interfaces__msg__LbotFrame__EXPECTED_HASH = {1, {
    0xee, 0x79, 0x47, 0xba, 0x8f, 0xfc, 0x96, 0xd2,
    0xac, 0xdb, 0xd0, 0x0c, 0x68, 0xcc, 0x4c, 0xfc,
    0x9d, 0x6b, 0xab, 0xf7, 0x8a, 0x99, 0xa5, 0x32,
    0x45, 0x1d, 0x9c, 0x5d, 0xaa, 0x67, 0x9a, 0x29,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char lbot_arm_interfaces__srv__SetFrame__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetFrame";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";
static char lbot_arm_interfaces__msg__LbotFrame__TYPE_NAME[] = "lbot_arm_interfaces/msg/LbotFrame";
static char lbot_arm_interfaces__srv__SetFrame_Event__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetFrame_Event";
static char lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetFrame_Request";
static char lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME[] = "lbot_arm_interfaces/srv/SetFrame_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__request_message[] = "request_message";
static char lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__response_message[] = "response_message";
static char lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetFrame__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__srv__SetFrame_Event__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__SetFrame__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__msg__LbotFrame__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Event__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetFrame__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetFrame__TYPE_NAME, 32, 32},
      {lbot_arm_interfaces__srv__SetFrame__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__SetFrame__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lbot_arm_interfaces__msg__LbotFrame__EXPECTED_HASH, lbot_arm_interfaces__msg__LbotFrame__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__msg__LbotFrame__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__SetFrame_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lbot_arm_interfaces__srv__SetFrame_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = lbot_arm_interfaces__srv__SetFrame_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetFrame_Request__FIELD_NAME__frame[] = "frame";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetFrame_Request__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetFrame_Request__FIELD_NAME__frame, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lbot_arm_interfaces__msg__LbotFrame__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__SetFrame_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__msg__LbotFrame__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetFrame_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
      {lbot_arm_interfaces__srv__SetFrame_Request__FIELDS, 1, 1},
    },
    {lbot_arm_interfaces__srv__SetFrame_Request__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lbot_arm_interfaces__msg__LbotFrame__EXPECTED_HASH, lbot_arm_interfaces__msg__LbotFrame__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lbot_arm_interfaces__msg__LbotFrame__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetFrame_Response__FIELD_NAME__success[] = "success";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetFrame_Response__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetFrame_Response__FIELD_NAME__success, 7, 7},
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
lbot_arm_interfaces__srv__SetFrame_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
      {lbot_arm_interfaces__srv__SetFrame_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__info[] = "info";
static char lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__request[] = "request";
static char lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__srv__SetFrame_Event__FIELDS[] = {
  {
    {lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__srv__SetFrame_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__msg__LbotFrame__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__srv__SetFrame_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__srv__SetFrame_Event__TYPE_NAME, 38, 38},
      {lbot_arm_interfaces__srv__SetFrame_Event__FIELDS, 3, 3},
    },
    {lbot_arm_interfaces__srv__SetFrame_Event__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lbot_arm_interfaces__msg__LbotFrame__EXPECTED_HASH, lbot_arm_interfaces__msg__LbotFrame__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = lbot_arm_interfaces__msg__LbotFrame__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lbot_arm_interfaces__srv__SetFrame_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lbot_arm_interfaces__srv__SetFrame_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "lbot_arm_interfaces/LbotFrame frame\n"
  "---\n"
  "bool success";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetFrame__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetFrame__TYPE_NAME, 32, 32},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 53, 53},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetFrame_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetFrame_Request__TYPE_NAME, 40, 40},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetFrame_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetFrame_Response__TYPE_NAME, 41, 41},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__srv__SetFrame_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__srv__SetFrame_Event__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetFrame__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetFrame__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__msg__LbotFrame__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__SetFrame_Event__get_individual_type_description_source(NULL);
    sources[5] = *lbot_arm_interfaces__srv__SetFrame_Request__get_individual_type_description_source(NULL);
    sources[6] = *lbot_arm_interfaces__srv__SetFrame_Response__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetFrame_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetFrame_Request__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[2] = *lbot_arm_interfaces__msg__LbotFrame__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetFrame_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetFrame_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__srv__SetFrame_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__srv__SetFrame_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[3] = *lbot_arm_interfaces__msg__LbotFrame__get_individual_type_description_source(NULL);
    sources[4] = *lbot_arm_interfaces__srv__SetFrame_Request__get_individual_type_description_source(NULL);
    sources[5] = *lbot_arm_interfaces__srv__SetFrame_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
