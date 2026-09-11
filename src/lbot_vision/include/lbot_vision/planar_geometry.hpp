#pragma once

#include <array>
#include <vector>
#include <opencv2/core.hpp>

namespace lbot_vision {

// Corners must follow the boundary, in either winding. The arbitrary starting
// corner does not affect sizes on a square reference plane.
cv::Mat square_plane_transform(const std::vector<cv::Point2f> &corners, double side_mm);

// Equivalent outer-contour diameter, sqrt(4 * rectified area / pi), in mm.
// Inner holes are intentionally excluded from this size descriptor.
double rectified_diameter_mm(const std::vector<cv::Point2f> &contour, const cv::Mat &transform);

// Split the localized basket polygon into three equal base-X intervals.
// Centers lie halfway across each interval's Y cross-section, in increasing X.
// Never call with camera-frame coordinates.
std::array<cv::Point3d, 3> basket_slots_robot_x(const std::vector<cv::Point3d> &corners);

}  // namespace lbot_vision
