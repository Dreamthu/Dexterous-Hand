cmake_minimum_required(VERSION 3.8)
project(lbot_driver)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# ROS2 dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(lbot_arm_interfaces REQUIRED)

# include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)

# library directory
link_directories(${PROJECT_SOURCE_DIR}/lib)

# executable
add_executable(lbot_driver src/lbot_driver.cpp)

# link the API library (name only! no lib prefix, no .so suffix)
target_link_libraries(lbot_driver
  lbot_api_cpp
)

# ros deps
ament_target_dependencies(lbot_driver
  rclcpp
  std_msgs
  std_srvs
  sensor_msgs
  geometry_msgs
  tf2
  tf2_geometry_msgs
  lbot_arm_interfaces
)

# install binary
install(TARGETS lbot_driver
  DESTINATION lib/${PROJECT_NAME}
)

# install launch & config
install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

# install .so libraries
install(DIRECTORY lib/
  DESTINATION lib
)

ament_package()
