// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/arm_state__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `joints`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `euler`
#include "geometry_msgs/msg/detail/vector3__functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"

bool
lbot_arm_interfaces__msg__ArmState__init(lbot_arm_interfaces__msg__ArmState * msg)
{
  if (!msg) {
    return false;
  }
  // joints
  if (!rosidl_runtime_c__float__Sequence__init(&msg->joints, 0)) {
    lbot_arm_interfaces__msg__ArmState__fini(msg);
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__init(&msg->euler)) {
    lbot_arm_interfaces__msg__ArmState__fini(msg);
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    lbot_arm_interfaces__msg__ArmState__fini(msg);
    return false;
  }
  return true;
}

void
lbot_arm_interfaces__msg__ArmState__fini(lbot_arm_interfaces__msg__ArmState * msg)
{
  if (!msg) {
    return;
  }
  // joints
  rosidl_runtime_c__float__Sequence__fini(&msg->joints);
  // euler
  geometry_msgs__msg__Vector3__fini(&msg->euler);
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
}

bool
lbot_arm_interfaces__msg__ArmState__are_equal(const lbot_arm_interfaces__msg__ArmState * lhs, const lbot_arm_interfaces__msg__ArmState * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // joints
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->joints), &(rhs->joints)))
  {
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->euler), &(rhs->euler)))
  {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__msg__ArmState__copy(
  const lbot_arm_interfaces__msg__ArmState * input,
  lbot_arm_interfaces__msg__ArmState * output)
{
  if (!input || !output) {
    return false;
  }
  // joints
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->joints), &(output->joints)))
  {
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->euler), &(output->euler)))
  {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  return true;
}

lbot_arm_interfaces__msg__ArmState *
lbot_arm_interfaces__msg__ArmState__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__ArmState * msg = (lbot_arm_interfaces__msg__ArmState *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__ArmState), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__msg__ArmState));
  bool success = lbot_arm_interfaces__msg__ArmState__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__msg__ArmState__destroy(lbot_arm_interfaces__msg__ArmState * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__msg__ArmState__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__msg__ArmState__Sequence__init(lbot_arm_interfaces__msg__ArmState__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__ArmState * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__ArmState)) {
      return false;
    }
    data = (lbot_arm_interfaces__msg__ArmState *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__msg__ArmState), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__msg__ArmState__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__msg__ArmState__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
lbot_arm_interfaces__msg__ArmState__Sequence__fini(lbot_arm_interfaces__msg__ArmState__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      lbot_arm_interfaces__msg__ArmState__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

lbot_arm_interfaces__msg__ArmState__Sequence *
lbot_arm_interfaces__msg__ArmState__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__ArmState__Sequence * array = (lbot_arm_interfaces__msg__ArmState__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__ArmState__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__msg__ArmState__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__msg__ArmState__Sequence__destroy(lbot_arm_interfaces__msg__ArmState__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__msg__ArmState__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__msg__ArmState__Sequence__are_equal(const lbot_arm_interfaces__msg__ArmState__Sequence * lhs, const lbot_arm_interfaces__msg__ArmState__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__msg__ArmState__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__msg__ArmState__Sequence__copy(
  const lbot_arm_interfaces__msg__ArmState__Sequence * input,
  lbot_arm_interfaces__msg__ArmState__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__ArmState)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__msg__ArmState);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__msg__ArmState * data =
      (lbot_arm_interfaces__msg__ArmState *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__msg__ArmState__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__msg__ArmState__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__msg__ArmState__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
