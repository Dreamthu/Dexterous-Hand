#include "lbot_vision/planar_geometry.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <opencv2/imgproc.hpp>

namespace {
void require(bool condition, const char *message)
{
  if (!condition) throw std::runtime_error(message);
}
template<class F> void invalid(F action)
{
  try { action(); } catch (const std::invalid_argument &) { return; }
  throw std::runtime_error("expected invalid geometry rejection");
}
std::vector<cv::Point2f> hexagon(cv::Point2f center, double radius)
{
  std::vector<cv::Point2f> points;
  for (int i = 0; i < 6; ++i) {
    const double angle = i * CV_PI / 3.0;
    points.emplace_back(center.x + radius * std::cos(angle), center.y + radius * std::sin(angle));
  }
  return points;
}
}

int main()
{
  try {
    const std::vector<cv::Point2f> plane{{0, 0}, {190, 0}, {190, 190}, {0, 190}};
    const std::vector<cv::Point2f> image{{230, 50}, {300, 50}, {400, 350}, {100, 350}};
    const auto to_image = cv::getPerspectiveTransform(plane, image);
    const auto to_plane = lbot_vision::square_plane_transform(image, 190.0);
    const auto large = hexagon({80, 35}, 15);
    const auto small = hexagon({110, 165}, 10);
    std::vector<cv::Point2f> large_px, small_px;
    cv::perspectiveTransform(large, large_px, to_image);
    cv::perspectiveTransform(small, small_px, to_image);
    require(cv::contourArea(large_px) < cv::contourArea(small_px),
            "test must actually reverse apparent size by perspective");
    const auto large_mm = lbot_vision::rectified_diameter_mm(large_px, to_plane);
    const auto small_mm = lbot_vision::rectified_diameter_mm(small_px, to_plane);
    require(std::abs(large_mm / small_mm - 1.5) < 1e-4, "perspective size ratio is incorrect");
    require(std::abs(large_mm - std::sqrt(4.0 * cv::contourArea(large) / CV_PI)) < 1e-3,
            "190 mm reference does not yield metric dimensions");
    const auto near_large = hexagon({110, 155}, 15);
    std::vector<cv::Point2f> near_large_px;
    cv::perspectiveTransform(near_large, near_large_px, to_image);
    require(std::abs(lbot_vision::rectified_diameter_mm(near_large_px, to_plane) - large_mm) < 1e-3,
            "same nut changes rectified size with position");
    auto reversed = image;
    std::reverse(reversed.begin(), reversed.end());
    require(std::abs(lbot_vision::rectified_diameter_mm(large_px,
              lbot_vision::square_plane_transform(reversed, 190.0)) - large_mm) < 1e-3,
            "corner winding changes physical size");
    require(std::abs(lbot_vision::rectified_diameter_mm(large_px,
              lbot_vision::square_plane_transform(image, 380.0)) - 2 * large_mm) < 1e-3,
            "configured reference length is ignored");
    invalid([&] { lbot_vision::square_plane_transform({{0, 0}, {1, 0}, {2, 0}, {3, 0}}, 190); });

    // The basket is longer along Y, but must still be partitioned along X.
    const std::vector<cv::Point3d> corners{{0.2, -0.3, 0.8}, {0.38, -0.3, 0.8},
                                          {0.38, 0.3, 0.8}, {0.2, 0.3, 0.8}};
    const auto slots = lbot_vision::basket_slots_robot_x(corners);
    for (std::size_t i = 0; i < slots.size(); ++i) {
      require(std::abs(slots[i].x - (0.23 + 0.06 * i)) < 1e-9, "slots do not divide robot X");
      require(std::abs(slots[i].y) < 1e-9 && std::abs(slots[i].z - 0.8) < 1e-9,
              "slot center moved off basket centerline/plane");
    }
    auto reordered = corners;
    std::reverse(reordered.begin(), reordered.end());
    require(lbot_vision::basket_slots_robot_x(reordered) == slots, "corner order changes slot IDs");
    // A rotated footprint and sloped plane must also produce interior centers.
    std::vector<cv::Point3d> tilted;
    std::vector<cv::Point2f> footprint;
    for (const auto &p : corners) {
      const double x = p.x * std::cos(0.4) - p.y * std::sin(0.4);
      const double y = p.x * std::sin(0.4) + p.y * std::cos(0.4);
      tilted.emplace_back(x, y, 0.8 + 0.1 * x + 0.05 * y);
      footprint.emplace_back(x, y);
    }
    const auto tilted_slots = lbot_vision::basket_slots_robot_x(tilted);
    for (const auto &p : tilted_slots) {
      require(cv::pointPolygonTest(footprint, cv::Point2f(p.x, p.y), false) > 0,
              "robot-X center is outside rotated basket");
      require(std::abs(p.z - (0.8 + 0.1 * p.x + 0.05 * p.y)) < 1e-9,
              "slot is not on the localized basket plane");
    }
    auto bad = corners;
    bad[0].x = std::numeric_limits<double>::quiet_NaN();
    invalid([&] { lbot_vision::basket_slots_robot_x(bad); });
    invalid([&] { lbot_vision::basket_slots_robot_x({{0, 0, 0}, {0, 1, 0}, {0, 2, 0}, {0, 3, 0}}); });
    std::cout << "Perspective metric sizing and robot-X slot partition tests passed\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
