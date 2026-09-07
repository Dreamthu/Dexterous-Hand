# Shared target definition: both ROS and standalone builds compile this source.
set(LBOT_VISION_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
add_library(lbot_vision_detector "${LBOT_VISION_SOURCE_DIR}/src/core/nut_detector.cpp")
target_compile_features(lbot_vision_detector PUBLIC cxx_std_17)
target_include_directories(lbot_vision_detector PUBLIC
  $<BUILD_INTERFACE:${LBOT_VISION_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS})
target_link_libraries(lbot_vision_detector PUBLIC ${OpenCV_LIBRARIES})
