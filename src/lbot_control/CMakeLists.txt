cmake_minimum_required(VERSION 3.8)
project(lbot_control)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(lbot_motion REQUIRED)

# The task library is deliberately ROS-free. It owns task order and plan
# construction, but cannot send a robot command.
add_library(lbot_control_core SHARED
  src/task_state_machine.cpp
  src/motion_plan.cpp
  src/motion_system.cpp
  src/table_route.cpp
  src/task_coordinator.cpp)
target_include_directories(lbot_control_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# The task-level adapter converts phases to the narrow lbot_motion interface;
# it does not create driver service clients itself.
add_library(lbot_control_ros_motion_system SHARED
  src/ros_left_arm_motion_system.cpp)
target_include_directories(lbot_control_ros_motion_system
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(lbot_control_ros_motion_system lbot_control_core)
ament_target_dependencies(lbot_control_ros_motion_system rclcpp lbot_motion)

add_executable(motion_bringup_node src/motion_bringup_node.cpp)
target_link_libraries(
  motion_bringup_node lbot_control_core lbot_control_ros_motion_system)
ament_target_dependencies(motion_bringup_node rclcpp lbot_motion)

if(BUILD_TESTING)
  enable_testing()
  add_executable(task_state_machine_test tests/task_state_machine_test.cpp)
  target_link_libraries(task_state_machine_test PRIVATE lbot_control_core)
  add_test(NAME task_state_machine_test COMMAND task_state_machine_test)

  add_executable(motion_plan_test tests/motion_plan_test.cpp)
  target_link_libraries(motion_plan_test PRIVATE lbot_control_core)
  add_test(NAME motion_plan_test COMMAND motion_plan_test)

  add_executable(task_coordinator_test tests/task_coordinator_test.cpp)
  target_link_libraries(task_coordinator_test PRIVATE lbot_control_core)
  add_test(NAME task_coordinator_test COMMAND task_coordinator_test)

  add_executable(table_route_test tests/table_route_test.cpp)
  target_link_libraries(table_route_test PRIVATE lbot_control_core)
  add_test(NAME table_route_test COMMAND table_route_test)
endif()

install(
  TARGETS lbot_control_core lbot_control_ros_motion_system
  EXPORT export_lbot_control
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)
install(TARGETS motion_bringup_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
  PATTERN "__pycache__" EXCLUDE)
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../config/control/nut_task.yaml
  DESTINATION share/${PROJECT_NAME}/config)
install(FILES README.md DESTINATION share/${PROJECT_NAME})

ament_export_targets(export_lbot_control HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_dependencies(rclcpp lbot_motion)
ament_package()
