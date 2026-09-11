#include "lbot_vision/planar_geometry.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <opencv2/imgproc.hpp>

namespace lbot_vision {

cv::Mat square_plane_transform(const std::vector<cv::Point2f> &corners, double side_mm)
{
  if (corners.size() != 4 || !std::isfinite(side_mm) || side_mm <= 0.0 ||
      !std::all_of(corners.begin(), corners.end(), [](const auto &p) {
        return std::isfinite(p.x) && std::isfinite(p.y);
      }) || !cv::isContourConvex(corners) || std::abs(cv::contourArea(corners)) < 1e-12) {
    throw std::invalid_argument("square reference requires four ordered convex corners and a positive side");
  }
  const float side = static_cast<float>(side_mm);
  const std::vector<cv::Point2f> square{{0, 0}, {side, 0}, {side, side}, {0, side}};
  const auto transform = cv::getPerspectiveTransform(corners, square);
  if (!cv::checkRange(transform) || std::abs(cv::determinant(transform)) < 1e-15)
    throw std::invalid_argument("degenerate square reference transform");
  return transform;
}

double rectified_diameter_mm(const std::vector<cv::Point2f> &contour, const cv::Mat &transform)
{
  if (contour.size() < 3) throw std::invalid_argument("size requires an outer contour");
  std::vector<cv::Point2f> rectified;
  cv::perspectiveTransform(contour, rectified, transform);
  const double area = std::abs(cv::contourArea(rectified));
  if (!std::isfinite(area) || area <= 0.0)
    throw std::invalid_argument("invalid rectified contour area");
  return std::sqrt(4.0 * area / CV_PI);
}

std::array<cv::Point3d, 3> basket_slots_robot_x(const std::vector<cv::Point3d> &corners)
{
  if (corners.size() != 4) throw std::invalid_argument("basket requires four localized corners");
  std::vector<cv::Point2f> xy;
  for (const auto &p : corners) {
    if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z))
      throw std::invalid_argument("invalid basket corner");
    xy.emplace_back(p.x, p.y);
  }
  if (!cv::isContourConvex(xy) || std::abs(cv::contourArea(xy)) < 1e-8)
    throw std::invalid_argument("basket is degenerate in robot XY");
  const auto bounds = std::minmax_element(corners.begin(), corners.end(),
    [](const auto &a, const auto &b) { return a.x < b.x; });
  const double min_x = bounds.first->x;
  const double width = bounds.second->x - min_x;
  if (width < 1e-5) throw std::invalid_argument("basket has no robot-X extent");
  std::array<cv::Point3d, 3> slots;
  for (std::size_t slot = 0; slot < slots.size(); ++slot) {
    const double x = min_x + width * (static_cast<double>(slot) + 0.5) / 3.0;
    std::vector<cv::Point3d> intersections;
    for (std::size_t edge = 0; edge < corners.size(); ++edge) {
      const auto &a = corners[edge];
      const auto &b = corners[(edge + 1) % corners.size()];
      if (std::abs(b.x - a.x) < 1e-12) continue;
      const double t = (x - a.x) / (b.x - a.x);
      if (t >= 0.0 && t <= 1.0) intersections.push_back(a + (b - a) * t);
    }
    if (intersections.size() < 2) throw std::invalid_argument("invalid basket cross-section");
    const auto span = std::minmax_element(intersections.begin(), intersections.end(),
      [](const auto &a, const auto &b) { return a.y < b.y; });
    slots[slot] = (*span.first + *span.second) * 0.5;
    slots[slot].x = x;
  }
  return slots;
}

}  // namespace lbot_vision
