#include <algorithm>
#include <chrono>
#include <functional>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "cv_bridge/cv_bridge.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "lbot_vision/camera_geometry.hpp"
#include "lbot_vision/nut_detector.hpp"
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

    // Mandatory values come from the same central YAML as the offline adapter.
#define LBOT_DETECTOR_FIELD(type, name) detector_config_.name = declare_parameter<type>(#name);
#include "lbot_vision/detector_fields.inc"
#undef LBOT_DETECTOR_FIELD
    detector_config_.validate();
    min_nut_clearance_m_ = declare_parameter("min_nut_clearance_m", 0.003);
    depth_min_m_ = declare_parameter("depth_min_m", 0.15);
    depth_max_m_ = declare_parameter("depth_max_m", 5.0);

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
  std::string target_frame_, detection_topic_, slot_topic_, debug_topic_;
  bool use_tf_{true};
  double publish_rate_hz_{5.0};
  lbot_vision::DetectorConfig detector_config_;
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
    if (!color_msg_) return;
    cv::Mat color;
    try {
      color = cv_bridge::toCvCopy(color_msg_, sensor_msgs::image_encodings::BGR8)->image;
    } catch (const std::exception &error) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000, "Color conversion failed: %s", error.what());
      publish_status("invalid_color_image", 0);
      return;
    }
    lbot_vision::Detection2D observation;
    try {
      observation = lbot_vision::detect_2d(color, detector_config_);
    } catch (const std::exception &error) {
      RCLCPP_ERROR_THROTTLE(get_logger(), *get_clock(), 3000, "2D detection failed: %s", error.what());
      publish_status("detection_2d_failed", 0);
      return;
    }
    cv::Mat debug = observation.annotated;
    const auto &geometry = observation.geometry;
    if (!observation.frame_found) {
      publish_status("frame_not_found", 0);
      return publish_debug(debug, color_msg_->header);
    }
    if (!observation.basket_found) {
      publish_status("basket_not_found", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    if (!depth_msg_ || !camera_geometry_) {
      publish_status("localization_inputs_missing", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    if (camera_geometry_->width() != static_cast<std::uint32_t>(color.cols) ||
        camera_geometry_->height() != static_cast<std::uint32_t>(color.rows)) {
      publish_status("camera_info_size_mismatch", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    cv::Mat depth;
    try {
      if (depth_msg_->encoding == sensor_msgs::image_encodings::TYPE_16UC1)
        depth = cv_bridge::toCvCopy(depth_msg_, sensor_msgs::image_encodings::TYPE_16UC1)->image;
      else if (depth_msg_->encoding == sensor_msgs::image_encodings::TYPE_32FC1)
        depth = cv_bridge::toCvCopy(depth_msg_, sensor_msgs::image_encodings::TYPE_32FC1)->image;
      else {
        publish_status("unsupported_depth_encoding", observation.circles.size());
        return publish_debug(debug, color_msg_->header);
      }
    } catch (const std::exception &error) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000, "Depth conversion failed: %s", error.what());
      publish_status("invalid_depth_image", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }

    const double sx = static_cast<double>(depth.cols) / color.cols;
    const double sy = static_cast<double>(depth.rows) / color.rows;
    std::vector<Detection> detections;
    for (const auto &circle : observation.circles) {
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
