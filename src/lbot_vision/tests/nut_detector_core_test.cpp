#include "offline_io.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <opencv2/imgproc.hpp>

namespace {
void require(bool value, const char *message)
{
  if (!value) throw std::runtime_error(message);
}
void invalid(const std::function<void()> &action)
{
  try { action(); } catch (const std::invalid_argument &) { return; }
  throw std::runtime_error("Expected invalid_argument");
}
cv::Mat scene(bool basket, int count)
{
  cv::Mat image(600, 800, CV_8UC3, cv::Scalar(240, 240, 240));
  // 200x180 is 7.5% of this 800x600 scene, within the calibrated 1--10%
  // source-frame range used by the 640x360 camera profile.
  cv::rectangle(image, cv::Rect(30, 40, 200, 180), cv::Scalar(0, 0, 0), 6);
  if (basket) cv::rectangle(image, cv::Rect(490, 120, 110, 300), cv::Scalar(255, 0, 0), cv::FILLED);
  const cv::Point centers[] = {{85, 95}, {175, 105}, {125, 165}, {190, 160}};
  // Keep synthetic targets within the calibrated 5--24px contour radius
  // range of the fixed 640x360 competition camera view.
  const int radii[] = {20, 16, 12, 10};
  for (int i = 0; i < count; ++i) {
    std::vector<cv::Point> polygon;
    for (int j = 0; j < 6; ++j) {
      const double angle = j * CV_PI / 3;
      polygon.emplace_back(centers[i].x + cvRound(radii[i] * std::cos(angle)),
                           centers[i].y + cvRound(radii[i] * std::sin(angle)));
    }
    cv::fillConvexPoly(image, polygon, cv::Scalar(65, 65, 65));
    cv::circle(image, centers[i], radii[i] / 3, cv::Scalar(240, 240, 240), cv::FILLED);
  }
  return image;
}
}  // namespace

int main(int argc, char **argv)
{
  try {
    require(argc == 2, "Expected generated detector config path");
    const auto config = lbot_vision::offline::load_config(argv[1]);
    invalid([&] { lbot_vision::detect_2d(cv::Mat(), config); });
    invalid([&] { lbot_vision::detect_2d(cv::Mat::zeros(10, 10, CV_8UC1), config); });
    auto bad = config;
    bad.adaptive_block_size = 4; invalid([&] { bad.validate(); });
    bad = config; bad.blue_h_min = bad.blue_h_max + 1; invalid([&] { bad.validate(); });
    bad = config; bad.min_nut_area_px = std::numeric_limits<double>::quiet_NaN();
    invalid([&] { bad.validate(); });
    bad = config; bad.frame_inner_scale = 1.2; invalid([&] { bad.validate(); });
    bad = config; bad.basket_side = "invalid"; invalid([&] { bad.validate(); });
    const cv::Mat blank(600, 800, CV_8UC3, cv::Scalar(240, 240, 240));
    const auto missing = lbot_vision::detect_2d(blank, config);
    require(!missing.frame_found && missing.circles.empty(), "Missing frame is not an observed empty frame");
    const auto image = scene(true, 3);
    const auto original = image.clone();
    const auto observed = lbot_vision::detect_2d(image, config);
    require(cv::norm(image, original, cv::NORM_INF) == 0, "Detector modified input pixels");
    require(observed.frame_found && observed.basket_found, "Synthetic scene geometry not found");
    require(observed.circles.size() == 3, "Expected three synthetic nuts");
    require(!observed.roi_mask.empty() && !observed.blackhat_mask.empty() &&
            !observed.adaptive_mask.empty(), "Missing diagnostics");
    size_t accepted = 0, rejected = 0;
    for (const auto &c : observed.candidates) {
      require(!c.source.empty() && !c.reason.empty(), "Candidate missing source/reason");
      if (c.accepted) ++accepted; else ++rejected;
    }
    require(accepted == observed.circles.size() && rejected > 0, "Incomplete candidate accounting");
    auto frame_debug_config = config;
    frame_debug_config.debug_black_frame = true;
    const auto frame_debug = lbot_vision::detect_2d(image, frame_debug_config);
    require(!frame_debug.value_channel.empty() && !frame_debug.black_mask_before_close.empty() &&
            !frame_debug.black_mask.empty() && !frame_debug.frame_candidate_debug.empty(),
            "Black-frame debug intermediates are missing");
    require(std::any_of(frame_debug.frame_candidates.begin(), frame_debug.frame_candidates.end(),
                        [](const auto &candidate) { return candidate.accepted; }),
            "Black-frame debug did not retain the selected candidate metrics");
    const auto without_basket = lbot_vision::detect_2d(scene(false, 3), config);
    require(without_basket.frame_found && !without_basket.basket_found && without_basket.circles.size() == 3,
            "Basket must not gate nut detection");
    require(!config.enable_hough_fallback, "Hough fallback must be disabled by default");
    const auto empty = lbot_vision::detect_2d(scene(true, 0), config);
    require(empty.frame_found && empty.circles.empty(),
            "Empty frame must report zero nuts when Hough fallback is disabled");
    const auto one = lbot_vision::detect_2d(scene(true, 1), config);
    const auto two = lbot_vision::detect_2d(scene(true, 2), config);
    require(one.circles.size() == 1 && two.circles.size() == 2,
            "Contour-only mode must honestly report one/two observations");
    auto hough_config = config;
    hough_config.enable_hough_fallback = true;
    const auto compatibility_empty = lbot_vision::detect_2d(scene(true, 0), hough_config);
    require(compatibility_empty.hough_fallback_enabled,
            "Compatibility switch must report the legacy Hough fallback");
    const auto four = lbot_vision::detect_2d(scene(true, 4), config);
    require(four.circles.size() == 3, "Legacy three-target cap changed during refactor");
    const auto repeated = lbot_vision::detect_2d(image, config);
    require(repeated.circles == observed.circles, "Independent-frame replay is not deterministic");
    std::cout << "Detector core contract tests passed\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
