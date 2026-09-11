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
find_package(geometry_msgs REQUIRED)
find_package(lbot_motion REQUIRED)
find_package(lbot_vision REQUIRED)
find_package(moveit_ros_planning REQUIRED)
find_package(moveit_core REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(octomap_msgs REQUIRED)
find_package(octomap REQUIRED)
find_package(geometric_shapes REQUIRED)

# The task library is deliberately ROS-free. It owns task order and plan
# construction, but cannot send a robot command.
add_library(lbot_control_core SHARED
  src/task_state_machine.cpp
  src/motion_plan.cpp
  src/motion_system.cpp
  src/table_route.cpp
  src/task_coordinator.cpp)
target_sources(lbot_control_core PRIVATE src/joint_follow_trajectory.cpp)
target_include_directories(lbot_control_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# The task-level adapter converts phases to the narrow lbot_motion interface;
# it does not create driver service clients itself.
add_library(lbot_control_moveit SHARED src/moveit_transfer_planner.cpp src/moveit_cloud_snapshot.cpp)
target_include_directories(lbot_control_moveit PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(lbot_control_moveit lbot_control_core ${OCTOMAP_LIBRARIES})
target_include_directories(lbot_control_moveit SYSTEM PUBLIC ${OCTOMAP_INCLUDE_DIRS})
ament_target_dependencies(lbot_control_moveit rclcpp moveit_core moveit_ros_planning moveit_msgs
  sensor_msgs tf2_ros octomap_msgs geometric_shapes)

add_library(lbot_control_ros_motion_system SHARED
  src/ros_left_arm_motion_system.cpp)
target_include_directories(lbot_control_ros_motion_system
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(lbot_control_ros_motion_system lbot_control_core lbot_control_moveit)
ament_target_dependencies(lbot_control_ros_motion_system rclcpp lbot_motion)

add_library(lbot_control_ros_vision_system SHARED
  src/ros_vision_system.cpp)
target_include_directories(lbot_control_ros_vision_system
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
ament_target_dependencies(lbot_control_ros_vision_system rclcpp geometry_msgs lbot_vision)

add_executable(motion_bringup_node src/motion_bringup_node.cpp)
target_link_libraries(
  motion_bringup_node lbot_control_core lbot_control_ros_motion_system)
ament_target_dependencies(motion_bringup_node rclcpp lbot_motion)

add_executable(nut_task_controller src/nut_task_controller.cpp)
target_link_libraries(
  nut_task_controller lbot_control_core lbot_control_ros_motion_system
  lbot_control_ros_vision_system)
ament_target_dependencies(nut_task_controller rclcpp lbot_motion lbot_vision)

if(BUILD_TESTING)
  enable_testing()
  add_executable(joint_follow_trajectory_test tests/joint_follow_trajectory_test.cpp)
  target_link_libraries(joint_follow_trajectory_test PRIVATE lbot_control_core)
  add_test(NAME joint_follow_trajectory_test COMMAND joint_follow_trajectory_test)
  add_executable(joint_follow_device_test tests/joint_follow_device_test.cpp)
  ament_target_dependencies(joint_follow_device_test rclcpp lbot_motion)
  add_test(NAME joint_follow_device_test COMMAND joint_follow_device_test)
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

  add_executable(ros_vision_system_test tests/ros_vision_system_test.cpp)
  target_link_libraries(ros_vision_system_test lbot_control_ros_vision_system)
  ament_target_dependencies(ros_vision_system_test rclcpp geometry_msgs lbot_vision)
  add_test(NAME ros_vision_system_test COMMAND ros_vision_system_test)

  add_executable(horizontal_approach_test tests/horizontal_approach_test.cpp)
  target_link_libraries(horizontal_approach_test PRIVATE lbot_control_core)
  add_test(NAME horizontal_approach_test COMMAND horizontal_approach_test)

  add_executable(horizontal_motion_test tests/horizontal_motion_test.cpp)
  target_link_libraries(horizontal_motion_test lbot_control_ros_motion_system)
  ament_target_dependencies(horizontal_motion_test rclcpp lbot_motion)
  add_test(NAME horizontal_motion_test COMMAND horizontal_motion_test)

  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_test(NAME hand_collision_model_test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/hand_collision_model_test.py)
  set_tests_properties(hand_collision_model_test PROPERTIES TIMEOUT 30)
  add_test(NAME moveit_planner_test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/moveit_planner_test.py
      $<TARGET_FILE:moveit_transfer_preview>)
  set_tests_properties(moveit_planner_test PROPERTIES TIMEOUT 90)
  add_test(NAME moveit_cloud_test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/moveit_cloud_test.py
      $<TARGET_FILE:moveit_transfer_preview>)
  set_tests_properties(moveit_cloud_test PROPERTIES TIMEOUT 75
    ENVIRONMENT "ROS_DOMAIN_ID=185;RMW_IMPLEMENTATION=rmw_cyclonedds_cpp")
  add_test(NAME moveit_follow_pipeline_test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/moveit_follow_pipeline_test.py
      $<TARGET_FILE:nut_task_controller>)
  set_tests_properties(moveit_follow_pipeline_test PROPERTIES TIMEOUT 150
    ENVIRONMENT "ROS_DOMAIN_ID=183;RMW_IMPLEMENTATION=rmw_cyclonedds_cpp")
  add_test(NAME pregrasp_pipeline_test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/pregrasp_pipeline_test.py
      $<TARGET_FILE:nut_task_controller>)
  set_tests_properties(pregrasp_pipeline_test PROPERTIES TIMEOUT 120)
endif()

install(
  TARGETS lbot_control_core lbot_control_ros_motion_system lbot_control_ros_vision_system lbot_control_moveit
  EXPORT export_lbot_control
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)
install(TARGETS motion_bringup_node DESTINATION lib/${PROJECT_NAME})
install(TARGETS nut_task_controller DESTINATION lib/${PROJECT_NAME})
add_executable(moveit_transfer_preview src/moveit_transfer_preview.cpp)
target_link_libraries(moveit_transfer_preview lbot_control_moveit)
ament_target_dependencies(moveit_transfer_preview rclcpp)
install(TARGETS moveit_transfer_preview DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY python/ DESTINATION share/${PROJECT_NAME}/python PATTERN "__pycache__" EXCLUDE)
install(DIRECTORY include/ DESTINATION include)
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
  PATTERN "__pycache__" EXCLUDE)
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../config/control/nut_task.yaml
  DESTINATION share/${PROJECT_NAME}/config)
install(FILES README.md DESTINATION share/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../config/control/left_hand_collision_envelope.json
  DESTINATION share/${PROJECT_NAME}/config)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../config/control/left_hand_collision_meshes
  DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_lbot_control HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_dependencies(rclcpp lbot_motion lbot_vision moveit_core moveit_ros_planning moveit_msgs
  sensor_msgs tf2_ros octomap_msgs octomap geometric_shapes)
ament_package()
