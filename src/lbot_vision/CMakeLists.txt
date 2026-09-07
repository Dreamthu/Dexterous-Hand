cmake_minimum_required(VERSION 3.8)
project(lbot_vision)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(OpenCV REQUIRED)
include(detector_core.cmake)

add_library(lbot_vision_geometry src/core/camera_geometry.cpp)
target_include_directories(lbot_vision_geometry
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${OpenCV_INCLUDE_DIRS})
target_link_libraries(lbot_vision_geometry PUBLIC ${OpenCV_LIBRARIES})

add_executable(nut_detector_node src/ros/nut_detector_node.cpp)
target_include_directories(nut_detector_node PRIVATE ${OpenCV_INCLUDE_DIRS})
ament_target_dependencies(nut_detector_node
  rclcpp sensor_msgs geometry_msgs std_msgs cv_bridge tf2 tf2_ros
  tf2_geometry_msgs)
target_link_libraries(nut_detector_node lbot_vision_detector lbot_vision_geometry ${OpenCV_LIBRARIES})

if(BUILD_TESTING)
  enable_testing()
  add_executable(camera_geometry_test tests/camera_geometry_test.cpp)
  target_link_libraries(camera_geometry_test PRIVATE lbot_vision_geometry ${OpenCV_LIBRARIES})
  add_test(NAME camera_geometry_test COMMAND camera_geometry_test)
endif()

install(
  TARGETS lbot_vision_geometry lbot_vision_detector
  EXPORT export_lbot_vision_geometry
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)
install(TARGETS nut_detector_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

ament_export_targets(export_lbot_vision_geometry HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_dependencies(OpenCV)

# The old motion prototype is intentionally kept under experimental/ but is
# not built. It contains placeholder poses and must not be exposed as a
# competition runtime entry point.
ament_package()
