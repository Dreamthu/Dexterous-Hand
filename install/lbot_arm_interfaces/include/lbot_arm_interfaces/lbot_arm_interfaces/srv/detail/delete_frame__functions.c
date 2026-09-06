// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lbot_arm_interfaces:srv/DeleteFrame.idl
// generated code does not contain a copyright notice
#include "lbot_arm_interfaces/srv/detail/delete_frame__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"

bool
lbot_arm_interfaces__srv__DeleteFrame_Request__init(lbot_arm_interfaces__srv__DeleteFrame_Request * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    lbot_arm_interfaces__srv__DeleteFrame_Request__fini(msg);
    return false;
  }
  return true;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Request__fini(lbot_arm_interfaces__srv__DeleteFrame_Request * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Request__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Request * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Request__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Request * input,
  lbot_arm_interfaces__srv__DeleteFrame_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  return true;
}

lbot_arm_interfaces__srv__DeleteFrame_Request *
lbot_arm_interfaces__srv__DeleteFrame_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Request * msg = (lbot_arm_interfaces__srv__DeleteFrame_Request *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request));
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Request__destroy(lbot_arm_interfaces__srv__DeleteFrame_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__srv__DeleteFrame_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__init(lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request)) {
      return false;
    }
    data = (lbot_arm_interfaces__srv__DeleteFrame_Request *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__srv__DeleteFrame_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__srv__DeleteFrame_Request__fini(&data[i - 1]);
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
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__fini(lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * array)
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
      lbot_arm_interfaces__srv__DeleteFrame_Request__fini(&array->data[i]);
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

lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence *
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * array = (lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__destroy(lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * input,
  lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__srv__DeleteFrame_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__srv__DeleteFrame_Request * data =
      (lbot_arm_interfaces__srv__DeleteFrame_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__srv__DeleteFrame_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__srv__DeleteFrame_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
lbot_arm_interfaces__srv__DeleteFrame_Response__init(lbot_arm_interfaces__srv__DeleteFrame_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  return true;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Response__fini(lbot_arm_interfaces__srv__DeleteFrame_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Response__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Response * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Response__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Response * input,
  lbot_arm_interfaces__srv__DeleteFrame_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  return true;
}

lbot_arm_interfaces__srv__DeleteFrame_Response *
lbot_arm_interfaces__srv__DeleteFrame_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Response * msg = (lbot_arm_interfaces__srv__DeleteFrame_Response *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response));
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Response__destroy(lbot_arm_interfaces__srv__DeleteFrame_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__srv__DeleteFrame_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__init(lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response)) {
      return false;
    }
    data = (lbot_arm_interfaces__srv__DeleteFrame_Response *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__srv__DeleteFrame_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__srv__DeleteFrame_Response__fini(&data[i - 1]);
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
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__fini(lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * array)
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
      lbot_arm_interfaces__srv__DeleteFrame_Response__fini(&array->data[i]);
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

lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence *
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * array = (lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__destroy(lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * input,
  lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__srv__DeleteFrame_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__srv__DeleteFrame_Response * data =
      (lbot_arm_interfaces__srv__DeleteFrame_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__srv__DeleteFrame_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__srv__DeleteFrame_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "lbot_arm_interfaces/srv/detail/delete_frame__functions.h"

bool
lbot_arm_interfaces__srv__DeleteFrame_Event__init(lbot_arm_interfaces__srv__DeleteFrame_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    lbot_arm_interfaces__srv__DeleteFrame_Event__fini(msg);
    return false;
  }
  // request
  if (!lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__init(&msg->request, 0)) {
    lbot_arm_interfaces__srv__DeleteFrame_Event__fini(msg);
    return false;
  }
  // response
  if (!lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__init(&msg->response, 0)) {
    lbot_arm_interfaces__srv__DeleteFrame_Event__fini(msg);
    return false;
  }
  return true;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Event__fini(lbot_arm_interfaces__srv__DeleteFrame_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__fini(&msg->request);
  // response
  lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__fini(&msg->response);
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Event__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Event * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Event__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Event * input,
  lbot_arm_interfaces__srv__DeleteFrame_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

lbot_arm_interfaces__srv__DeleteFrame_Event *
lbot_arm_interfaces__srv__DeleteFrame_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Event * msg = (lbot_arm_interfaces__srv__DeleteFrame_Event *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event));
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Event__destroy(lbot_arm_interfaces__srv__DeleteFrame_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lbot_arm_interfaces__srv__DeleteFrame_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__init(lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event)) {
      return false;
    }
    data = (lbot_arm_interfaces__srv__DeleteFrame_Event *)allocator.zero_allocate(size, sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lbot_arm_interfaces__srv__DeleteFrame_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lbot_arm_interfaces__srv__DeleteFrame_Event__fini(&data[i - 1]);
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
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__fini(lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * array)
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
      lbot_arm_interfaces__srv__DeleteFrame_Event__fini(&array->data[i]);
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

lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence *
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * array = (lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence *)allocator.allocate(sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__destroy(lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__are_equal(const lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * lhs, const lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence__copy(
  const lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * input,
  lbot_arm_interfaces__srv__DeleteFrame_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(lbot_arm_interfaces__srv__DeleteFrame_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lbot_arm_interfaces__srv__DeleteFrame_Event * data =
      (lbot_arm_interfaces__srv__DeleteFrame_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lbot_arm_interfaces__srv__DeleteFrame_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lbot_arm_interfaces__srv__DeleteFrame_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lbot_arm_interfaces__srv__DeleteFrame_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
