cmake_minimum_required(VERSION 3.8)
project(lbot_motion)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(lbot_arm_interfaces REQUIRED)

add_library(lbot_motion SHARED
  src/motion_device.cpp
  src/motion_planner.cpp
  src/camera_motion_plan.cpp
)
target_include_directories(lbot_motion PUBLIC include)
ament_target_dependencies(lbot_motion rclcpp sensor_msgs std_msgs lbot_arm_interfaces)

add_executable(motion_demo_node src/motion_demo_node.cpp)
target_link_libraries(motion_demo_node lbot_motion)
ament_target_dependencies(motion_demo_node rclcpp sensor_msgs std_msgs lbot_arm_interfaces)

install(TARGETS lbot_motion
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS motion_demo_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(FILES README.md DESTINATION share/${PROJECT_NAME})

ament_export_include_directories(include)
ament_export_libraries(lbot_motion)
ament_export_dependencies(rclcpp sensor_msgs std_msgs lbot_arm_interfaces)
ament_package()
