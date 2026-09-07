#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include "cv_bridge/cv_bridge.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "lbot_vision/camera_geometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/image_encodings.hpp"
#include "sensor_msgs/msg/camera_info.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

using std::placeholders::_1;

namespace {

struct Detection {
  cv::Point2f pixel;
  float radius_px{0.0F};
  double radius_m{0.0};
  double depth_m{0.0};
  geometry_msgs::msg::Point point;
  std::string frame_id;
  std::string size_class;
};

struct SceneGeometry {
  std::vector<cv::Point> frame;
  std::vector<cv::Point> basket;
  std::vector<cv::Point2f> slots;
};

}  // namespace

class NutDetectorNode final : public rclcpp::Node {
public:
  NutDetectorNode()
  : Node("nut_detector_node"), tf_buffer_(get_clock()), tf_listener_(tf_buffer_)
  {
    color_topic_ = declare_parameter("color_topic", "/camera/color/image_raw");
    depth_topic_ = declare_parameter("depth_topic", "/camera/depth/image_raw");
    camera_info_topic_ = declare_parameter("camera_info_topic", "/camera/color/camera_info");
    target_frame_ = declare_parameter("target_frame", "base_torso_root");
    use_tf_ = declare_parameter("use_tf", true);
    detection_topic_ = declare_parameter("detection_topic", "/nut_detections");
    slot_topic_ = declare_parameter("slot_topic", "/nut_slots");
    debug_topic_ = declare_parameter("debug_image_topic", "/nut_detection/debug_image");
    publish_rate_hz_ = declare_parameter("publish_rate_hz", 5.0);

    // HSV thresholds are deliberately parameters: lighting and black/blue materials vary.
    black_v_max_ = declare_parameter("black_v_max", 90);
    blue_h_min_ = declare_parameter("blue_h_min", 90);
    blue_h_max_ = declare_parameter("blue_h_max", 140);
    blue_s_min_ = declare_parameter("blue_s_min", 70);
    blue_v_min_ = declare_parameter("blue_v_min", 35);
    min_frame_area_ratio_ = declare_parameter("min_frame_area_ratio", 0.08);
    max_frame_area_ratio_ = declare_parameter("max_frame_area_ratio", 0.30);
    min_basket_area_ratio_ = declare_parameter("min_basket_area_ratio", 0.01);
    frame_inner_scale_ = declare_parameter("frame_inner_scale", 0.94);
    min_nut_radius_px_ = declare_parameter("min_nut_radius_px", 5.0);
    max_nut_radius_px_ = declare_parameter("max_nut_radius_px", 180.0);
    min_nut_area_px_ = declare_parameter("min_nut_area_px", 350.0);
    max_nut_area_px_ = declare_parameter("max_nut_area_px", 100000.0);
    adaptive_block_size_ = declare_parameter("adaptive_block_size", 51);
    adaptive_c_ = declare_parameter("adaptive_c", 8.0);
    blackhat_kernel_size_ = declare_parameter("blackhat_kernel_size", 51);
    blackhat_threshold_ = declare_parameter("blackhat_threshold", 25.0);
    min_nut_solidity_ = declare_parameter("min_nut_solidity", 0.72);
    min_nut_circularity_ = declare_parameter("min_nut_circularity", 0.45);
    hough_param2_ = declare_parameter("hough_param2", 13.0);
    min_nut_clearance_m_ = declare_parameter("min_nut_clearance_m", 0.003);
    depth_min_m_ = declare_parameter("depth_min_m", 0.15);
    depth_max_m_ = declare_parameter("depth_max_m", 5.0);
    slot_axis_ = declare_parameter("slot_axis", "long");
    basket_side_ = declare_parameter("basket_side", "any");

    color_sub_ = create_subscription<sensor_msgs::msg::Image>(
      color_topic_, rclcpp::SensorDataQoS(),
      std::bind(&NutDetectorNode::color_callback, this, _1));
    depth_sub_ = create_subscription<sensor_msgs::msg::Image>(
      depth_topic_, rclcpp::SensorDataQoS(),
      std::bind(&NutDetectorNode::depth_callback, this, _1));
    info_sub_ = create_subscription<sensor_msgs::msg::CameraInfo>(
      camera_info_topic_, rclcpp::QoS(10),
      std::bind(&NutDetectorNode::info_callback, this, _1));

    detections_pub_ = create_publisher<geometry_msgs::msg::PoseArray>(detection_topic_, 10);
    slots_pub_ = create_publisher<geometry_msgs::msg::PoseArray>(slot_topic_, 10);
    status_pub_ = create_publisher<std_msgs::msg::String>(detection_topic_ + "/status", 10);
    debug_pub_ = create_publisher<sensor_msgs::msg::Image>(debug_topic_, 2);

    auto period = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::duration<double>(1.0 / std::max(0.5, publish_rate_hz_)));
    timer_ = create_wall_timer(period, std::bind(&NutDetectorNode::process, this));
    RCLCPP_INFO(get_logger(), "Nut detector started; no robot motion is commanded by this node");
  }

private:
  std::string color_topic_, depth_topic_, camera_info_topic_;
  std::string target_frame_, detection_topic_, slot_topic_, debug_topic_, slot_axis_, basket_side_;
  bool use_tf_{true};
  double publish_rate_hz_{5.0};
  int black_v_max_{90}, blue_h_min_{90}, blue_h_max_{140};
  int blue_s_min_{70}, blue_v_min_{35};
  double min_frame_area_ratio_{0.08}, max_frame_area_ratio_{0.30}, min_basket_area_ratio_{0.01};
  double frame_inner_scale_{0.94};
  double min_nut_radius_px_{5.0}, max_nut_radius_px_{180.0}, hough_param2_{13.0};
  double min_nut_area_px_{350.0}, max_nut_area_px_{100000.0};
  int adaptive_block_size_{51};
  double adaptive_c_{8.0};
  int blackhat_kernel_size_{51};
  double blackhat_threshold_{25.0};
  double min_nut_solidity_{0.72}, min_nut_circularity_{0.45};
  double min_nut_clearance_m_{0.003}, depth_min_m_{0.15}, depth_max_m_{5.0};

  sensor_msgs::msg::Image::ConstSharedPtr color_msg_, depth_msg_;
  std::optional<lbot_vision::CameraGeometry> camera_geometry_;
  bool camera_info_logged_{false};
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_sub_, depth_sub_;
  rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
  rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr detections_pub_;
  rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr slots_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  void color_callback(sensor_msgs::msg::Image::ConstSharedPtr msg) { color_msg_ = msg; }
  void depth_callback(sensor_msgs::msg::Image::ConstSharedPtr msg) { depth_msg_ = msg; }
  void info_callback(sensor_msgs::msg::CameraInfo::ConstSharedPtr msg)
  {
    lbot_vision::CameraCalibration calibration;
    calibration.width = msg->width;
    calibration.height = msg->height;
    std::copy(msg->k.begin(), msg->k.end(), calibration.intrinsic_matrix.begin());
    calibration.distortion.assign(msg->d.begin(), msg->d.end());
    calibration.distortion_model = msg->distortion_model;
    try {
      camera_geometry_.emplace(std::move(calibration));
      if (!camera_info_logged_) {
        RCLCPP_INFO(get_logger(),
                    "Using driver CameraInfo (%ux%u, distortion=%s, D=%zu)",
                    camera_geometry_->width(), camera_geometry_->height(),
                    camera_geometry_->distortion_model().c_str(),
                    camera_geometry_->distortion_size());
        camera_info_logged_ = true;
      }
    } catch (const std::exception &error) {
      camera_geometry_.reset();
      RCLCPP_ERROR_THROTTLE(get_logger(), *get_clock(), 3000,
                            "Rejected invalid driver CameraInfo: %s", error.what());
    }
  }

  static bool valid_depth(double z, double min_z, double max_z)
  {
    return std::isfinite(z) && z >= min_z && z <= max_z;
  }

  double sample_depth(const cv::Mat &depth, const cv::Point2f &pixel) const
  {
    if (depth.empty()) return std::numeric_limits<double>::quiet_NaN();
    int u = static_cast<int>(std::lround(pixel.x));
    int v = static_cast<int>(std::lround(pixel.y));
    if (u < 0 || v < 0 || u >= depth.cols || v >= depth.rows) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    std::vector<double> values;
    const int radius = 3;
    for (int y = std::max(0, v - radius); y <= std::min(depth.rows - 1, v + radius); ++y) {
      for (int x = std::max(0, u - radius); x <= std::min(depth.cols - 1, u + radius); ++x) {
        double z = 0.0;
        if (depth.type() == CV_16UC1) {
          z = static_cast<double>(depth.at<uint16_t>(y, x)) * 0.001;
        } else if (depth.type() == CV_32FC1) {
          z = static_cast<double>(depth.at<float>(y, x));
        } else if (depth.type() == CV_64FC1) {
          z = depth.at<double>(y, x);
        }
        if (valid_depth(z, depth_min_m_, depth_max_m_)) values.push_back(z);
      }
    }
    if (values.empty()) return std::numeric_limits<double>::quiet_NaN();
    std::nth_element(values.begin(), values.begin() + values.size() / 2, values.end());
    return values[values.size() / 2];
  }

  geometry_msgs::msg::Point project_pixel(const cv::Point2f &pixel, double z) const
  {
    const cv::Point3d camera_point = camera_geometry_->back_project(pixel, z);
    geometry_msgs::msg::Point p;
    p.x = camera_point.x;
    p.y = camera_point.y;
    p.z = camera_point.z;
    return p;
  }

  bool transform_point(const geometry_msgs::msg::Point &input, const std::string &source,
                       geometry_msgs::msg::Point &output, std::string &frame)
  {
    frame = source;
    output = input;
    if (!use_tf_ || target_frame_.empty() || source == target_frame_) {
      return true;
    }
    try {
      auto tf = tf_buffer_.lookupTransform(target_frame_, source, tf2::TimePointZero,
                                           tf2::durationFromSec(0.05));
      geometry_msgs::msg::PointStamped in, out;
      in.header.frame_id = source;
      in.header.stamp = now();
      in.point = input;
      tf2::doTransform(in, out, tf);
      output = out.point;
      frame = target_frame_;
      return true;
    } catch (const tf2::TransformException &e) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000,
                           "No TF %s <- %s; publishing camera-frame coordinates: %s",
                           target_frame_.c_str(), source.c_str(), e.what());
      return false;
    }
  }

  static std::vector<cv::Point> largest_quadrilateral(const cv::Mat &mask, const cv::Mat &gray,
                                                      double min_area, double max_area)
  {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    double best = 0.0;
    std::vector<cv::Point> result;
    for (const auto &contour : contours) {
      const double area = cv::contourArea(contour);
      if (area < min_area || area > max_area) continue;
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, 0.03 * cv::arcLength(contour, true), true);
      if (approx.size() < 4 || approx.size() > 8 || !cv::isContourConvex(approx)) continue;
      const cv::RotatedRect rr = cv::minAreaRect(approx);
      const double rect_area = static_cast<double>(rr.size.area());
      if (rect_area <= 1e-6) continue;
      const double rectangularity = area / rect_area;
      const double short_side = std::min(rr.size.width, rr.size.height);
      const double long_side = std::max(rr.size.width, rr.size.height);
      if (short_side <= 1.0 || long_side / short_side > 2.5 || rectangularity < 0.55) continue;
      cv::Mat interior = cv::Mat::zeros(gray.size(), CV_8UC1);
      cv::fillConvexPoly(interior, approx, 255);
      cv::erode(interior, interior, cv::getStructuringElement(cv::MORPH_ELLIPSE, {11, 11}));
      const double mean_intensity = cv::mean(gray, interior)[0];
      // The line frame surrounds a bright sheet. This rejects black equipment,
      // floor areas and labels that happen to form large quadrilaterals.
      if (mean_intensity < 120.0) continue;
      const double score = area * rectangularity;
      if (score > best) {
        best = score;
        result = approx;
      }
    }
    return result;
  }

  SceneGeometry find_geometry(const cv::Mat &bgr, cv::Mat &debug)
  {
    SceneGeometry geometry;
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::Mat black_mask, blue_mask;
    cv::inRange(hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, black_v_max_), black_mask);
    cv::inRange(hsv, cv::Scalar(blue_h_min_, blue_s_min_, blue_v_min_),
                cv::Scalar(blue_h_max_, 255, 255), blue_mask);
    cv::morphologyEx(black_mask, black_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, {9, 9}));
    cv::morphologyEx(blue_mask, blue_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, {7, 7}));

    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

    const double image_area = static_cast<double>(bgr.cols * bgr.rows);
    geometry.frame = largest_quadrilateral(
      black_mask, gray, image_area * min_frame_area_ratio_, image_area * max_frame_area_ratio_);
    if (geometry.frame.empty()) {
      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(black_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
      double best_fallback = 0.0;
      for (const auto &contour : contours) {
        const double area = cv::contourArea(contour);
        if (area < image_area * min_frame_area_ratio_ ||
            area > image_area * max_frame_area_ratio_) continue;
        const cv::Rect rect = cv::boundingRect(contour);
        if (rect.width < 2 || rect.height < 2) continue;
        cv::Mat interior = cv::Mat::zeros(gray.size(), CV_8UC1);
        cv::rectangle(interior, rect, 255, cv::FILLED);
        const double mean_intensity = cv::mean(gray, interior)[0];
        if (mean_intensity < 120.0 || area <= best_fallback) continue;
        best_fallback = area;
        geometry.frame = {cv::Point(rect.x, rect.y),
                          cv::Point(rect.x + rect.width, rect.y),
                          cv::Point(rect.x + rect.width, rect.y + rect.height),
                          cv::Point(rect.x, rect.y + rect.height)};
      }
    }

    std::vector<std::vector<cv::Point>> blue_contours;
    cv::findContours(blue_mask, blue_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Point2f frame_center(bgr.cols * 0.5F, bgr.rows * 0.5F);
    if (!geometry.frame.empty()) {
      cv::Moments m = cv::moments(geometry.frame);
      if (std::abs(m.m00) > 1e-6) frame_center = {static_cast<float>(m.m10 / m.m00),
                                                   static_cast<float>(m.m01 / m.m00)};
    }
    double best_area = image_area * min_basket_area_ratio_;
    for (const auto &contour : blue_contours) {
      double area = cv::contourArea(contour);
      cv::Moments m = cv::moments(contour);
      if (area < best_area || std::abs(m.m00) < 1e-6) continue;
      cv::Point2f c(static_cast<float>(m.m10 / m.m00), static_cast<float>(m.m01 / m.m00));
      if (basket_side_ == "right" && c.x <= frame_center.x) continue;
      if (basket_side_ == "left" && c.x >= frame_center.x) continue;
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
      if (slot_axis_ == "right_to_left") std::reverse(geometry.slots.begin(), geometry.slots.end());
    }

    debug = bgr.clone();
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
      cv::Point2f q = center + static_cast<float>(frame_inner_scale_) * (cv::Point2f(p) - center);
      inner.emplace_back(cv::Point(cvRound(q.x), cvRound(q.y)));
    }
    cv::fillConvexPoly(roi, inner, 255);
    // The frame is the only dark large object; suppress its residual border.
    cv::erode(roi, roi, cv::getStructuringElement(cv::MORPH_ELLIPSE, {9, 9}));

    // Reference images show hexagonal nuts with circular holes. Detect the outer
    // silhouette first, so size classification is not driven by the inner hole.
    cv::Mat adaptive_mask;
    int block_size = std::max(3, adaptive_block_size_);
    if ((block_size % 2) == 0) ++block_size;
    // The table and paper have a strong illumination gradient in the reference
    // images. Adaptive thresholding keeps the silver nut visible without
    // turning the whole shaded paper into one connected component.
    cv::adaptiveThreshold(gray, adaptive_mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, block_size, adaptive_c_);
    cv::bitwise_and(adaptive_mask, roi, adaptive_mask);
    cv::morphologyEx(adaptive_mask, adaptive_mask, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3}));
    cv::morphologyEx(adaptive_mask, adaptive_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE, {7, 7}));

    int blackhat_size = std::max(3, blackhat_kernel_size_);
    if ((blackhat_size % 2) == 0) ++blackhat_size;
    cv::Mat blackhat, blackhat_mask;
    cv::morphologyEx(gray, blackhat, cv::MORPH_BLACKHAT,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                {blackhat_size, blackhat_size}));
    cv::threshold(blackhat, blackhat_mask, blackhat_threshold_, 255, cv::THRESH_BINARY);
    cv::bitwise_and(blackhat_mask, roi, blackhat_mask);
    cv::morphologyEx(blackhat_mask, blackhat_mask, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3}));
    cv::morphologyEx(blackhat_mask, blackhat_mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_ELLIPSE, {7, 7}));

    std::vector<cv::Vec3f> candidates;
    // Black-hat handles the large illumination gradient in the sample photos;
    // adaptive thresholding is retained for cases where the nut has weak edges.
    for (const cv::Mat &candidate_mask : {blackhat_mask, adaptive_mask}) {
      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(candidate_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
      for (const auto &contour : contours) {
        const double area = cv::contourArea(contour);
        if (area < min_nut_area_px_ || area > max_nut_area_px_) continue;
        const double perimeter = cv::arcLength(contour, true);
        if (perimeter <= 1e-6) continue;
        const double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);
        if (circularity < min_nut_circularity_) continue;
        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        const double hull_area = cv::contourArea(hull);
        if (hull_area <= 1e-6 || area / hull_area < min_nut_solidity_) continue;
        const cv::RotatedRect rr = cv::minAreaRect(contour);
        const float width = std::max(rr.size.width, rr.size.height);
        const float height = std::min(rr.size.width, rr.size.height);
        if (width <= 1.0F || height / width < 0.50F) continue;
        cv::Moments moments = cv::moments(contour);
        if (std::abs(moments.m00) <= 1e-6) continue;
        const cv::Point2f center(static_cast<float>(moments.m10 / moments.m00),
                                 static_cast<float>(moments.m01 / moments.m00));
        // Check every outer contour point, not just its center, against the frame.
        double frame_clearance = std::numeric_limits<double>::infinity();
        for (const auto &point : contour) {
          frame_clearance = std::min(frame_clearance,
                                     cv::pointPolygonTest(geometry.frame, point, true));
        }
        if (frame_clearance < 3.0) continue;
        const float radius = 0.5F * width;
        candidates.emplace_back(center.x, center.y, radius);
        cv::polylines(debug, contour, true, cv::Scalar(0, 200, 0), 2);
      }
    }

    std::sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
      return a[2] > b[2];
    });
    std::vector<cv::Vec3f> accepted;
    auto append_if_new = [&](const cv::Vec3f &c) {
      cv::Point2f p(c[0], c[1]);
      if (p.x < 0 || p.y < 0 || p.x >= roi.cols || p.y >= roi.rows ||
          !roi.at<uint8_t>(cvRound(p.y), cvRound(p.x))) return;
      if (cv::pointPolygonTest(geometry.frame, p, true) < c[2] + 3.0) return;
      bool duplicate = false;
      for (const auto &a : accepted) {
        if (cv::norm(p - cv::Point2f(a[0], a[1])) < 0.55 * (c[2] + a[2])) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate && accepted.size() < 3) accepted.push_back(c);
    };

    for (const auto &candidate : candidates) append_if_new(candidate);

    // Metallic glare or dark shadows can break the silhouette mask. Keep the
    // original circle detector as a fallback for any missing targets.
    if (accepted.size() < 3) {
      std::vector<cv::Vec3f> circles;
      cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.2,
                       std::max(10.0, min_nut_radius_px_ * 1.4), 90.0, hough_param2_,
                       cvRound(min_nut_radius_px_), cvRound(max_nut_radius_px_));
      std::sort(circles.begin(), circles.end(), [](const auto &a, const auto &b) {
        return a[2] > b[2];
      });
      for (const auto &circle : circles) append_if_new(circle);
    }
    for (const auto &c : accepted) {
      cv::circle(debug, {cvRound(c[0]), cvRound(c[1])}, cvRound(c[2]), cv::Scalar(0, 255, 0), 2);
    }
    return accepted;
  }

  void publish_status(const std::string &status, size_t count)
  {
    std_msgs::msg::String msg;
    std::ostringstream out;
    out << "{\"status\":\"" << status << "\",\"count\":" << count << "}";
    msg.data = out.str();
    status_pub_->publish(msg);
  }

  void process()
  {
    if (!color_msg_ || !depth_msg_ || !camera_geometry_) return;
    cv::Mat color, depth;
    try {
      color = cv_bridge::toCvCopy(color_msg_, sensor_msgs::image_encodings::BGR8)->image;
      if (depth_msg_->encoding == sensor_msgs::image_encodings::TYPE_16UC1) {
        depth = cv_bridge::toCvCopy(depth_msg_, sensor_msgs::image_encodings::TYPE_16UC1)->image;
      } else {
        depth = cv_bridge::toCvCopy(depth_msg_, sensor_msgs::image_encodings::TYPE_32FC1)->image;
      }
    } catch (const std::exception &e) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000, "Image conversion failed: %s", e.what());
      return;
    }
    if (camera_geometry_->width() != static_cast<std::uint32_t>(color.cols) ||
        camera_geometry_->height() != static_cast<std::uint32_t>(color.rows)) {
      RCLCPP_ERROR_THROTTLE(
        get_logger(), *get_clock(), 3000,
        "CameraInfo size %ux%u does not match raw color image %dx%d; refusing invalid projection",
        camera_geometry_->width(), camera_geometry_->height(), color.cols, color.rows);
      publish_status("camera_info_size_mismatch", 0);
      return;
    }
    cv::Mat debug;
    SceneGeometry geometry = find_geometry(color, debug);
    if (geometry.frame.size() < 4) {
      publish_status("frame_not_found", 0);
      return publish_debug(debug, color_msg_->header);
    }
    if (geometry.basket.size() < 4 || geometry.slots.size() != 3) {
      publish_status("basket_not_found", 0);
      return publish_debug(debug, color_msg_->header);
    }

    const double sx = static_cast<double>(depth.cols) / color.cols;
    const double sy = static_cast<double>(depth.rows) / color.rows;
    std::vector<Detection> detections;
    for (const auto &circle : find_nut_circles(color, geometry, debug)) {
      cv::Point2f px(circle[0], circle[1]);
      double z = sample_depth(depth, {px.x * static_cast<float>(sx), px.y * static_cast<float>(sy)});
      if (!valid_depth(z, depth_min_m_, depth_max_m_)) continue;
      auto camera_point = project_pixel(px, z);
      Detection d;
      d.pixel = px;
      d.radius_px = circle[2];
      d.radius_m = camera_geometry_->projected_radius_m(px, circle[2], z);
      d.depth_m = z;
      if (!transform_point(camera_point, color_msg_->header.frame_id, d.point, d.frame_id)) {
        // Missing camera-to-robot extrinsics are not fatal; publish camera coordinates.
        d.frame_id = color_msg_->header.frame_id;
      }
      detections.push_back(d);
    }
    if (detections.size() != 3) {
      publish_status("need_exactly_three_nuts", detections.size());
      return publish_debug(debug, color_msg_->header);
    }
    std::sort(detections.begin(), detections.end(),
              [](const Detection &a, const Detection &b) { return a.radius_px > b.radius_px; });
    detections[0].size_class = "large";
    detections[1].size_class = "medium";
    detections[2].size_class = "small";
    bool separated = true;
    for (size_t i = 0; i < detections.size(); ++i) {
      for (size_t j = i + 1; j < detections.size(); ++j) {
        const double dx = detections[i].point.x - detections[j].point.x;
        const double dy = detections[i].point.y - detections[j].point.y;
        // Convert the detected outer silhouette radius back to a conservative
        // tabletop footprint. This rejects touching nuts even when their
        // centers are several millimetres apart.
        if (std::hypot(dx, dy) < detections[i].radius_m + detections[j].radius_m +
                                  min_nut_clearance_m_) {
          separated = false;
        }
      }
    }
    if (!separated) {
      publish_status("nuts_touching_or_too_close", detections.size());
      return publish_debug(debug, color_msg_->header);
    }

    geometry_msgs::msg::PoseArray poses;
    poses.header = color_msg_->header;
    poses.header.frame_id = detections.front().frame_id;
    geometry_msgs::msg::PoseArray slots;
    slots.header = poses.header;
    std::ostringstream json;
    json << "{\"status\":\"ok\",\"nuts\":[";
    for (size_t i = 0; i < detections.size(); ++i) {
      const auto &d = detections[i];
      geometry_msgs::msg::Pose pose;
      pose.position = d.point;
      pose.orientation.w = 1.0;
      poses.poses.push_back(pose);
      cv::putText(debug, d.size_class, d.pixel + cv::Point2f(8, 8), cv::FONT_HERSHEY_SIMPLEX,
                  0.75, cv::Scalar(0, 255, 0), 2);
      if (i) json << ',';
      json << "{\"size\":\"" << d.size_class << "\",\"u\":" << d.pixel.x
           << ",\"v\":" << d.pixel.y << ",\"x\":" << d.point.x
           << ",\"y\":" << d.point.y << ",\"z\":" << d.point.z
           << ",\"frame\":\"" << d.frame_id << "\"}";
    }
    json << "],\"slots\":[";
    for (size_t i = 0; i < geometry.slots.size(); ++i) {
      const cv::Point2f px = geometry.slots[i];
      const double z = sample_depth(depth, {px.x * static_cast<float>(sx), px.y * static_cast<float>(sy)});
      if (!valid_depth(z, depth_min_m_, depth_max_m_)) {
        json << (i ? "," : "") << "null";
        continue;
      }
      const auto camera_point = project_pixel(px, z);
      geometry_msgs::msg::Point slot_point;
      std::string slot_frame;
      transform_point(camera_point, color_msg_->header.frame_id, slot_point, slot_frame);
      geometry_msgs::msg::Pose slot_pose;
      slot_pose.position = slot_point;
      slot_pose.orientation.w = 1.0;
      slots.poses.push_back(slot_pose);
      json << (i ? "," : "") << "{\"x\":" << slot_point.x << ",\"y\":"
           << slot_point.y << ",\"z\":" << slot_point.z << "}";
    }
    json << "],\"slot_count\":3}";
    std_msgs::msg::String status;
    status.data = json.str();
    status_pub_->publish(status);
    detections_pub_->publish(poses);
    if (slots.poses.size() == 3) slots_pub_->publish(slots);
    publish_debug(debug, color_msg_->header);
  }

  void publish_debug(const cv::Mat &debug, const std_msgs::msg::Header &header)
  {
    if (debug.empty() || debug_pub_->get_subscription_count() == 0) return;
    auto msg = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, debug).toImageMsg();
    debug_pub_->publish(*msg);
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NutDetectorNode>());
  rclcpp::shutdown();
  return 0;
}
