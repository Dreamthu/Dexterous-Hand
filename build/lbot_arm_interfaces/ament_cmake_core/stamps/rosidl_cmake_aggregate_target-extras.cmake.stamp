# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target lbot_arm_interfaces::lbot_arm_interfaces
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${lbot_arm_interfaces_TARGETS}.
if(lbot_arm_interfaces_TARGETS AND NOT TARGET lbot_arm_interfaces::lbot_arm_interfaces)
  add_library(lbot_arm_interfaces::lbot_arm_interfaces INTERFACE IMPORTED)
  set_target_properties(lbot_arm_interfaces::lbot_arm_interfaces PROPERTIES
    INTERFACE_LINK_LIBRARIES "${lbot_arm_interfaces_TARGETS}")
endif()
