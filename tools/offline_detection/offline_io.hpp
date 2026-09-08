#pragma once
#include <string>
#include "lbot_vision/nut_detector.hpp"

namespace lbot_vision::offline {
// JSON is a generated transport snapshot, never a second hand-maintained config.
DetectorConfig load_config(const std::string &path);
void write_result(const Detection2D &result, const cv::Size &size, const std::string &path);
}  // namespace lbot_vision::offline
