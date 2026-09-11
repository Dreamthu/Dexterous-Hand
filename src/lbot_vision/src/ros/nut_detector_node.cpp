#include <algorithm>
#include <chrono>
#include <functional>
#include <cmath>
#include <cstdint>
#include <memory>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opencv2/imgproc.hpp>

#include "cv_bridge/cv_bridge.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "lbot_vision/camera_geometry.hpp"
#include "lbot_vision/nut_detector.hpp"
#include "lbot_vision/nut_sequence.hpp"
#include "lbot_vision/planar_geometry.hpp"
#include "lbot_vision/msg/nut_sequence_state.hpp"
#include "lbot_vision/srv/set_nut_state.hpp"
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
  int target_id{0};
  cv::Point2f pixel;
  float radius_px{0.0F};
  double radius_m{0.0};
  double depth_m{0.0};
  double size_mm{0.0};
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
    target_frame_ = declare_parameter("target_frame", "base_link");
    use_tf_ = declare_parameter("use_tf", true);
    detection_topic_ = declare_parameter("detection_topic", "/nut_detections");
    slot_topic_ = declare_parameter("slot_topic", "/nut_slots");
    debug_topic_ = declare_parameter("debug_image_topic", "/nut_detection/debug_image");
    publish_rate_hz_ = declare_parameter("publish_rate_hz", 5.0);

    // Mandatory values come from the same central YAML as the offline adapter.
#define LBOT_DETECTOR_FIELD(type, name) detector_config_.name = declare_parameter<type>(#name);
#define LBOT_DETECTOR_FIELD_DEFAULT(type, name, fallback) \
    detector_config_.name = declare_parameter<type>(#name, fallback);
#include "lbot_vision/detector_fields.inc"
#undef LBOT_DETECTOR_FIELD
#undef LBOT_DETECTOR_FIELD_DEFAULT
    detector_config_.validate();
    detector_ = std::make_unique<lbot_vision::TemporalDetector>(
      detector_config_, declare_parameter("frame_hold_ms", 3000.0));
    lbot_vision::NutSequenceConfig sequence_config;
#define LBOT_SEQUENCE_FIELD(type, name) sequence_config.name = declare_parameter<type>(#name);
#include "lbot_vision/sequence_fields.inc"
#undef LBOT_SEQUENCE_FIELD
    sequence_ = std::make_unique<lbot_vision::NutSequence>(sequence_config);
    max_image_age_ms_ = declare_parameter<double>("max_image_age_ms");
    max_depth_delta_ms_ = declare_parameter<double>("max_depth_delta_ms");
    if (!std::isfinite(max_image_age_ms_) || max_image_age_ms_ <= 0 ||
        !std::isfinite(max_depth_delta_ms_) || max_depth_delta_ms_ <= 0)
      throw std::invalid_argument("max_image_age_ms/max_depth_delta_ms must be finite and positive");
    session_id_ = std::to_string(std::random_device{}()) + "-" + std::to_string(now().nanoseconds());
    sequence_pub_ = create_publisher<lbot_vision::msg::NutSequenceState>(
      declare_parameter<std::string>("sequence_topic"), 10);
    sequence_event_service_ = create_service<lbot_vision::srv::SetNutState>(
      declare_parameter<std::string>("sequence_event_service"),
      [this](const std::shared_ptr<lbot_vision::srv::SetNutState::Request> request,
             std::shared_ptr<lbot_vision::srv::SetNutState::Response> response) {
        if (request->session_id != session_id_) {
          response->accepted = false; response->reason = "wrong_session";
        } else if (request->action == "start" && (!color_is_fresh() ||
                   rclcpp::Time(color_msg_->header.stamp).nanoseconds() != last_processed_ns_)) {
          sequence_->invalidate("stale_color_image");
          response->accepted = false;
          response->reason = "stale_color_image";
        } else {
          response->accepted = sequence_->event(request->round_id, request->event_sequence,
              request->target_id, request->action, response->reason);
          if (response->accepted && request->action == "reset") last_processed_ns_ = -1;
        }
        response->round_id = sequence_->snapshot().round;
        clear_poses(); publish_sequence_state();
      });
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
    large_target_pub_ = create_publisher<geometry_msgs::msg::PointStamped>(
      declare_parameter("large_target_topic", std::string("/nut_detections/large")), 1);
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
  std::unique_ptr<lbot_vision::TemporalDetector> detector_;
  std::unique_ptr<lbot_vision::NutSequence> sequence_;
  std::string session_id_;
  std_msgs::msg::Header observation_header_;
  double max_image_age_ms_{0}, max_depth_delta_ms_{0};
  std::int64_t last_processed_ns_{-1};
  std::optional<geometry_msgs::msg::TransformStamped> frame_tf_;
  rclcpp::Publisher<lbot_vision::msg::NutSequenceState>::SharedPtr sequence_pub_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr large_target_pub_;
  rclcpp::Service<lbot_vision::srv::SetNutState>::SharedPtr sequence_event_service_;
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
    if (!frame_tf_) return false;
    geometry_msgs::msg::PointStamped in, out;
    in.header = color_msg_->header;
    in.point = input;
    tf2::doTransform(in, out, *frame_tf_);
    output = out.point; frame = target_frame_;
    return true;
  }

  bool color_is_fresh()
  {
    if (!color_msg_) return false;
    const double age = (now() - rclcpp::Time(color_msg_->header.stamp)).seconds() * 1000.0;
    return age >= 0 && age <= max_image_age_ms_;
  }

  void clear_poses()
  {
    geometry_msgs::msg::PoseArray empty;
    if (color_msg_) empty.header = color_msg_->header;
    detections_pub_->publish(empty); slots_pub_->publish(empty);
  }

  void publish_sequence_state(const std::vector<Detection> &positions = {})
  {
    lbot_vision::msg::NutSequenceState message;
    message.header = observation_header_;
    const auto &snapshot = sequence_->snapshot();
    message.session_id = session_id_; message.round_id = snapshot.round;
    message.initialized = snapshot.initialized;
    message.observation_valid = snapshot.observation_valid;
    message.observed_count = snapshot.observed_count;
    message.expected_count = snapshot.expected_count;
    message.current_target_id = snapshot.current_target_id;
    message.status = snapshot.status;
    for (const auto &target : snapshot.targets) {
      lbot_vision::msg::NutTarget message_target;
      message_target.id = target.id;
      message_target.size_class = target.size_class;
      message_target.task_state = target.state;
      message_target.visible = target.visible;
      message_target.observation_index = target.observation_index;
      message_target.last_u = target.last_observation[0];
      message_target.last_v = target.last_observation[1];
      message_target.last_radius_px = target.last_observation[2];
      message_target.last_seen_ms = target.last_seen_ms;
      for (const auto &d : positions) {
        if (d.target_id == target.id && target.visible && snapshot.observation_valid) {
          message_target.position_valid = true;
          message_target.position.header = message.header;
          message_target.position.header.frame_id = d.frame_id;
          message_target.position.point = d.point;
        }
      }
      message.targets.push_back(message_target);
    }
    sequence_pub_->publish(message);
  }

  void publish_status(const std::string &status, size_t count)
  {
    std_msgs::msg::String msg;
    std::ostringstream out;
    const auto &snapshot = sequence_->snapshot();
    out << "{\"status\":\"" << status << "\",\"count\":" << count
        << ",\"expected_count\":" << snapshot.expected_count
        << ",\"current_target_id\":" << snapshot.current_target_id
        << ",\"round_id\":" << snapshot.round << "}";
    msg.data = out.str();
    status_pub_->publish(msg);
  }

  void process()
  {
    if (!color_msg_) return;
    if (!color_is_fresh()) {
      sequence_->invalidate("stale_color_image", false);
      clear_poses(); publish_sequence_state(); return;
    }
    const auto stamp_ns = rclcpp::Time(color_msg_->header.stamp).nanoseconds();
    if (stamp_ns == last_processed_ns_) return;  // timers are not new observations
    clear_poses();
    if (stamp_ns < last_processed_ns_) {
      sequence_->invalidate("non_increasing_timestamp"); publish_sequence_state(); return;
    }
    last_processed_ns_ = stamp_ns;
    observation_header_ = color_msg_->header;
    cv::Mat color;
    try {
      color = cv_bridge::toCvCopy(color_msg_, sensor_msgs::image_encodings::BGR8)->image;
    } catch (const std::exception &error) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000, "Color conversion failed: %s", error.what());
      sequence_->invalidate("invalid_color_image"); publish_sequence_state();
      publish_status("invalid_color_image", 0);
      return;
    }
    lbot_vision::Detection2D observation;
    try {
      lbot_vision::PointNormalizer normalize;
      if (camera_geometry_ && camera_geometry_->width() == static_cast<std::uint32_t>(color.cols) &&
          camera_geometry_->height() == static_cast<std::uint32_t>(color.rows)) {
        normalize = [this](const cv::Point2f &pixel) {
          return cv::Point2f(camera_geometry_->normalized_point(pixel));
        };
      }
      observation = detector_->detect(color, stamp_ns / 1000000, normalize);
    } catch (const std::exception &error) {
      RCLCPP_ERROR_THROTTLE(get_logger(), *get_clock(), 3000, "2D detection failed: %s", error.what());
      sequence_->invalidate("detection_2d_failed"); publish_sequence_state();
      publish_status("detection_2d_failed", 0);
      return;
    }
    cv::Mat debug = observation.annotated;
    if (observation.black_frame_debug_enabled) {
      std::ostringstream frame_debug_log;
      for (const auto &candidate : observation.frame_candidates) {
        frame_debug_log << " [" << candidate.reason << ": area=" << candidate.area
                        << " rect=[" << candidate.bounding_rect.x << ',' << candidate.bounding_rect.y
                        << ',' << candidate.bounding_rect.width << ',' << candidate.bounding_rect.height
                        << "] vertices=" << candidate.vertex_count
                        << " aspect=" << candidate.aspect_ratio
                        << " fill=" << candidate.fill_ratio
                        << " gray=" << candidate.mean_gray
                        << " score=" << candidate.score
                        << " hull_stabilized=" << candidate.hull_stabilized << ']';
      }
      RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 2000,
                           "Black-frame candidates:%s", frame_debug_log.str().c_str());
    }
    const auto &geometry = observation.geometry;
    // A held region may confirm newly detected nuts, but missing nuts in a
    // region whose border is not visible must not count as a successful pick.
    const bool frame_valid = observation.frame_found &&
      (!observation.frame_reused || observation.circles.size() == sequence_->snapshot().expected_count);
    const auto &sequence_snapshot = sequence_->observe(
      stamp_ns / 1000000, frame_valid, color.size(), observation.circles, observation.sizes_mm);
    publish_sequence_state();  // 2D identity/state remains available without basket/depth/TF.
    if (!observation.frame_found) {
      publish_status("frame_not_found", 0);
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
    if (std::abs((rclcpp::Time(color_msg_->header.stamp) -
                  rclcpp::Time(depth_msg_->header.stamp)).seconds() * 1000.0) > max_depth_delta_ms_) {
      publish_status("depth_color_timestamp_mismatch", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    // One timestamped transform for the entire observation; never mix TF frames in a PoseArray.
    frame_tf_.reset();
    if (use_tf_ && !target_frame_.empty() && color_msg_->header.frame_id != target_frame_) {
      try {
        frame_tf_ = tf_buffer_.lookupTransform(target_frame_, color_msg_->header.frame_id,
            rclcpp::Time(color_msg_->header.stamp), rclcpp::Duration::from_seconds(0.05));
      } catch (const tf2::TransformException &error) {
        RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000,
            "No capture-time TF; all positions remain in camera frame: %s", error.what());
      }
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
    // Initial approach uses the same rectified size metric as the full task.
    // This stream does not require stable three-nut identity or basket slots.
    if (!observation.circles.empty()) {
      const auto largest_index = static_cast<std::size_t>(std::distance(observation.sizes_mm.begin(),
        std::max_element(observation.sizes_mm.begin(), observation.sizes_mm.end())));
      const auto &largest = observation.circles.at(largest_index);
      const cv::Point2f pixel(largest[0], largest[1]);
      const double z = sample_depth(depth, {pixel.x * static_cast<float>(sx), pixel.y * static_cast<float>(sy)});
      if (valid_depth(z, depth_min_m_, depth_max_m_)) {
        geometry_msgs::msg::PointStamped target;
        target.header = color_msg_->header;
        const auto camera_point = project_pixel(pixel, z);
        if (transform_point(camera_point, color_msg_->header.frame_id, target.point, target.header.frame_id) &&
            target.header.frame_id == target_frame_ &&
            std::isfinite(target.point.x) && std::isfinite(target.point.y) && std::isfinite(target.point.z)) {
          large_target_pub_->publish(target);
          cv::putText(debug, "LARGE candidate", pixel + cv::Point2f(8, -8),
                      cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(0, 255, 255), 1);
        }
      }
    }
    // Localize the rectangular basket boundary first. Robot X is meaningful
    // only after all four corners have been transformed to base_link.
    geometry_msgs::msg::PoseArray slots;
    slots.header = color_msg_->header;
    slots.header.frame_id = target_frame_;
    std::ostringstream slots_json;
    if (observation.basket_found && target_frame_ == "base_link") {
      std::vector<cv::Point3d> base_corners;
      std::vector<cv::Point2f> base_xy, image_corners;
      for (const auto &corner : geometry.basket) {
        const cv::Point2f pixel(corner);
        const double z = sample_depth(depth, {pixel.x * static_cast<float>(sx), pixel.y * static_cast<float>(sy)});
        if (!valid_depth(z, depth_min_m_, depth_max_m_)) break;
        geometry_msgs::msg::Point point;
        std::string frame;
        if (!transform_point(project_pixel(pixel, z), color_msg_->header.frame_id, point, frame) ||
            frame != "base_link") break;
        base_corners.emplace_back(point.x, point.y, point.z);
        base_xy.emplace_back(point.x, point.y);
        image_corners.push_back(pixel);
      }
      if (base_corners.size() == 4) {
        try {
          auto centers = lbot_vision::basket_slots_robot_x(base_corners);
          // This inverse planar map is for the debug overlay only. Published
          // coordinates come directly from the robot-X partition above.
          const auto to_image = cv::getPerspectiveTransform(base_xy, image_corners);
          std::vector<cv::Point2f> slot_xy, slot_pixels;
          for (const auto &center : centers) slot_xy.emplace_back(center.x, center.y);
          cv::perspectiveTransform(slot_xy, slot_pixels, to_image);
          // The rim defines the XY footprint, not the compartment floor's
          // height. Retain center-depth sampling for placement Z, and publish
          // only a complete set of three valid centers.
          for (std::size_t i = 0; i < centers.size(); ++i) {
            const auto &pixel = slot_pixels[i];
            const double z = sample_depth(depth,
              {pixel.x * static_cast<float>(sx), pixel.y * static_cast<float>(sy)});
            if (!valid_depth(z, depth_min_m_, depth_max_m_))
              throw std::invalid_argument("slot center depth unavailable");
            geometry_msgs::msg::Point floor;
            std::string frame;
            if (!transform_point(project_pixel(pixel, z), color_msg_->header.frame_id, floor, frame) ||
                frame != "base_link" || !std::isfinite(floor.z))
              throw std::invalid_argument("slot center transform unavailable");
            centers[i].z = floor.z;
          }
          for (std::size_t i = 0; i < centers.size(); ++i) {
            const auto &center = centers[i];
            geometry_msgs::msg::Pose pose;
            pose.position.x = center.x; pose.position.y = center.y; pose.position.z = center.z;
            pose.orientation.w = 1.0;
            slots.poses.push_back(pose);
            if (i) slots_json << ',';
            slots_json << "{\"x\":" << center.x << ",\"y\":" << center.y << ",\"z\":" << center.z << "}";
            cv::drawMarker(debug, slot_pixels[i], cv::Scalar(255, 0, 255), cv::MARKER_CROSS, 20, 2);
            cv::putText(debug, "slot_" + std::to_string(i + 1) + " (+base X)",
                        slot_pixels[i] + cv::Point2f(5, -5), cv::FONT_HERSHEY_SIMPLEX,
                        0.4, cv::Scalar(255, 0, 255), 1);
          }
          slots_pub_->publish(slots);
        } catch (const std::exception &error) {
          RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 3000,
                              "Cannot partition basket along robot X: %s", error.what());
        }
      }
    }
    if (!sequence_snapshot.initialized || !sequence_snapshot.observation_valid) {
      publish_status(sequence_snapshot.status, observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    if (!observation.basket_found) {
      publish_status("basket_not_found", observation.circles.size());
      return publish_debug(debug, color_msg_->header);
    }
    std::vector<Detection> detections;
    for (const auto &track : sequence_snapshot.targets) {
      if (!track.visible || track.state == "completed") continue;
      const auto &circle = observation.circles.at(track.observation_index);
      cv::Point2f px(circle[0], circle[1]);
      double z = sample_depth(depth, {px.x * static_cast<float>(sx), px.y * static_cast<float>(sy)});
      if (!valid_depth(z, depth_min_m_, depth_max_m_)) continue;
      auto camera_point = project_pixel(px, z);
      Detection d;
      d.target_id = track.id; d.size_class = track.size_class;
      d.pixel = px;
      d.radius_px = circle[2];
      d.size_mm = observation.sizes_mm.at(track.observation_index);
      d.radius_m = camera_geometry_->projected_radius_m(px, circle[2], z);
      d.depth_m = z;
      if (!transform_point(camera_point, color_msg_->header.frame_id, d.point, d.frame_id)) {
        // Missing camera-to-robot extrinsics are not fatal; publish camera coordinates.
        d.frame_id = color_msg_->header.frame_id;
      }
      detections.push_back(d);
    }
    if (detections.size() != sequence_snapshot.expected_count) {
      publish_status("target_localization_incomplete", detections.size());
      return publish_debug(debug, color_msg_->header);
    }
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
    poses.header.frame_id = frame_tf_ ? target_frame_ : color_msg_->header.frame_id;
    std::ostringstream json;
    json << "{\"status\":\"ok\",\"nuts\":[";
    for (size_t i = 0; i < detections.size(); ++i) {
      const auto &d = detections[i];
      geometry_msgs::msg::Pose pose;
      pose.position = d.point;
      pose.orientation.w = 1.0;
      poses.poses.push_back(pose);
      const auto &target = sequence_snapshot.targets.at(static_cast<std::size_t>(d.target_id - 1));
      cv::putText(debug, d.size_class + ":" + target.state + cv::format(" %.1fmm", d.size_mm), d.pixel + cv::Point2f(8, 8),
                  cv::FONT_HERSHEY_SIMPLEX, 0.65, cv::Scalar(0, 255, 0), 2);
      if (i) json << ',';
      json << "{\"id\":" << d.target_id << ",\"size\":\"" << d.size_class
           << "\",\"size_mm\":" << d.size_mm
           << ",\"state\":\"" << target.state << "\",\"u\":" << d.pixel.x
           << ",\"v\":" << d.pixel.y << ",\"x\":" << d.point.x
           << ",\"y\":" << d.point.y << ",\"z\":" << d.point.z
           << ",\"frame\":\"" << d.frame_id << "\"}";
    }
    json << "],\"slots\":[" << slots_json.str();
    json << "],\"slot_count\":" << slots.poses.size() << "}";
    std_msgs::msg::String status;
    status.data = json.str();
    status_pub_->publish(status);
    detections_pub_->publish(poses);
    publish_sequence_state(detections);
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
