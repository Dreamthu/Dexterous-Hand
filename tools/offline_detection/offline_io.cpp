#include "offline_io.hpp"
#include <stdexcept>

namespace lbot_vision::offline {
namespace {
void read_field(const cv::FileNode &node, int &value, const char *name)
{
  if (!node.isInt()) throw std::invalid_argument(std::string("Expected integer: ") + name);
  node >> value;
}
void read_field(const cv::FileNode &node, bool &value, const char *name)
{
  if (node.isInt()) {
    const int raw = static_cast<int>(node);
    if (raw == 0 || raw == 1) { value = raw != 0; return; }
  }
  if (node.isString()) {
    const std::string raw = static_cast<std::string>(node);
    if (raw == "true" || raw == "1") { value = true; return; }
    if (raw == "false" || raw == "0") { value = false; return; }
  }
  throw std::invalid_argument(std::string("Expected boolean: ") + name);
}
void read_field(const cv::FileNode &node, double &value, const char *name)
{
  if (!node.isInt() && !node.isReal())
    throw std::invalid_argument(std::string("Expected number: ") + name);
  node >> value;
}
void read_field(const cv::FileNode &node, std::string &value, const char *name)
{
  if (!node.isString()) throw std::invalid_argument(std::string("Expected string: ") + name);
  node >> value;
}
void points(cv::FileStorage &file, const std::vector<cv::Point> &contour)
{
  file << "[";
  for (const auto &p : contour) file << "[" << p.x << p.y << "]";
  file << "]";
}
}  // namespace

DetectorConfig load_config(const std::string &path)
{
  cv::FileStorage file(path, cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  if (!file.isOpened()) throw std::runtime_error("Cannot open effective config: " + path);
  DetectorConfig config;
#define LBOT_DETECTOR_FIELD(type, name) read_field(file[#name], config.name, #name);
#define LBOT_DETECTOR_FIELD_DEFAULT(type, name, fallback) \
  if (!file[#name].isNone()) read_field(file[#name], config.name, #name);
#include "lbot_vision/detector_fields.inc"
#undef LBOT_DETECTOR_FIELD
#undef LBOT_DETECTOR_FIELD_DEFAULT
  config.validate();
  return config;
}

void write_result(const Detection2D &result, const cv::Size &size, const std::string &path)
{
  cv::FileStorage file(path, cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
  if (!file.isOpened()) throw std::runtime_error("Cannot write result: " + path);
  file << "schema_version" << 2 << "width" << size.width << "height" << size.height;
  file << "frame_found" << int(result.frame_found) << "basket_found" << int(result.basket_found);
  file << "status" << (result.frame_found ? "observed_2d" : "frame_not_found");
  file << "selection_policy"
       << "cap_three_outer_contours_rectified_size";
  file << "size_metric" << "rectified_equivalent_outer_diameter_mm";
  file << "hough_fallback_enabled" << int(result.hough_fallback_enabled);
  file << "localization_valid" << 0 << "localization_reason" << "offline_2d_only";
  file << "frame_contour"; points(file, result.geometry.frame);
  file << "frame_inner_contour"; points(file, result.geometry.frame_inner);
  file << "frame_candidates" << "[";
  for (const auto &candidate : result.frame_candidates) {
    file << "{" << "area" << candidate.area
         << "bounding_rect" << "[" << candidate.bounding_rect.x << candidate.bounding_rect.y
         << candidate.bounding_rect.width << candidate.bounding_rect.height << "]"
         << "vertex_count" << candidate.vertex_count
         << "aspect_ratio" << candidate.aspect_ratio
         << "fill_ratio" << candidate.fill_ratio
         << "mean_gray" << candidate.mean_gray
         << "score" << candidate.score
         << "hull_stabilized" << int(candidate.hull_stabilized)
         << "accepted" << int(candidate.accepted)
         << "reason" << candidate.reason << "contour";
    points(file, candidate.contour);
    file << "}";
  }
  file << "]";
  file << "basket_contour"; points(file, result.geometry.basket);
  file << "slot_estimates_px" << "[";
  file << "]" << "slots_reason" << "requires_depth_and_robot_frame" << "nuts" << "[";
  for (std::size_t i = 0; i < result.circles.size(); ++i) {
    const auto &c = result.circles[i];
    file << "{" << "center_px" << "[" << c[0] << c[1] << "]" << "radius_px" << c[2]
         << "size_mm" << result.sizes_mm.at(i) << "}";
  }
  file << "]" << "candidates" << "[";
  for (const auto &c : result.candidates) {
    file << "{" << "center_px" << "[" << c.center.x << c.center.y << "]"
         << "radius_px" << c.radius_px << "size_mm" << c.size_mm << "source" << c.source
         << "accepted" << int(c.accepted) << "reason" << c.reason << "contour";
    points(file, c.contour);
    file << "}";
  }
  file << "]";
}
}  // namespace lbot_vision::offline
