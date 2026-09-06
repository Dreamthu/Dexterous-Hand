cmake_minimum_required(VERSION 3.8)
project(lbot_vision)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
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
find_package(lbot_arm_interfaces REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(nut_task_node src/nut_task_node.cpp)
target_include_directories(nut_task_node PRIVATE ${OpenCV_INCLUDE_DIRS})
ament_target_dependencies(nut_task_node
  rclcpp sensor_msgs geometry_msgs std_msgs cv_bridge tf2 tf2_ros
  tf2_geometry_msgs lbot_arm_interfaces)
target_link_libraries(nut_task_node ${OpenCV_LIBRARIES})

add_executable(nut_detector_node src/nut_detector_node.cpp)
target_include_directories(nut_detector_node PRIVATE ${OpenCV_INCLUDE_DIRS})
ament_target_dependencies(nut_detector_node
  rclcpp sensor_msgs geometry_msgs std_msgs cv_bridge tf2 tf2_ros
  tf2_geometry_msgs lbot_arm_interfaces)
target_link_libraries(nut_detector_node ${OpenCV_LIBRARIES})

install(TARGETS nut_task_node nut_detector_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY config launch DESTINATION share/${PROJECT_NAME})
ament_package()
