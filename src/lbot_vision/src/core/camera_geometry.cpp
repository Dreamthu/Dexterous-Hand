#include "lbot_vision/camera_geometry.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace lbot_vision {
namespace {

bool is_standard_model(const std::string &model)
{
  return model == "plumb_bob" || model == "rational_polynomial";
}

bool valid_standard_coefficient_count(std::size_t count)
{
  return count == 4 || count == 5 || count == 8 || count == 12 || count == 14;
}

}  // namespace

CameraGeometry::CameraGeometry(CameraCalibration calibration)
: calibration_(std::move(calibration))
{
  if (calibration_.width == 0 || calibration_.height == 0) {
    throw std::invalid_argument("camera calibration image dimensions must be non-zero");
  }
  const auto &k = calibration_.intrinsic_matrix;
  if (!std::isfinite(k[0]) || !std::isfinite(k[2]) || !std::isfinite(k[4]) ||
      !std::isfinite(k[5]) || k[0] <= 0.0 || k[4] <= 0.0) {
    throw std::invalid_argument("camera calibration contains an invalid intrinsic matrix");
  }
  if (!std::all_of(calibration_.distortion.begin(), calibration_.distortion.end(),
                   [](double value) { return std::isfinite(value); })) {
    throw std::invalid_argument("camera calibration contains a non-finite distortion value");
  }

  has_distortion_ = std::any_of(
    calibration_.distortion.begin(), calibration_.distortion.end(),
    [](double value) { return std::abs(value) > 1e-15; });
  if (!has_distortion_) return;

  if (is_standard_model(calibration_.distortion_model)) {
    if (!valid_standard_coefficient_count(calibration_.distortion.size())) {
      throw std::invalid_argument("unsupported plumb-bob distortion coefficient count");
    }
  } else if (calibration_.distortion_model == "equidistant") {
    if (calibration_.distortion.size() != 4) {
      throw std::invalid_argument("equidistant distortion requires exactly four coefficients");
    }
  } else {
    throw std::invalid_argument(
      "unsupported camera distortion model: " + calibration_.distortion_model);
  }
}

cv::Point2d CameraGeometry::normalized_point(const cv::Point2d &raw_pixel) const
{
  if (!std::isfinite(raw_pixel.x) || !std::isfinite(raw_pixel.y)) {
    throw std::invalid_argument("raw pixel coordinates must be finite");
  }
  const auto &k = calibration_.intrinsic_matrix;
  if (!has_distortion_) {
    return {(raw_pixel.x - k[2]) / k[0], (raw_pixel.y - k[5]) / k[4]};
  }

  const cv::Matx33d camera_matrix(k[0], k[1], k[2],
                                  k[3], k[4], k[5],
                                  k[6], k[7], k[8]);
  const std::vector<cv::Point2d> input{raw_pixel};
  std::vector<cv::Point2d> output;
  const cv::Mat distortion(calibration_.distortion, true);
  if (calibration_.distortion_model == "equidistant") {
    cv::fisheye::undistortPoints(input, output, camera_matrix, distortion);
  } else {
    cv::undistortPoints(input, output, camera_matrix, distortion);
  }
  if (output.size() != 1 || !std::isfinite(output[0].x) || !std::isfinite(output[0].y)) {
    throw std::runtime_error("OpenCV failed to undistort the image point");
  }
  return output.front();
}

cv::Point3d CameraGeometry::back_project(const cv::Point2d &raw_pixel, double depth_m) const
{
  if (!std::isfinite(depth_m) || depth_m <= 0.0) {
    throw std::invalid_argument("back-projection depth must be finite and positive");
  }
  const cv::Point2d normalized = normalized_point(raw_pixel);
  return {normalized.x * depth_m, normalized.y * depth_m, depth_m};
}

double CameraGeometry::projected_radius_m(const cv::Point2d &raw_center, double radius_px,
                                          double depth_m) const
{
  if (!std::isfinite(radius_px) || radius_px <= 0.0) {
    throw std::invalid_argument("projected pixel radius must be finite and positive");
  }
  const cv::Point3d center = back_project(raw_center, depth_m);
  const std::array<cv::Point2d, 4> boundary{{
    {raw_center.x - radius_px, raw_center.y},
    {raw_center.x + radius_px, raw_center.y},
    {raw_center.x, raw_center.y - radius_px},
    {raw_center.x, raw_center.y + radius_px},
  }};
  double radius_m = 0.0;
  for (const auto &pixel : boundary) {
    const cv::Point3d point = back_project(pixel, depth_m);
    radius_m = std::max(radius_m, std::hypot(point.x - center.x, point.y - center.y));
  }
  return radius_m;
}

}  // namespace lbot_vision
