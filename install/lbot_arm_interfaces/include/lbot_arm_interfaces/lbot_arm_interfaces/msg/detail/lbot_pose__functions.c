// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lbot_arm_interfaces:msg/LbotPose.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/lbot_pose__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `euler`
// Member `position`
#include "geometry_msgs/msg/detail/vector3__functions.h"

bool
lbot_arm_interfaces__msg__LbotPose__init(lbot_arm_interfaces__msg__LbotPose * msg)
{
  if (!msg) {
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__init(&msg->euler)) {
    lbot_arm_interfaces__msg__LbotPose__fini(msg);
    return false;
  }
  // position
  if (!geometry_msgs__msg__Vector3__init(&msg->position)) {
    lbot_arm_interfaces__msg__LbotPose__fini(msg);
    return false;
  }
  return true;
}

void
lbot_arm_interfaces__msg__LbotPose__fini(lbot_arm_interfaces__msg__LbotPose * msg)
{
  if (!msg) {
    return;
  }
  // euler
  geometry_msgs__msg__Vector3__fini(&msg->euler);
  // position
  geometry_msgs__msg__Vector3__fini(&msg->position);
}

bool
lbot_arm_interfaces__msg__LbotPose__are_equal(const lbot_arm_interfaces__msg__LbotPose * lhs, const lbot_arm_interfaces__msg__LbotPose * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->euler), &(rhs->euler)))
  {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->position), &(rhs->position)))
  {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__msg__LbotPose__copy(
  const lbot_arm_interfaces__msg__LbotPose * input,
  lbot_arm_interfaces__msg__LbotPose * output)
{
  if (!input || !output) {
    return false;
  }
  // euler
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->euler), &(output->euler)))
  {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->position), &(output->position)))
  {
    return false;
  }
  return true;
}

lbot_arm_interfaces__msg__LbotPose *
lbot_arm_interfaces__msg__LbotPose__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__LbotPose * msg = (lbot_arm_interfaces__msg__LbotPose *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__LbotPose), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__msg__LbotPose));
  bool success = lbot_arm_interfaces__msg__LbotPose__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__msg__LbotPose__destroy(lbot_arm_interfaces__msg__LbotPose * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__msg__LbotPose__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__msg__LbotPose__Sequence__init(lbot_arm_interfaces__msg__LbotPose__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__LbotPose * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__LbotPose)) {
      return false;
    }
    data = (lbot_arm_interfaces__msg__LbotPose *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__msg__LbotPose), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__msg__LbotPose__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__msg__LbotPose__fini(&data[i - 1]);
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
lbot_arm_interfaces__msg__LbotPose__Sequence__fini(lbot_arm_interfaces__msg__LbotPose__Sequence * array)
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
      lbot_arm_interfaces__msg__LbotPose__fini(&array->data[i]);
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

lbot_arm_interfaces__msg__LbotPose__Sequence *
lbot_arm_interfaces__msg__LbotPose__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__LbotPose__Sequence * array = (lbot_arm_interfaces__msg__LbotPose__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__LbotPose__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__msg__LbotPose__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__msg__LbotPose__Sequence__destroy(lbot_arm_interfaces__msg__LbotPose__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__msg__LbotPose__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__msg__LbotPose__Sequence__are_equal(const lbot_arm_interfaces__msg__LbotPose__Sequence * lhs, const lbot_arm_interfaces__msg__LbotPose__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__msg__LbotPose__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__msg__LbotPose__Sequence__copy(
  const lbot_arm_interfaces__msg__LbotPose__Sequence * input,
  lbot_arm_interfaces__msg__LbotPose__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__LbotPose)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__msg__LbotPose);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__msg__LbotPose * data =
      (lbot_arm_interfaces__msg__LbotPose *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__msg__LbotPose__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__msg__LbotPose__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__msg__LbotPose__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
