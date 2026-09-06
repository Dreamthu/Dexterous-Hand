// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice

#include "lbot_arm_interfaces/msg/detail/follow_joint__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lbot_arm_interfaces
const rosidl_type_hash_t *
lbot_arm_interfaces__msg__FollowJoint__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1b, 0x42, 0x08, 0x6a, 0x88, 0x17, 0x1a, 0x53,
      0xd2, 0x5c, 0x7b, 0xf0, 0x8a, 0xbb, 0xce, 0xdd,
      0xbe, 0x53, 0x47, 0xb5, 0xc8, 0xe5, 0x47, 0x23,
      0xeb, 0x52, 0x70, 0xfd, 0xe9, 0xf5, 0x00, 0x0d,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char lbot_arm_interfaces__msg__FollowJoint__TYPE_NAME[] = "lbot_arm_interfaces/msg/FollowJoint";

// Define type names, field names, and default values
static char lbot_arm_interfaces__msg__FollowJoint__FIELD_NAME__joints[] = "joints";
static char lbot_arm_interfaces__msg__FollowJoint__FIELD_NAME__follow[] = "follow";

static rosidl_runtime_c__type_description__Field lbot_arm_interfaces__msg__FollowJoint__FIELDS[] = {
  {
    {lbot_arm_interfaces__msg__FollowJoint__FIELD_NAME__joints, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lbot_arm_interfaces__msg__FollowJoint__FIELD_NAME__follow, 6, 6},
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
lbot_arm_interfaces__msg__FollowJoint__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lbot_arm_interfaces__msg__FollowJoint__TYPE_NAME, 35, 35},
      {lbot_arm_interfaces__msg__FollowJoint__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float32[] joints\n"
  "bool follow";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lbot_arm_interfaces__msg__FollowJoint__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lbot_arm_interfaces__msg__FollowJoint__TYPE_NAME, 35, 35},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 29, 29},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lbot_arm_interfaces__msg__FollowJoint__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lbot_arm_interfaces__msg__FollowJoint__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
