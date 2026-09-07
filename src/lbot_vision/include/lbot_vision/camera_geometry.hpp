#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/types.hpp>

namespace lbot_vision {

struct CameraCalibration {
  std::uint32_t width{0};
  std::uint32_t height{0};
  std::array<double, 9> intrinsic_matrix{};
  std::vector<double> distortion;
  std::string distortion_model;
};

// Converts raw image pixels to distortion-corrected camera-frame geometry.
// The struct deliberately has no ROS dependency so every vision component can
// reuse the same K/D handling and test it without a running ROS graph.
class CameraGeometry {
public:
  explicit CameraGeometry(CameraCalibration calibration);

  cv::Point2d normalized_point(const cv::Point2d &raw_pixel) const;
  cv::Point3d back_project(const cv::Point2d &raw_pixel, double depth_m) const;
  double projected_radius_m(const cv::Point2d &raw_center, double radius_px,
                            double depth_m) const;

  std::uint32_t width() const noexcept { return calibration_.width; }
  std::uint32_t height() const noexcept { return calibration_.height; }
  const std::string &distortion_model() const noexcept {
    return calibration_.distortion_model;
  }
  std::size_t distortion_size() const noexcept { return calibration_.distortion.size(); }

private:
  CameraCalibration calibration_;
  bool has_distortion_{false};
};

}  // namespace lbot_vision
