#include "lbot_vision/nut_detector.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <opencv2/imgproc.hpp>

namespace lbot_vision {

void DetectorConfig::validate() const
{
  auto require = [](bool valid, const char *name) {
    if (!valid) throw std::invalid_argument(std::string("Invalid detector configuration: ") + name);
  };
#define LBOT_FINITE(name) require(std::isfinite(name), #name " must be finite")
  LBOT_FINITE(min_frame_area_ratio);
  LBOT_FINITE(max_frame_area_ratio);
  LBOT_FINITE(min_basket_area_ratio);
  LBOT_FINITE(frame_inner_scale);
  LBOT_FINITE(min_nut_radius_px);
  LBOT_FINITE(max_nut_radius_px);
  LBOT_FINITE(min_nut_area_px);
  LBOT_FINITE(max_nut_area_px);
  LBOT_FINITE(adaptive_c);
  LBOT_FINITE(blackhat_threshold);
  LBOT_FINITE(min_nut_solidity);
  LBOT_FINITE(min_nut_circularity);
  LBOT_FINITE(hough_param2);
#undef LBOT_FINITE
  require(black_v_max >= 0 && black_v_max <= 255, "black_v_max");
  require(blue_h_min >= 0 && blue_h_min <= blue_h_max && blue_h_max <= 180, "blue_h_min/max");
  require(blue_s_min >= 0 && blue_s_min <= 255, "blue_s_min");
  require(blue_v_min >= 0 && blue_v_min <= 255, "blue_v_min");
  require(min_frame_area_ratio > 0 && min_frame_area_ratio < max_frame_area_ratio &&
          max_frame_area_ratio <= 1, "min/max_frame_area_ratio");
  require(min_basket_area_ratio > 0 && min_basket_area_ratio <= 1, "min_basket_area_ratio");
  require(frame_inner_scale > 0 && frame_inner_scale < 1, "frame_inner_scale");
  require(min_nut_radius_px > 0 && min_nut_radius_px < max_nut_radius_px &&
          max_nut_radius_px <= 100000, "min/max_nut_radius_px");
  require(min_nut_area_px > 0 && min_nut_area_px < max_nut_area_px, "min/max_nut_area_px");
  require(adaptive_block_size >= 3 && adaptive_block_size <= 4095 && adaptive_block_size % 2 == 1,
          "adaptive_block_size must be odd, 3..4095");
  require(blackhat_kernel_size >= 3 && blackhat_kernel_size <= 4095 && blackhat_kernel_size % 2 == 1,
          "blackhat_kernel_size must be odd, 3..4095");
  require(nut_mask_close_kernel_size >= 1 && nut_mask_close_kernel_size <= 255 &&
          nut_mask_close_kernel_size % 2 == 1,
          "nut_mask_close_kernel_size must be odd, 1..255");
  require(nut_mask_open_kernel_size >= 1 && nut_mask_open_kernel_size <= 255 &&
          nut_mask_open_kernel_size % 2 == 1,
          "nut_mask_open_kernel_size must be odd, 1..255");
  require(blackhat_threshold >= 0 && blackhat_threshold <= 255, "blackhat_threshold");
  require(min_nut_solidity > 0 && min_nut_solidity <= 1, "min_nut_solidity");
  require(min_nut_circularity > 0 && min_nut_circularity <= 1, "min_nut_circularity");
  require(min_nut_aspect_ratio > 0 && min_nut_aspect_ratio <= 1, "min_nut_aspect_ratio");
  require(hough_param2 > 0, "hough_param2");
  require(slot_axis == "long" || slot_axis == "right_to_left", "slot_axis");
  require(basket_side == "any" || basket_side == "left" || basket_side == "right", "basket_side");
}

namespace {
class Detector {
public:
  Detector(const DetectorConfig &config, Detection2D &result) : config_(config), result_(result) {}
  std::vector<cv::Point> best_frame_quadrilateral(const cv::Mat &mask, const cv::Mat &gray,
                                                   double min_area, double max_area)
  {
    std::vector<std::vector<cv::Point>> contours;
    // A line frame has nested inner/outer contours.  RETR_EXTERNAL can discard
    // the useful one when a shadow or neighbouring dark object surrounds it.
    cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    double best_score = -std::numeric_limits<double>::infinity();
    std::size_t best_index = std::numeric_limits<std::size_t>::max();
    std::vector<cv::Point> result;
    for (const auto &contour : contours) {
      const double area = cv::contourArea(contour);
      if (area < min_area || area > max_area) continue;

      FrameCandidateDiagnostic diagnostic;
      diagnostic.contour = contour;
      diagnostic.area = area;
      diagnostic.bounding_rect = cv::boundingRect(contour);
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, 0.02 * cv::arcLength(contour, true), true);
      diagnostic.vertex_count = static_cast<int>(approx.size());
      if (approx.size() < 4 || approx.size() > 8 || !cv::isContourConvex(approx)) {
        // With V<=170, a genuine thin source-frame edge can acquire small
        // inward dents where an internal dark nut or a local shadow touches
        // the binary component.  Stabilize only such minor dents: a large
        // convex-hull expansion remains rejected as an unrelated dark object.
        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        const double hull_area = cv::contourArea(hull);
        constexpr double kMaxHullAreaInflation = 1.15;
        if (hull_area <= area * kMaxHullAreaInflation) {
          std::vector<cv::Point> hull_approx;
          cv::approxPolyDP(hull, hull_approx, 0.02 * cv::arcLength(hull, true), true);
          if (hull_approx.size() >= 4 && hull_approx.size() <= 8 &&
              cv::isContourConvex(hull_approx)) {
            approx = std::move(hull_approx);
            diagnostic.vertex_count = static_cast<int>(approx.size());
            diagnostic.hull_stabilized = true;
          }
        }
      }
      if (approx.size() < 4 || approx.size() > 8) {
        diagnostic.reason = "vertex_count";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }
      if (!cv::isContourConvex(approx)) {
        diagnostic.reason = "not_convex";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }
      const cv::RotatedRect rr = cv::minAreaRect(approx);
      const double rect_area = static_cast<double>(rr.size.area());
      if (rect_area <= 1e-6) {
        diagnostic.reason = "zero_rect_area";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }
      const double rectangularity = area / rect_area;
      diagnostic.fill_ratio = rectangularity;
      const double short_side = std::min(rr.size.width, rr.size.height);
      const double long_side = std::max(rr.size.width, rr.size.height);
      diagnostic.aspect_ratio = short_side > 1.0 ? long_side / short_side : 0.0;
      if (short_side <= 1.0 || diagnostic.aspect_ratio > 2.5) {
        diagnostic.reason = "aspect_ratio";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }
      if (rectangularity < 0.55) {
        diagnostic.reason = "low_fill_ratio";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }
      cv::Mat interior = cv::Mat::zeros(gray.size(), CV_8UC1);
      cv::fillConvexPoly(interior, approx, 255);
      cv::erode(interior, interior, cv::getStructuringElement(cv::MORPH_ELLIPSE, {11, 11}));
      const double mean_intensity = cv::mean(gray, interior)[0];
      diagnostic.mean_gray = mean_intensity;
      // The line frame surrounds a bright sheet. This rejects black equipment,
      // floor areas and labels that happen to form large quadrilaterals.
      if (mean_intensity < 120.0) {
        diagnostic.reason = "low_mean_gray";
        result_.frame_candidates.push_back(std::move(diagnostic));
        continue;
      }

      // Score only candidates which met every required geometry condition.
      // This deliberately favors a clear four-sided bright-inside frame over
      // a merely large dark contour such as a table shadow.
      const double normalized_area = (area - min_area) / (max_area - min_area);
      const double area_score = std::clamp(1.0 - std::abs(normalized_area - 0.5) * 2.0, 0.0, 1.0);
      const double shape_score = 1.0 - 0.12 * std::abs(diagnostic.vertex_count - 4);
      const double aspect_score = std::clamp((2.5 - diagnostic.aspect_ratio) / 1.5, 0.0, 1.0);
      const double fill_score = std::clamp((rectangularity - 0.55) / 0.45, 0.0, 1.0);
      const double brightness_score = std::clamp((mean_intensity - 120.0) / 135.0, 0.0, 1.0);
      diagnostic.score = shape_score + aspect_score + fill_score + brightness_score + area_score;
      diagnostic.reason = diagnostic.hull_stabilized ? "eligible_hull_stabilized" : "eligible";
      result_.frame_candidates.push_back(std::move(diagnostic));
      const std::size_t index = result_.frame_candidates.size() - 1;
      if (result_.frame_candidates[index].score > best_score) {
        best_score = result_.frame_candidates[index].score;
        best_index = index;
        result = approx;
      }
    }
    if (best_index != std::numeric_limits<std::size_t>::max()) {
      result_.frame_candidates[best_index].accepted = true;
      result_.frame_candidates[best_index].reason = result_.frame_candidates[best_index].hull_stabilized
          ? "accepted_hull_stabilized" : "accepted";
    }
    return result;
  }

  SceneGeometry find_geometry(const cv::Mat &bgr, cv::Mat &debug)
  {
    SceneGeometry geometry;
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::Mat black_mask, blue_mask;
    cv::inRange(hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, config_.black_v_max), black_mask);
    cv::inRange(hsv, cv::Scalar(config_.blue_h_min, config_.blue_s_min, config_.blue_v_min),
                cv::Scalar(config_.blue_h_max, 255, 255), blue_mask);
    cv::extractChannel(hsv, result_.value_channel, 2);
    result_.black_mask_before_close = black_mask.clone();
    cv::morphologyEx(black_mask, black_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, {5, 5}), cv::Point(-1, -1), 1);
    cv::morphologyEx(blue_mask, blue_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, {7, 7}));

    result_.black_mask = black_mask.clone();
    result_.blue_mask = blue_mask.clone();
    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

    const double image_area = static_cast<double>(bgr.cols * bgr.rows);
    geometry.frame = best_frame_quadrilateral(
      black_mask, gray, image_area * config_.min_frame_area_ratio, image_area * config_.max_frame_area_ratio);
    std::vector<std::vector<cv::Point>> blue_contours;
    cv::findContours(blue_mask, blue_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Point2f frame_center(bgr.cols * 0.5F, bgr.rows * 0.5F);
    if (!geometry.frame.empty()) {
      cv::Moments m = cv::moments(geometry.frame);
      if (std::abs(m.m00) > 1e-6) frame_center = {static_cast<float>(m.m10 / m.m00),
                                                   static_cast<float>(m.m01 / m.m00)};
    }
    double best_area = image_area * config_.min_basket_area_ratio;
    for (const auto &contour : blue_contours) {
      double area = cv::contourArea(contour);
      cv::Moments m = cv::moments(contour);
      if (area < best_area || std::abs(m.m00) < 1e-6) continue;
      cv::Point2f c(static_cast<float>(m.m10 / m.m00), static_cast<float>(m.m01 / m.m00));
      if (config_.basket_side == "right" && c.x <= frame_center.x) continue;
      if (config_.basket_side == "left" && c.x >= frame_center.x) continue;
      best_area = area;
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, 0.03 * cv::arcLength(contour, true), true);
      geometry.basket = approx.size() >= 4 ? approx : contour;
    }

    if (!geometry.basket.empty()) {
      cv::RotatedRect rr = cv::minAreaRect(geometry.basket);
      float angle = rr.angle * static_cast<float>(CV_PI / 180.0);
      float long_size = rr.size.width;
      cv::Point2f axis(std::cos(angle), std::sin(angle));
      if (rr.size.height > rr.size.width) {
        long_size = rr.size.height;
        axis = cv::Point2f(-std::sin(angle), std::cos(angle));
      }
      axis *= (long_size / 3.0F);
      for (int i = 0; i < 3; ++i) {
        geometry.slots.push_back(rr.center + axis * (static_cast<float>(i) - 1.0F));
      }
      if (config_.slot_axis == "right_to_left") std::reverse(geometry.slots.begin(), geometry.slots.end());
    }

    debug = bgr.clone();
    if (config_.debug_black_frame) {
      for (const auto &candidate : result_.frame_candidates) {
        cv::polylines(debug, candidate.contour, true, cv::Scalar(0, 165, 255), 1);
        cv::putText(debug, candidate.reason + ":" + std::to_string(candidate.score),
                    candidate.bounding_rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.35,
                    cv::Scalar(0, 165, 255), 1);
      }
      result_.frame_candidate_debug = debug.clone();
    }
    if (geometry.frame.size() >= 4)
      cv::polylines(debug, geometry.frame, true, cv::Scalar(0, 0, 255), 3);
    if (geometry.basket.size() >= 4)
      cv::polylines(debug, geometry.basket, true, cv::Scalar(255, 0, 0), 3);
    for (size_t i = 0; i < geometry.slots.size(); ++i) {
      cv::drawMarker(debug, geometry.slots[i], cv::Scalar(255, 0, 255), cv::MARKER_CROSS, 20, 2);
      cv::putText(debug, "slot_" + std::to_string(i + 1), geometry.slots[i] + cv::Point2f(5, -5),
                  cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(255, 0, 255), 2);
    }
    return geometry;
  }

  std::vector<cv::Vec3f> find_nut_circles(const cv::Mat &bgr, const SceneGeometry &geometry,
                                          cv::Mat &debug)
  {
    if (geometry.frame.size() < 4) return {};
    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, {5, 5}, 1.2);
    cv::Mat roi = cv::Mat::zeros(gray.size(), CV_8UC1);
    std::vector<cv::Point> inner;
    cv::Point2f center(0, 0);
    for (const auto &p : geometry.frame) center += cv::Point2f(p);
    center *= 1.0F / static_cast<float>(geometry.frame.size());
    for (const auto &p : geometry.frame) {
      cv::Point2f q = center + static_cast<float>(config_.frame_inner_scale) * (cv::Point2f(p) - center);
      inner.emplace_back(cv::Point(cvRound(q.x), cvRound(q.y)));
    }
    cv::fillConvexPoly(roi, inner, 255);
    // The frame is the only dark large object; suppress its residual border.
    cv::erode(roi, roi, cv::getStructuringElement(cv::MORPH_ELLIPSE, {9, 9}));

    // Reference images show hexagonal nuts with circular holes. Detect the outer
    // silhouette first, so size classification is not driven by the inner hole.
    cv::Mat adaptive_mask;
    int block_size = std::max(3, config_.adaptive_block_size);
    if ((block_size % 2) == 0) ++block_size;
    // The table and paper have a strong illumination gradient in the reference
    // images. Adaptive thresholding keeps the silver nut visible without
    // turning the whole shaded paper into one connected component.
    cv::adaptiveThreshold(gray, adaptive_mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, block_size, config_.adaptive_c);
    cv::bitwise_and(adaptive_mask, roi, adaptive_mask);
    cv::morphologyEx(adaptive_mask, adaptive_mask, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {config_.nut_mask_open_kernel_size,
                                                 config_.nut_mask_open_kernel_size}));
    cv::morphologyEx(adaptive_mask, adaptive_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {config_.nut_mask_close_kernel_size,
                                                 config_.nut_mask_close_kernel_size}));

    int blackhat_size = std::max(3, config_.blackhat_kernel_size);
    if ((blackhat_size % 2) == 0) ++blackhat_size;
    cv::Mat blackhat, blackhat_mask;
    cv::morphologyEx(gray, blackhat, cv::MORPH_BLACKHAT,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {blackhat_size, blackhat_size}));
    cv::threshold(blackhat, blackhat_mask, config_.blackhat_threshold, 255, cv::THRESH_BINARY);
    cv::bitwise_and(blackhat_mask, roi, blackhat_mask);
    cv::morphologyEx(blackhat_mask, blackhat_mask, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {config_.nut_mask_open_kernel_size,
                                                 config_.nut_mask_open_kernel_size}));
    cv::morphologyEx(blackhat_mask, blackhat_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {config_.nut_mask_close_kernel_size,
                                                 config_.nut_mask_close_kernel_size}));

    result_.roi_mask = roi.clone();
    result_.adaptive_mask = adaptive_mask.clone();
    result_.blackhat_mask = blackhat_mask.clone();
    std::vector<size_t> candidates;
    int mask_index = 0;
    // Black-hat handles the large illumination gradient in the sample photos;
    // adaptive thresholding is retained for cases where the nut has weak edges.
    for (const cv::Mat &candidate_mask : {blackhat_mask, adaptive_mask}) {
      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(candidate_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
      const std::string source = mask_index++ == 0 ? "blackhat" : "adaptive";
      for (const auto &contour : contours) {
        CandidateDiagnostic candidate;
        candidate.contour = contour;
        candidate.source = source;
        cv::minEnclosingCircle(contour, candidate.center, candidate.radius_px);
        result_.candidates.push_back(candidate);
        auto &diagnostic = result_.candidates.back();
        const double area = cv::contourArea(contour);
        if (area < config_.min_nut_area_px || area > config_.max_nut_area_px) {
          diagnostic.reason = "area_out_of_range"; continue;
        }
        const double perimeter = cv::arcLength(contour, true);
        if (perimeter <= 1e-6) { diagnostic.reason = "zero_perimeter"; continue; }
        const double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);
        if (circularity < config_.min_nut_circularity) { diagnostic.reason = "low_circularity"; continue; }
        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        const double hull_area = cv::contourArea(hull);
        if (hull_area <= 1e-6 || area / hull_area < config_.min_nut_solidity) { diagnostic.reason = "low_solidity"; continue; }
        const cv::RotatedRect rr = cv::minAreaRect(contour);
        const float width = std::max(rr.size.width, rr.size.height);
        const float height = std::min(rr.size.width, rr.size.height);
        if (width <= 1.0F || height / width < config_.min_nut_aspect_ratio) {
          diagnostic.reason = "aspect_ratio"; continue;
        }
        cv::Moments moments = cv::moments(contour);
        if (std::abs(moments.m00) <= 1e-6) { diagnostic.reason = "zero_moment"; continue; }
        const cv::Point2f center(static_cast<float>(moments.m10 / moments.m00),
                                 static_cast<float>(moments.m01 / moments.m00));
        // Check every outer contour point, not just its center, against the frame.
        double frame_clearance = std::numeric_limits<double>::infinity();
        for (const auto &point : contour) {
          frame_clearance = std::min(frame_clearance,
                                     cv::pointPolygonTest(geometry.frame, point, true));
        }
        if (frame_clearance < 3.0) { diagnostic.reason = "contour_near_frame"; continue; }
        const float radius = 0.5F * width;
        if (radius < config_.min_nut_radius_px || radius > config_.max_nut_radius_px) {
          diagnostic.reason = "radius_out_of_range"; continue;
        }
        diagnostic.center = center;
        diagnostic.radius_px = radius;
        candidates.push_back(result_.candidates.size() - 1);
      }
    }

    std::sort(candidates.begin(), candidates.end(), [&](size_t a, size_t b) {
      return result_.candidates[a].radius_px > result_.candidates[b].radius_px;
    });
    std::vector<cv::Vec3f> accepted;
    auto append_if_new = [&](size_t index) {
      auto &candidate = result_.candidates[index];
      const cv::Point2f p = candidate.center;
      const float radius = candidate.radius_px;
      // Check rounded indices as well: a valid float can round outside the image.
      const int x = cvRound(p.x), y = cvRound(p.y);
      if (p.x < 0 || p.y < 0 || x < 0 || y < 0 || x >= roi.cols || y >= roi.rows ||
          !roi.at<uint8_t>(y, x)) {
        candidate.reason = "outside_roi"; return;
      }
      if (cv::pointPolygonTest(geometry.frame, p, true) < radius + 3.0) {
        candidate.reason = "circle_near_frame"; return;
      }
      for (const auto &other : accepted) {
        if (cv::norm(p - cv::Point2f(other[0], other[1])) < 0.55 * (radius + other[2])) {
          candidate.reason = "duplicate"; return;
        }
      }
      if (accepted.size() >= 3) {
        candidate.reason = "legacy_three_target_cap"; return;
      }
      candidate.accepted = true;
      candidate.reason = "accepted";
      accepted.emplace_back(p.x, p.y, radius);
    };
    for (size_t candidate : candidates) append_if_new(candidate);

    // Hough is an explicit compatibility fallback, disabled for honest 0--3 output.
    if (config_.enable_hough_fallback && accepted.size() < 3) {
      std::vector<cv::Vec3f> circles;
      cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.2,
                       std::max(10.0, config_.min_nut_radius_px * 1.4), 90.0, config_.hough_param2,
                       cvRound(config_.min_nut_radius_px), cvRound(config_.max_nut_radius_px));
      std::sort(circles.begin(), circles.end(), [](const auto &a, const auto &b) {
        return a[2] > b[2];
      });
      for (const auto &circle : circles) {
        result_.candidates.push_back({{circle[0], circle[1]}, circle[2], {}, "hough", "", false});
        append_if_new(result_.candidates.size() - 1);
      }
    }
    for (const auto &c : accepted) {
      cv::drawMarker(debug, {cvRound(c[0]), cvRound(c[1])}, cv::Scalar(0, 255, 0),
                     cv::MARKER_CROSS, 16, 2);
    }
    return accepted;
  }


private:
  const DetectorConfig &config_;
  Detection2D &result_;
};
}  // namespace

Detection2D detect_2d(const cv::Mat &bgr, const DetectorConfig &config)
{
  config.validate();
  if (bgr.empty() || bgr.type() != CV_8UC3)
    throw std::invalid_argument("detect_2d requires a non-empty BGR8 image");
  Detection2D result;
  result.hough_fallback_enabled = config.enable_hough_fallback;
  result.black_frame_debug_enabled = config.debug_black_frame;
  Detector detector(config, result);
  result.geometry = detector.find_geometry(bgr, result.annotated);
  result.frame_found = result.geometry.frame.size() >= 4;
  result.basket_found = result.geometry.basket.size() >= 4 && result.geometry.slots.size() == 3;
  // Missing basket/depth must not prevent inspection of nut recognition.
  if (result.frame_found)
    result.circles = detector.find_nut_circles(bgr, result.geometry, result.annotated);
  return result;
}
}  // namespace lbot_vision
