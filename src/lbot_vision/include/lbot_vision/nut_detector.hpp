#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>

namespace lbot_vision {

// Adapters must populate every field from the central YAML before use.
struct DetectorConfig {
#define LBOT_DETECTOR_FIELD(type, name) type name{};
#include "lbot_vision/detector_fields.inc"
#undef LBOT_DETECTOR_FIELD
  void validate() const;
};

struct SceneGeometry {
  std::vector<cv::Point> frame;
  std::vector<cv::Point> basket;
  // Existing long-axis thirds: estimates, NOT measured compartment centers.
  std::vector<cv::Point2f> slots;
};

struct CandidateDiagnostic {
  cv::Point2f center;
  float radius_px{0};
  std::vector<cv::Point> contour;
  std::string source;
  std::string reason;
  bool accepted{false};
};

// Diagnostics for the black source-frame search.  Entries are recorded once
// their contour passes the configured area range; `accepted` means that the
// remaining frame geometry checks passed and that this was the best-scoring
// candidate.  These values make field tuning inspectable without changing the
// nut detector or accepting a frame merely because it is the largest contour.
struct FrameCandidateDiagnostic {
  std::vector<cv::Point> contour;
  double area{0.0};
  cv::Rect bounding_rect;
  int vertex_count{0};
  double aspect_ratio{0.0};
  double fill_ratio{0.0};
  double mean_gray{0.0};
  double score{0.0};
  bool hull_stabilized{false};
  std::string reason;
  bool accepted{false};
};

struct Detection2D {
  SceneGeometry geometry;
  // At most three accepted observations; optional Hough fallback is reported explicitly.
  std::vector<cv::Vec3f> circles;
  bool hough_fallback_enabled{false};
  std::vector<CandidateDiagnostic> candidates;
  std::vector<FrameCandidateDiagnostic> frame_candidates;
  bool black_frame_debug_enabled{false};
  cv::Mat annotated, value_channel, black_mask_before_close, black_mask, blue_mask,
      frame_candidate_debug, roi_mask, adaptive_mask, blackhat_mask;
  bool frame_found{false};
  bool basket_found{false};
};

// BGR8 only. No ROS, depth, intrinsics, filesystem or task-state dependencies.
// Throws std::invalid_argument for an empty/wrong-type image or invalid config.
Detection2D detect_2d(const cv::Mat &bgr, const DetectorConfig &config);

}  // namespace lbot_vision
