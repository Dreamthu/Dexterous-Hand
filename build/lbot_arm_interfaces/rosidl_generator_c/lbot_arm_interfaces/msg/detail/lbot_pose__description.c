// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/msg/detail/lbot_pose__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__msg__LbotPose__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc1, 0x70, 0x14, 0xa5, 0x28, 0x29, 0x1c, 0xfa,
      0x5a, 0xeb, 0x96, 0xa3, 0xd2, 0x53, 0x4e, 0xff,
      0xeb, 0x2b, 0x18, 0x5b, 0xa6, 0x36, 0x47, 0x29,
      0xe7, 0x9c, 0x22, 0x74, 0xf8, 0xdf, 0x5d, 0x17,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/vector3__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
#endif

static char lbot_arm_interfaces__msg__LbotPose__TYPE_NAME[] = "lbot_arm_interfaces/msg/LbotPose";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";

// Define type names, field names, and default values
static char lbot_arm_interfaces__msg__LbotPose__FIELD_NAME__euler[] = "euler";
static char lbot_arm_interfaces__msg__LbotPose__FIELD_NAME__position[] = "position";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__msg__LbotPose__FIELDS[] = {
  {
    {lbot_arm_interfaces__msg__LbotPose__FIELD_NAME__euler, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__msg__LbotPose__FIELD_NAME__position, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lbot_arm_interfaces__msg__LbotPose__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lbot_arm_interfaces__msg__LbotPose__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__msg__LbotPose__TYPE_NAME, 32, 32},
      {lbot_arm_interfaces__msg__LbotPose__FIELDS, 2, 2},
    },
    {lbot_arm_interfaces__msg__LbotPose__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "geometry_msgs/Vector3 euler\n"
  "geometry_msgs/Vector3 position";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__msg__LbotPose__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__msg__LbotPose__TYPE_NAME, 32, 32},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 59, 59},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__msg__LbotPose__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__msg__LbotPose__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
