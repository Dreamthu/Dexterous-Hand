#include "lbot_control/moveit_cloud_snapshot.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <thread>
#include <unordered_set>
#include <geometric_shapes/body_operations.h>
#include <octomap/OcTree.h>
#include <octomap_msgs/conversions.h>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <tf2_ros/buffer.hpp>
#include <tf2_ros/transform_listener.hpp>

namespace lbot_control {
struct MoveItCloudSnapshot::Impl {
  bool enabled{false};
  rclcpp::Node::SharedPtr node;
  std::unique_ptr<tf2_ros::Buffer> tf;
  std::unique_ptr<tf2_ros::TransformListener> listener;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription;
  sensor_msgs::msg::PointCloud2::ConstSharedPtr latest;
  std::string topic;
  double resolution{.015}, padding{.015}, self_padding{.03};
  int timeout_ms{8000}, max_age_ms{1000}, minimum_points{500};
  std::vector<double> crop;
};

MoveItCloudSnapshot::MoveItCloudSnapshot(const rclcpp::Node::SharedPtr &parameters)
: impl_(std::make_unique<Impl>()) {
  auto &p = *impl_;
  parameters->get_parameter_or("moveit_point_cloud_enabled", p.enabled, false);
  if (!p.enabled) return;
  parameters->get_parameter_or("moveit_point_cloud_topic", p.topic, std::string("/camera/depth/points"));
  parameters->get_parameter_or("moveit_point_cloud_resolution_m", p.resolution, .015);
  parameters->get_parameter_or("moveit_point_cloud_padding_m", p.padding, .015);
  parameters->get_parameter_or("moveit_point_cloud_self_filter_padding_m", p.self_padding, .03);
  parameters->get_parameter_or("moveit_point_cloud_timeout_ms", p.timeout_ms, 8000);
  parameters->get_parameter_or("moveit_point_cloud_max_age_ms", p.max_age_ms, 1000);
  parameters->get_parameter_or("moveit_point_cloud_min_points", p.minimum_points, 500);
  parameters->get_parameter_or("moveit_point_cloud_crop", p.crop,
    std::vector<double>{-.15, -1., -.8, 1.2, 1., .6});
  if (p.topic.empty() || !std::isfinite(p.resolution) || p.resolution < .005 || p.resolution > .05 ||
      !std::isfinite(p.padding) || p.padding < 0 || p.padding > .05 ||
      !std::isfinite(p.self_padding) || p.self_padding < 0 || p.self_padding > .08 ||
      p.timeout_ms < 100 || p.timeout_ms > 30000 || p.max_age_ms < 50 || p.max_age_ms > 3000 ||
      p.minimum_points < 1 || p.crop.size() != 6) throw std::runtime_error("invalid MoveIt point-cloud configuration");
  for (int i = 0; i < 3; ++i) {
    if (!std::isfinite(p.crop[i]) || !std::isfinite(p.crop[i+3]) || p.crop[i] >= p.crop[i+3] ||
        p.crop[i] < -3 || p.crop[i+3] > 3) throw std::runtime_error("invalid point-cloud crop in base_link");
  }
  p.node = std::make_shared<rclcpp::Node>("lbot_cloud_snapshot",
    rclcpp::NodeOptions().context(parameters->get_node_base_interface()->get_context()).use_global_arguments(false));
  p.tf = std::make_unique<tf2_ros::Buffer>(p.node->get_clock());
  p.listener = std::make_unique<tf2_ros::TransformListener>(*p.tf, p.node, false);
  p.subscription = p.node->create_subscription<sensor_msgs::msg::PointCloud2>(p.topic,
    rclcpp::SensorDataQoS().keep_last(1), [&p](sensor_msgs::msg::PointCloud2::ConstSharedPtr cloud) {p.latest = cloud;});
}
MoveItCloudSnapshot::~MoveItCloudSnapshot() = default;
bool MoveItCloudSnapshot::enabled() const {return impl_->enabled;}

MoveItCloudSnapshotResult MoveItCloudSnapshot::capture(const moveit::core::RobotState &robot) {
  auto &p = *impl_;
  MoveItCloudSnapshotResult result;
  if (!p.enabled) {result.success = true; result.message = "point-cloud snapshot disabled"; return result;}
  try {
    p.latest.reset();
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(p.timeout_ms);
    const auto requested = p.node->now();
    Eigen::Isometry3d camera_to_base = Eigen::Isometry3d::Identity();
    sensor_msgs::msg::PointCloud2::ConstSharedPtr cloud;
    std::string last_error = "no fresh PointCloud2 received on " + p.topic;
    RCLCPP_INFO(p.node->get_logger(), "capturing obstacle cloud before motion: %s -> base_link", p.topic.c_str());
    while (rclcpp::ok() && std::chrono::steady_clock::now() < deadline) {
      rclcpp::spin_some(p.node);
      if (p.latest) {
        const auto stamp = rclcpp::Time(p.latest->header.stamp);
        const double age_ms = (p.node->now()-stamp).seconds()*1000.;
        if (stamp.nanoseconds() <= 0 || stamp < requested || age_ms < -100 || age_ms > p.max_age_ms) {
          last_error = "stale point-cloud timestamp; age=" + std::to_string(age_ms) + " ms";
        } else if (p.latest->header.frame_id.empty()) {
          last_error = "point cloud has empty frame_id";
        } else {
          try {
            const auto tf = p.tf->lookupTransform("base_link", p.latest->header.frame_id, stamp);
            const auto &q = tf.transform.rotation;
            Eigen::Quaterniond rotation(q.w, q.x, q.y, q.z);
            const auto &v = tf.transform.translation;
            if (!rotation.coeffs().allFinite() || std::abs(rotation.norm()-1) > .001) {
              throw std::runtime_error("invalid point-cloud TF quaternion");
            }
            camera_to_base.linear() = rotation.normalized().toRotationMatrix();
            camera_to_base.translation() = Eigen::Vector3d(v.x, v.y, v.z);
            if (!camera_to_base.matrix().allFinite()) throw std::runtime_error("invalid point-cloud TF");
            cloud = p.latest; break;
          } catch (const tf2::TransformException &error) {
            last_error = std::string("point-cloud TF unavailable: ") + error.what();
          }
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (!cloud) throw std::runtime_error(last_error);
    const auto count = std::uint64_t(cloud->width)*cloud->height;
    if (!count || count > 2000000 || !cloud->point_step ||
        std::uint64_t(cloud->row_step) < std::uint64_t(cloud->width)*cloud->point_step ||
        std::uint64_t(cloud->row_step)*cloud->height > cloud->data.size()) {
      throw std::runtime_error("malformed or oversized point cloud");
    }
    std::array<sensor_msgs::msg::PointField, 3> fields;
    for (std::size_t i = 0; i < 3; ++i) {
      const auto name = std::string(1, "xyz"[i]);
      const auto field = std::find_if(cloud->fields.begin(), cloud->fields.end(),
        [&](const auto &f) {return f.name == name;});
      if (field == cloud->fields.end() || field->count != 1 ||
          (field->datatype != field->FLOAT32 && field->datatype != field->FLOAT64) ||
          std::uint64_t(field->offset)+(field->datatype == field->FLOAT32 ? 4 : 8) > cloud->point_step) {
        throw std::runtime_error("point cloud needs scalar FLOAT32/FLOAT64 x,y,z fields");
      }
      fields[i] = *field;
    }
    const std::uint16_t endian_test = 1;
    const bool host_big_endian = *reinterpret_cast<const std::uint8_t *>(&endian_test) == 0;
    const auto coordinate = [&](const std::uint8_t *data, const auto &field) {
      std::uint8_t bytes[8]{};
      const std::size_t size = field.datatype == field.FLOAT32 ? 4 : 8;
      std::memcpy(bytes, data+field.offset, size);
      if (cloud->is_bigendian != host_big_endian) std::reverse(bytes, bytes+size);
      if (size == 4) {float value; std::memcpy(&value, bytes, 4); return double(value);}
      double value; std::memcpy(&value, bytes, 8); return value;
    };
    // Only the supplied torso/left-arm model is filtered; no right-arm state is read.
    auto measured = robot; measured.update();
    std::vector<std::unique_ptr<bodies::Body>> self_bodies;
    std::vector<bodies::BoundingSphere> spheres;
    for (const auto *link : measured.getRobotModel()->getLinkModelsWithCollisionGeometry()) {
      for (std::size_t i = 0; i < link->getShapes().size(); ++i) {
        std::unique_ptr<bodies::Body> body(bodies::createBodyFromShape(link->getShapes()[i].get()));
        if (!body) throw std::runtime_error("cannot construct robot point-cloud self filter");
        body->setPadding(p.self_padding);
        body->setPose(measured.getGlobalLinkTransform(link)*link->getCollisionOriginTransforms()[i]);
        bodies::BoundingSphere sphere; body->computeBoundingSphere(sphere);
        spheres.push_back(sphere); self_bodies.push_back(std::move(body));
      }
    }
    octomap::OcTree tree(p.resolution);
    octomap::KeySet occupied;
    std::size_t cropped = 0, removed_self = 0, accepted = 0;
    for (std::uint32_t row = 0; row < cloud->height; ++row) {
      for (std::uint32_t col = 0; col < cloud->width; ++col) {
        const auto *data = cloud->data.data()+std::size_t(row)*cloud->row_step+std::size_t(col)*cloud->point_step;
        Eigen::Vector3d point(coordinate(data, fields[0]), coordinate(data, fields[1]), coordinate(data, fields[2]));
        if (!point.allFinite()) continue;
        point = camera_to_base*point;
        if (point.x() < p.crop[0] || point.y() < p.crop[1] || point.z() < p.crop[2] ||
            point.x() > p.crop[3] || point.y() > p.crop[4] || point.z() > p.crop[5]) continue;
        ++cropped;
        bool self = false;
        for (std::size_t i = 0; i < self_bodies.size(); ++i) {
          if ((point-spheres[i].center).squaredNorm() <= spheres[i].radius*spheres[i].radius &&
              self_bodies[i]->containsPoint(point)) {self = true; break;}
        }
        if (self) {++removed_self; continue;}
        octomap::OcTreeKey key;
        if (!tree.coordToKeyChecked(point.x(), point.y(), point.z(), key)) continue;
        occupied.insert(key); ++accepted;
      }
    }
    if (accepted < std::size_t(p.minimum_points) || occupied.empty()) {
      throw std::runtime_error("insufficient obstacle points after crop/self-filter: " + std::to_string(accepted));
    }
    // Conservative voxel inflation; round padding up to whole cells. Unknown
    // space remains unknown, so this is observed-obstacle avoidance, not coverage.
    const int steps = int(std::ceil(p.padding/p.resolution-1e-9));
    octomap::KeySet inflated;
    for (const auto &key : occupied) {
      for (int x = -steps; x <= steps; ++x) for (int y = -steps; y <= steps; ++y) for (int z = -steps; z <= steps; ++z) {
        const int a = int(key[0])+x, b = int(key[1])+y, c = int(key[2])+z;
        if (a < 0 || b < 0 || c < 0 || a > 65535 || b > 65535 || c > 65535) continue;
        inflated.insert(octomap::OcTreeKey(a, b, c));
      }
      if (inflated.size() > 250000) throw std::runtime_error("obstacle map exceeds 250000 voxels; check crop/resolution");
    }
    for (const auto &key : inflated) tree.updateNode(key, true, true);
    tree.updateInnerOccupancy(); tree.prune();
    if (!octomap_msgs::binaryMapToMsg(tree, result.map)) throw std::runtime_error("cannot serialize obstacle octomap");
    result.map.header.frame_id = "base_link"; result.map.header.stamp = cloud->header.stamp;
    result.success = true;
    result.message = "point-cloud snapshot ready: source=" + p.topic + " frame=" + cloud->header.frame_id +
      " -> base_link; cropped=" + std::to_string(cropped) + ", self_filtered=" + std::to_string(removed_self) +
      ", occupied=" + std::to_string(occupied.size()) + ", inflated=" + std::to_string(inflated.size()) +
      "; resolution=" + std::to_string(p.resolution) + " m; cached before enter";
  } catch (const std::exception &error) {
    result.message = std::string("point-cloud snapshot failed before motion: ") + error.what();
  }
  p.latest.reset();
  return result;
}
}
