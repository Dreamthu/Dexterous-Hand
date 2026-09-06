// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lbot_arm_interfaces:msg/FollowJoint.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/msg/detail/follow_joint__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `joints`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
lbot_arm_interfaces__msg__FollowJoint__init(lbot_arm_interfaces__msg__FollowJoint * msg)
{
  if (!msg) {
    return false;
  }
  // joints
  if (!rosidl_runtime_c__float__Sequence__init(&msg->joints, 0)) {
    lbot_arm_interfaces__msg__FollowJoint__fini(msg);
    return false;
  }
  // follow
  return true;
}

void
lbot_arm_interfaces__msg__FollowJoint__fini(lbot_arm_interfaces__msg__FollowJoint * msg)
{
  if (!msg) {
    return;
  }
  // joints
  rosidl_runtime_c__float__Sequence__fini(&msg->joints);
  // follow
}

bool
lbot_arm_interfaces__msg__FollowJoint__are_equal(const lbot_arm_interfaces__msg__FollowJoint * lhs, const lbot_arm_interfaces__msg__FollowJoint * rhs)
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
  // follow
  if (lhs->follow != rhs->follow) {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__msg__FollowJoint__copy(
  const lbot_arm_interfaces__msg__FollowJoint * input,
  lbot_arm_interfaces__msg__FollowJoint * output)
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
  // follow
  output->follow = input->follow;
  return true;
}

lbot_arm_interfaces__msg__FollowJoint *
lbot_arm_interfaces__msg__FollowJoint__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__FollowJoint * msg = (lbot_arm_interfaces__msg__FollowJoint *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__FollowJoint), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__msg__FollowJoint));
  bool success = lbot_arm_interfaces__msg__FollowJoint__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__msg__FollowJoint__destroy(lbot_arm_interfaces__msg__FollowJoint * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__msg__FollowJoint__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__msg__FollowJoint__Sequence__init(lbot_arm_interfaces__msg__FollowJoint__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__FollowJoint * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__FollowJoint)) {
      return false;
    }
    data = (lbot_arm_interfaces__msg__FollowJoint *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__msg__FollowJoint), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__msg__FollowJoint__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__msg__FollowJoint__fini(&data[i - 1]);
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
lbot_arm_interfaces__msg__FollowJoint__Sequence__fini(lbot_arm_interfaces__msg__FollowJoint__Sequence * array)
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
      lbot_arm_interfaces__msg__FollowJoint__fini(&array->data[i]);
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

lbot_arm_interfaces__msg__FollowJoint__Sequence *
lbot_arm_interfaces__msg__FollowJoint__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__msg__FollowJoint__Sequence * array = (lbot_arm_interfaces__msg__FollowJoint__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__msg__FollowJoint__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__msg__FollowJoint__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__msg__FollowJoint__Sequence__destroy(lbot_arm_interfaces__msg__FollowJoint__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__msg__FollowJoint__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__msg__FollowJoint__Sequence__are_equal(const lbot_arm_interfaces__msg__FollowJoint__Sequence * lhs, const lbot_arm_interfaces__msg__FollowJoint__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__msg__FollowJoint__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__msg__FollowJoint__Sequence__copy(
  const lbot_arm_interfaces__msg__FollowJoint__Sequence * input,
  lbot_arm_interfaces__msg__FollowJoint__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__msg__FollowJoint)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__msg__FollowJoint);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__msg__FollowJoint * data =
      (lbot_arm_interfaces__msg__FollowJoint *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__msg__FollowJoint__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__msg__FollowJoint__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__msg__FollowJoint__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
