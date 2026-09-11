#include "offline_io.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <opencv2/imgproc.hpp>

namespace {
void require(bool value, const std::string &message)
{
  if (!value) throw std::runtime_error(message);
}

struct Fixture {
  cv::Mat image;
  cv::Point center;
};

Fixture scene(int side, int thickness, int radius, double angle, int edge, int gap)
{
  Fixture fixture{cv::Mat(side == 80 ? 360 : 600, side == 80 ? 640 : 800,
                          CV_8UC3, cv::Scalar(240, 240, 240)), {}};
  const cv::Point top_left(60, 60), bottom_right(60 + side, 60 + side);
  cv::rectangle(fixture.image, top_left, bottom_right, cv::Scalar(0, 0, 0), thickness);
  std::vector<cv::Point> contour;
  for (int i = 0; i < 6; ++i) {
    const double a = angle + i * CV_PI / 3;
    contour.emplace_back(cvRound(radius * std::cos(a)), cvRound(radius * std::sin(a)));
  }
  const cv::Rect bounds = cv::boundingRect(contour);
  const int low = 60 + thickness / 2 + 1;
  const int high = 60 + side - thickness / 2 - 1;
  fixture.center = {60 + side / 2, 60 + side / 2};
  if (edge == 0 || edge == 4 || edge == 7) fixture.center.x = low + gap - bounds.x;
  if (edge == 1 || edge == 5 || edge == 6) fixture.center.x = high - gap - (bounds.x + bounds.width - 1);
  if (edge == 2 || edge == 4 || edge == 5) fixture.center.y = low + gap - bounds.y;
  if (edge == 3 || edge == 6 || edge == 7) fixture.center.y = high - gap - (bounds.y + bounds.height - 1);
  for (auto &p : contour) p += fixture.center;
  cv::fillConvexPoly(fixture.image, contour, cv::Scalar(65, 65, 65));
  cv::circle(fixture.image, fixture.center, radius / 3, cv::Scalar(240, 240, 240), cv::FILLED);
  return fixture;
}
}

int main(int argc, char **argv)
{
  try {
    require(argc == 2, "expected detector config");
    cv::setNumThreads(1);
    const auto config = lbot_vision::offline::load_config(argv[1]);
    int cases = 0;
    for (int side : {80, 190}) {
      for (int thickness : {4, 8}) {
        for (double angle : {0.0, CV_PI / 6}) {
          const int radius = side == 80 ? 8 : 18;
          const auto control = scene(side, thickness, radius, angle, -1, 0);
          const auto centered = lbot_vision::detect_2d(control.image, config);
          require(centered.circles.size() == 1, "centered control was not detected: side=" +
            std::to_string(side) + " line=" + std::to_string(thickness) + " angle=" + std::to_string(angle) +
            " frame=" + std::to_string(centered.frame_found));
          for (int edge = 0; edge < 8; ++edge) {
            for (int gap : {1, 2, 4}) {
              const auto fixture = scene(side, thickness, radius, angle, edge, gap);
              const auto result = lbot_vision::detect_2d(fixture.image, config);
              const std::string name = "side=" + std::to_string(side) + " line=" + std::to_string(thickness) +
                " angle=" + std::to_string(angle) + " edge=" + std::to_string(edge) + " gap=" + std::to_string(gap);
              if (!result.frame_found || result.circles.size() != 1) {
                std::cerr << "outer=" << result.geometry.frame << " inner=" << result.geometry.frame_inner << '\n';
                for (const auto &c : result.candidates)
                  std::cerr << c.source << ' ' << c.reason << " center=" << c.center << " radius=" << c.radius_px << '\n';
              }
              require(result.frame_found && result.circles.size() == 1, "near-border detection failed: " + name);
              const cv::Point2f detected(result.circles[0][0], result.circles[0][1]);
              require(cv::norm(detected - cv::Point2f(fixture.center)) < 1.5,
                      "near-border contour/center was clipped: " + name);
              require(std::abs(result.sizes_mm[0] / centered.sizes_mm[0] - 1.0) < 0.12,
                      "near-border metric size changed by more than 12%: " + name);
              ++cases;
            }
          }
        }
      }
    }
    // Objects physically crossing the black line must not become smaller,
    // apparently valid nuts after masking/morphology.
    for (int edge = 0; edge < 4; ++edge) {
      const auto fixture = scene(190, 6, 18, 0, edge, -5);
      const auto result = lbot_vision::detect_2d(fixture.image, config);
      require(result.circles.empty(), "line-crossing object accepted after clipping");
    }
    // Image-axis independence: the actual inner edges rotate with the source.
    for (int edge = 0; edge < 8; ++edge) {
      for (int gap : {2, 4, 8}) {
        const auto fixture = scene(190, 6, 18, CV_PI / 6, edge, gap);
        const auto transform = cv::getRotationMatrix2D({155, 155}, 23, 1.0);
        cv::Mat rotated;
        cv::warpAffine(fixture.image, rotated, transform, fixture.image.size(), cv::INTER_LINEAR,
                       cv::BORDER_CONSTANT, cv::Scalar(240, 240, 240));
        const auto result = lbot_vision::detect_2d(rotated, config);
        require(result.frame_found && result.circles.size() == 1,
                "rotated near-border detection failed: edge=" + std::to_string(edge) + " gap=" + std::to_string(gap));
        std::vector<cv::Point2f> expected;
        cv::transform(std::vector<cv::Point2f>{cv::Point2f(fixture.center)}, expected, transform);
        require(cv::norm(cv::Point2f(result.circles[0][0], result.circles[0][1]) - expected[0]) < 1.5,
                "rotated near-border center was clipped");
        ++cases;
      }
    }
    // Temporal reuse must cache both boundaries, never cache nut positions.
    auto fixture = scene(190, 6, 18, 0, 0, 1);
    lbot_vision::TemporalDetector temporal(config, 1000);
    const auto first = temporal.detect(fixture.image, 0);
    require(first.circles.size() == 1, "temporal border fixture missing");
    cv::Mat black;
    cv::inRange(fixture.image, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), black);
    fixture.image.setTo(cv::Scalar(240, 240, 240), black);
    const auto held = temporal.detect(fixture.image, 100);
    require(held.frame_reused && held.circles.size() == 1 &&
            held.geometry.frame_inner == first.geometry.frame_inner,
            "temporary frame loss dropped the measured inner boundary");
    fixture.image.setTo(cv::Scalar(240, 240, 240));
    const auto empty = temporal.detect(fixture.image, 200);
    require(empty.frame_reused && empty.circles.empty(), "cached border reused a stale nut");
    require(!temporal.detect(fixture.image, 1001).frame_found, "inner boundary cache never expired");
    std::cout << cases << " near-edge/corner scenes passed, including size and center checks\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
