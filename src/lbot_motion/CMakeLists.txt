cmake_minimum_required(VERSION 3.8)
project(lbot_motion)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(lbot_arm_interfaces REQUIRED)

add_library(lbot_motion SHARED
  src/left_arm_motion_device.cpp
)
target_include_directories(lbot_motion
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
ament_target_dependencies(
  lbot_motion rclcpp sensor_msgs std_msgs geometry_msgs lbot_arm_interfaces)

install(
  TARGETS lbot_motion
  EXPORT export_lbot_motion
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)
install(FILES README.md DESTINATION share/${PROJECT_NAME})

ament_export_targets(export_lbot_motion HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_libraries(lbot_motion)
ament_export_dependencies(rclcpp sensor_msgs std_msgs geometry_msgs lbot_arm_interfaces)
ament_package()
