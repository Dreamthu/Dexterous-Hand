#include "lbot_vision/camera_geometry.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace {

void require_near(double actual, double expected, double tolerance, const std::string &label)
{
  if (std::abs(actual - expected) > tolerance) {
    throw std::runtime_error(label + " differs from expected value");
  }
}

lbot_vision::CameraCalibration calibration_with(std::vector<double> distortion)
{
  return {640, 360,
          {500.0, 0.0, 320.0,
           0.0, 510.0, 180.0,
           0.0, 0.0, 1.0},
          std::move(distortion), "plumb_bob"};
}

void test_zero_distortion()
{
  const lbot_vision::CameraGeometry model(calibration_with({0.0, 0.0, 0.0, 0.0, 0.0}));
  const cv::Point3d point = model.back_project({370.0, 129.0}, 2.0);
  require_near(point.x, 0.2, 1e-12, "x");
  require_near(point.y, -0.2, 1e-12, "y");
  require_near(point.z, 2.0, 1e-12, "z");
  require_near(model.projected_radius_m({320.0, 180.0}, 10.0, 2.0),
               20.0 / 500.0, 1e-12, "radius");
}

void test_plumb_bob_distortion()
{
  const std::vector<double> coefficients{0.12, -0.06, 0.001, -0.002, 0.015, 0.0, 0.0, 0.0};
  const auto calibration = calibration_with(coefficients);
  const cv::Matx33d camera_matrix(500.0, 0.0, 320.0,
                                  0.0, 510.0, 180.0,
                                  0.0, 0.0, 1.0);
  std::vector<cv::Point3d> object_points{{0.31, -0.22, 1.0}};
  std::vector<cv::Point2d> image_points;
  cv::projectPoints(object_points, cv::Vec3d::zeros(), cv::Vec3d::zeros(),
                    camera_matrix, coefficients, image_points);

  const lbot_vision::CameraGeometry model(calibration);
  const cv::Point3d recovered = model.back_project(image_points.front(), 1.7);
  require_near(recovered.x, 0.31 * 1.7, 1e-7, "distortion-corrected x");
  require_near(recovered.y, -0.22 * 1.7, 1e-7, "distortion-corrected y");
}

void test_invalid_calibration_is_rejected()
{
  auto calibration = calibration_with({0.1, 0.0, 0.0});
  bool rejected = false;
  try {
    const lbot_vision::CameraGeometry model(std::move(calibration));
    (void)model;
  } catch (const std::invalid_argument &) {
    rejected = true;
  }
  if (!rejected) throw std::runtime_error("invalid distortion vector was accepted");
}

}  // namespace

int main()
{
  try {
    test_zero_distortion();
    test_plumb_bob_distortion();
    test_invalid_calibration_is_rejected();
  } catch (const std::exception &error) {
    std::cerr << "camera_geometry_test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "camera_geometry_test passed\n";
  return 0;
}
