cmake_minimum_required(VERSION 3.8)
project(lbot_demo)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(lbot_arm_interfaces REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}  
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(demo_lkrs_movej src/demo_lkrs_movej.cpp)
ament_target_dependencies(demo_lkrs_movej rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkrs_movel src/demo_lkrs_movel.cpp)
ament_target_dependencies(demo_lkrs_movel rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkrs_movejp src/demo_lkrs_movejp.cpp)
ament_target_dependencies(demo_lkrs_movejp rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_lkls_movej src/demo_lkls_movej.cpp)
ament_target_dependencies(demo_lkls_movej rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkls_movel src/demo_lkls_movel.cpp)
ament_target_dependencies(demo_lkls_movel rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkls_movejp src/demo_lkls_movejp.cpp)
ament_target_dependencies(demo_lkls_movejp rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_lkls_i1_movej src/demo_lkls_i1_movej.cpp)
ament_target_dependencies(demo_lkls_i1_movej rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkls_i1_movel src/demo_lkls_i1_movel.cpp)
ament_target_dependencies(demo_lkls_i1_movel rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_lkls_i1_movejp src/demo_lkls_i1_movejp.cpp)
ament_target_dependencies(demo_lkls_i1_movejp rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_ik src/demo_ik.cpp)
ament_target_dependencies(demo_ik rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_fk src/demo_fk.cpp)
ament_target_dependencies(demo_fk rclcpp sensor_msgs lbot_arm_interfaces)  

add_executable(demo_tool_frame src/demo_tool_frame.cpp)
ament_target_dependencies(demo_tool_frame rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_emergency_enable src/demo_emergency_enable.cpp)
ament_target_dependencies(demo_emergency_enable rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_hand_l6_control src/demo_hand_l6_control.cpp)
ament_target_dependencies(demo_hand_l6_control rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_hand_l10_control src/demo_hand_l10_control.cpp)
ament_target_dependencies(demo_hand_l10_control rclcpp sensor_msgs lbot_arm_interfaces)

add_executable(demo_motion_test src/demo_motion_test.cpp)
ament_target_dependencies(demo_motion_test rclcpp sensor_msgs lbot_arm_interfaces)

install(TARGETS
  demo_lkrs_movel
  demo_lkrs_movej
  demo_lkrs_movejp
  demo_lkls_movel
  demo_lkls_movej
  demo_lkls_movejp
  demo_lkls_i1_movel
  demo_lkls_i1_movej
  demo_lkls_i1_movejp
  demo_fk
  demo_ik
  demo_tool_frame
  demo_emergency_enable
  demo_hand_l6_control
  demo_hand_l10_control
  demo_motion_test
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch 
  DESTINATION share/${PROJECT_NAME})

ament_package()
