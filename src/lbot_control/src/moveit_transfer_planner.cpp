#include "lbot_control/moveit_transfer_planner.hpp"
#include "lbot_control/moveit_cloud_snapshot.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <sstream>
#include <set>
#include <tuple>
#include <stdexcept>
#include <moveit/robot_model_loader/robot_model_loader.hpp>
#include <moveit/planning_pipeline/planning_pipeline.hpp>
#include <moveit/planning_scene/planning_scene.hpp>
#include <moveit/kinematic_constraints/utils.hpp>
#include <moveit/robot_state/conversions.hpp>
#include <moveit/trajectory_processing/time_optimal_trajectory_generation.hpp>
#include <moveit_msgs/msg/display_trajectory.hpp>

namespace lbot_control {
namespace {
Eigen::Isometry3d transform(const Pose6 &p) {
  Eigen::Isometry3d result = Eigen::Isometry3d::Identity();
  result.translation() = Eigen::Vector3d(p.x, p.y, p.z);
  result.linear() = (Eigen::AngleAxisd(p.yaw, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(p.pitch, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(p.roll, Eigen::Vector3d::UnitX())).toRotationMatrix();
  return result;
}
Pose6 pose(const Eigen::Isometry3d &t) {
  const auto &r = t.linear();
  return {t.translation().x(), t.translation().y(), t.translation().z(),
    std::atan2(r(2, 1), r(2, 2)), std::atan2(-r(2, 0), std::hypot(r(0, 0), r(1, 0))),
    std::atan2(r(1, 0), r(0, 0))};
}
}
struct MoveItTransferPlanner::Impl {
  rclcpp::Node::SharedPtr node;
  std::unique_ptr<robot_model_loader::RobotModelLoader> loader;
  moveit::core::RobotModelPtr model;
  const moveit::core::JointModelGroup *group{};
  planning_scene::PlanningScenePtr scene;
  std::unique_ptr<planning_pipeline::PlanningPipeline> pipeline;
  rclcpp::Publisher<moveit_msgs::msg::DisplayTrajectory>::SharedPtr display;
  rclcpp::Publisher<moveit_msgs::msg::PlanningScene>::SharedPtr scene_display;
  std::unique_ptr<MoveItCloudSnapshot> cloud;
  bool cloud_ready{false};
  double velocity{.12}, acceleration{.12}, dt{.02}, preferred_goal_clearance{.005};
  bool keep_above{false};
  double max_drop{.005};
  std::vector<Eigen::Vector3d> hand_corners;
  double hand_min_z(const Eigen::Isometry3d &tip) const {
    double height = std::numeric_limits<double>::infinity();
    for (const auto &corner : hand_corners) height = std::min(height, (tip * corner).z());
    return height;
  }
  std::string invalid_height(moveit::core::RobotState &state,
    const MoveItTransferHeightLimits &limits) const {
    state.update();
    const auto &tip = state.getGlobalLinkTransform("arm_left_L8_Link");
    const double tip_z = tip.translation().z(), hand_z = hand_min_z(tip);
    if (tip_z >= limits.tip_z-1e-8 && hand_z >= limits.hand_z-1e-8) return {};
    std::ostringstream reason;
    reason << "transfer height floor violated: Arm_Tip z=" << tip_z << " minimum=" << limits.tip_z
      << "; hand bottom z=" << hand_z << " minimum=" << limits.hand_z;
    return reason.str();
  }
  moveit_msgs::msg::Constraints height_constraints(const MoveItTransferHeightLimits &limits) const {
    moveit_msgs::msg::Constraints constraints;
    constraints.name = "transfer_keep_above";
    const auto add_point = [&](const Eigen::Vector3d &offset, double minimum) {
      moveit_msgs::msg::PositionConstraint point;
      point.header.frame_id = "base_link";
      point.link_name = "arm_left_L8_Link";
      point.target_point_offset.x = offset.x(); point.target_point_offset.y = offset.y();
      point.target_point_offset.z = offset.z(); point.weight = 1.;
      shape_msgs::msg::SolidPrimitive box;
      box.type = box.BOX; box.dimensions = {4., 4., 2.-minimum};
      geometry_msgs::msg::Pose center;
      center.position.z = (2.+minimum)/2.; center.orientation.w = 1.;
      point.constraint_region.primitives.push_back(box);
      point.constraint_region.primitive_poses.push_back(center);
      constraints.position_constraints.push_back(point);
    };
    add_point(Eigen::Vector3d::Zero(), limits.tip_z);
    for (const auto &corner : hand_corners) add_point(corner, limits.hand_z);
    return constraints;
  }
  double joint_clearance(const moveit::core::RobotState &state) const {
    double clearance = std::numeric_limits<double>::infinity();
    for (const auto &name : group->getVariableNames()) {
      const auto &bounds = model->getVariableBounds(name);
      const double q = state.getVariablePosition(name);
      clearance = std::min({clearance, q-bounds.min_position_, bounds.max_position_-q});
    }
    return clearance;
  }
  std::string invalid(moveit::core::RobotState &state) const {
    // Geometric validity tests positions; RobotState::satisfiesBounds also
    // tests stored velocities and can reject a retimed boundary due to roundoff.
    bool bounded = true;
    for (const auto &name : group->getVariableNames()) {
      const auto &b = model->getVariableBounds(name);
      const double q = state.getVariablePosition(name);
      bounded = bounded && std::isfinite(q) && q >= b.min_position_ && q <= b.max_position_;
    }
    if (!bounded) {
      std::ostringstream reason;
      reason.precision(12);
      reason << "joint bounds violated";
      for (const auto &name : group->getVariableNames()) {
        const auto &b = model->getVariableBounds(name);
        const double q = state.getVariablePosition(name);
        if (!std::isfinite(q) || q < b.min_position_ || q > b.max_position_) {
          reason << ' ' << name << '=' << q << " outside [" << b.min_position_ << ',' << b.max_position_ << ']';
        }
      }
      return reason.str();
    }
    state.update();
    collision_detection::CollisionRequest request;
    request.group_name = "left_arm";
    request.contacts = true;
    request.max_contacts = 8;
    collision_detection::CollisionResult result;
    scene->checkCollision(request, result, state);
    if (!result.collision) return {};
    std::ostringstream text;
    text << "collision";
    for (const auto &contact : result.contacts) {
      text << ' ' << contact.first.first << '/' << contact.first.second;
      if (!contact.second.empty()) {
        const auto &hit = contact.second.front();
        text << " contact_base=[" << hit.pos.x() << ',' << hit.pos.y() << ',' << hit.pos.z()
          << "] penetration=" << hit.depth*1000. << " mm";
      }
    }
    return text.str();
  }
};

MoveItTransferPlanner::MoveItTransferPlanner(const rclcpp::Node::SharedPtr &node)
: impl_(std::make_unique<Impl>()) {
  auto &p = *impl_;
  std::vector<rclcpp::Parameter> overrides;
  for (const auto &entry : node->get_node_parameters_interface()->get_parameter_overrides()) {
    if (entry.second.get_type() != rclcpp::ParameterType::PARAMETER_NOT_SET) overrides.emplace_back(entry.first, entry.second);
  }
  p.node = std::make_shared<rclcpp::Node>("lbot_moveit_planner",
    rclcpp::NodeOptions().context(node->get_node_base_interface()->get_context())
      .use_global_arguments(false).parameter_overrides(overrides).automatically_declare_parameters_from_overrides(true));
  p.node->get_parameter_or("moveit_joint_velocity_rad_s", p.velocity, .12);
  p.node->get_parameter_or("moveit_joint_acceleration_rad_s2", p.acceleration, .12);
  p.node->get_parameter_or("moveit_goal_preferred_clearance_rad", p.preferred_goal_clearance, .005);
  p.node->get_parameter_or("moveit_transfer_keep_above", p.keep_above, false);
  p.node->get_parameter_or("moveit_transfer_max_drop_m", p.max_drop, .005);
  if (!std::isfinite(p.max_drop) || p.max_drop < 0 || p.max_drop > .05) {
    throw std::runtime_error("moveit_transfer_max_drop_m must be in [0, 0.05]");
  }
  if (!std::isfinite(p.preferred_goal_clearance) || p.preferred_goal_clearance < 0 || p.preferred_goal_clearance > .05) {
    throw std::runtime_error("moveit_goal_preferred_clearance_rad must be in [0, 0.05]");
  }
  double hz;
  p.node->get_parameter_or("joint_follow_rate_hz", hz, 50.);
  if (!std::isfinite(hz) || hz < 10 || hz > 100 || !std::isfinite(p.velocity) ||
      p.velocity <= 0 || p.velocity > 1 || !std::isfinite(p.acceleration) || p.acceleration <= 0 || p.acceleration > 1) {
    throw std::runtime_error("invalid MoveIt velocity/acceleration or joint_follow rate (10..100 Hz)");
  }
  p.dt = 1. / hz;
  std::string urdf, srdf;
  if (!p.node->get_parameter("robot_description", urdf) ||
      !p.node->get_parameter("robot_description_semantic", srdf) || urdf.empty() || srdf.empty()) {
    throw std::runtime_error("MoveIt model parameters missing; use lbot_task.launch.py");
  }
  p.loader = std::make_unique<robot_model_loader::RobotModelLoader>(p.node, "robot_description");
  p.model = p.loader->getModel();
  if (!p.model || p.model->getModelFrame() != "base_link") throw std::runtime_error("MoveIt model must use base_link");
  p.group = p.model->getJointModelGroup("left_arm");
  if (!p.group || p.group->getVariableCount() != 7 || !p.group->getSolverInstance()) {
    throw std::runtime_error("MoveIt left_arm chain / KDL solver unavailable");
  }
  for (std::size_t i = 0; i < 7; ++i) {
    if (p.group->getVariableNames()[i] != "arm_left_L" + std::to_string(i+1) + "_Joint") {
      throw std::runtime_error("unexpected MoveIt joint order");
    }
  }
  p.scene = std::make_shared<planning_scene::PlanningScene>(p.model);
  p.cloud = std::make_unique<MoveItCloudSnapshot>(p.node);
  // Use the actual convex collision shape for height limits, not the empty
  // corners of an axis-aligned box around an obliquely mounted hand.
  const auto *hand = p.model->getLinkModel("left_hand_envelope");
  if (!hand || hand->getShapes().size() != 1) {
    throw std::runtime_error("MoveIt requires one rigid left_hand_envelope collision shape");
  }
  const auto &hand_origin = hand->getCollisionOriginTransforms()[0];
  if (hand->getShapes()[0]->type == shapes::BOX) {
    const auto *box = static_cast<const shapes::Box *>(hand->getShapes()[0].get());
    for (int x : {-1, 1}) for (int y : {-1, 1}) for (int z : {-1, 1}) {
      p.hand_corners.push_back(hand_origin * Eigen::Vector3d(x*box->size[0]/2., y*box->size[1]/2., z*box->size[2]/2.));
    }
  } else if (hand->getShapes()[0]->type == shapes::MESH) {
    const auto *mesh = static_cast<const shapes::Mesh *>(hand->getShapes()[0].get());
    std::set<std::tuple<double, double, double>> unique;
    for (unsigned i = 0; i < mesh->vertex_count; ++i) {
      unique.emplace(mesh->vertices[3*i], mesh->vertices[3*i+1], mesh->vertices[3*i+2]);
    }
    for (const auto &[x, y, z] : unique) p.hand_corners.push_back(hand_origin*Eigen::Vector3d(x, y, z));
  } else {
    throw std::runtime_error("unsupported hand collision shape");
  }
  if (p.hand_corners.empty()) throw std::runtime_error("empty hand collision shape");
  std::vector<double> boxes;
  p.node->get_parameter_or("moveit_obstacle_boxes", boxes, std::vector<double>{});
  if (boxes.size() % 6) throw std::runtime_error("moveit_obstacle_boxes must contain repeated [x,y,z,sx,sy,sz]");
  for (std::size_t i = 0; i < boxes.size(); i += 6) {
    for (std::size_t j = 0; j < 6; ++j) {
      if (!std::isfinite(boxes[i+j]) || (j >= 3 && boxes[i+j] <= 0)) throw std::runtime_error("invalid obstacle box");
    }
    moveit_msgs::msg::CollisionObject object;
    object.id = "configured_box_" + std::to_string(i/6);
    object.header.frame_id = "base_link";
    object.operation = object.ADD;
    shape_msgs::msg::SolidPrimitive shape;
    shape.type = shape.BOX;
    shape.dimensions = {boxes[i+3], boxes[i+4], boxes[i+5]};
    geometry_msgs::msg::Pose where;
    where.position.x = boxes[i]; where.position.y = boxes[i+1]; where.position.z = boxes[i+2];
    where.orientation.w = 1;
    object.primitives.push_back(shape); object.primitive_poses.push_back(where);
    if (!p.scene->processCollisionObjectMsg(object)) throw std::runtime_error("failed to add obstacle box");
  }
  p.pipeline = std::make_unique<planning_pipeline::PlanningPipeline>(p.model, p.node, "ompl",
    std::vector<std::string>{"ompl_interface/OMPLPlanner"});
  p.display = node->create_publisher<moveit_msgs::msg::DisplayTrajectory>(
    "~/moveit_trajectory", rclcpp::QoS(1).transient_local());
  p.scene_display = node->create_publisher<moveit_msgs::msg::PlanningScene>(
    "~/moveit_planning_scene", rclcpp::QoS(1).transient_local());
  RCLCPP_INFO(node->get_logger(), "MoveIt ready: KDL + OMPL RRTConnect, %.1f Hz, %.3f rad/s; "
    "collision model: torso/left arm/hand envelope and %zu configured world boxes", hz, p.velocity, boxes.size()/6);
  RCLCPP_INFO(node->get_logger(), "MoveIt transfer height guard=%s, max drop below lower endpoint=%.3f m; "
    "Arm_Tip and %zu hand collision vertices", p.keep_above ? "true" : "false", p.max_drop, p.hand_corners.size());
}
MoveItTransferPlanner::~MoveItTransferPlanner() = default;

MotionResult MoveItTransferPlanner::capture_obstacle_cloud(const std::array<double, 7> &joints) {
  auto &p = *impl_;
  p.cloud_ready = false;
  if (!p.cloud->enabled()) return MotionResult::ok("point-cloud obstacles disabled");
  moveit::core::RobotState state(p.model);
  state.setToDefaultValues(); state.setJointGroupPositions(p.group, joints.data()); state.update();
  for (std::size_t j = 0; j < joints.size(); ++j) {
    const auto &bounds = p.model->getVariableBounds(p.group->getVariableNames()[j]);
    if (!std::isfinite(joints[j]) || joints[j] < bounds.min_position_ || joints[j] > bounds.max_position_) {
      return MotionResult::fail("invalid measured left joints for point-cloud self filter");
    }
  }
  const auto captured = p.cloud->capture(state);
  if (!captured.success) return MotionResult::fail(captured.message);
  p.scene->processOctomapMsg(captured.map);
  p.scene->setCurrentState(state);
  p.cloud_ready = true;
  moveit_msgs::msg::PlanningScene scene;
  p.scene->getPlanningSceneMsg(scene); p.scene_display->publish(scene);
  RCLCPP_INFO(p.node->get_logger(), "%s", captured.message.c_str());
  return MotionResult::ok(captured.message);
}

Pose6 MoveItTransferPlanner::forward_kinematics(const std::array<double, 7> &joints) const {
  moveit::core::RobotState state(impl_->model);
  state.setToDefaultValues();
  state.setJointGroupPositions(impl_->group, joints.data());
  state.update();
  return pose(state.getGlobalLinkTransform("arm_left_L8_Link"));
}

MoveItTransferPlan MoveItTransferPlanner::plan(const std::array<double, 7> &start,
  const std::vector<Pose6> &candidates, double timeout_s, const std::array<double, 7> *fixed_goal_joints,
  const MoveItTransferHeightLimits *height_limits) {
  MoveItTransferPlan output;
  auto &p = *impl_;
  if (p.cloud->enabled() && !p.cloud_ready) {
    output.message = "required point-cloud snapshot missing; capture obstacles before motion; no trajectory sent";
    return output;
  }
  if (!std::isfinite(timeout_s) || timeout_s <= 0 || candidates.empty() ||
      !std::all_of(start.begin(), start.end(), [](double v) {return std::isfinite(v);})) {
    output.message = "invalid MoveIt planning input"; return output;
  }
  moveit::core::RobotState initial(p.model);
  initial.setToDefaultValues(); initial.setJointGroupPositions(p.group, start.data()); initial.update();
  if (const auto why = p.invalid(initial); !why.empty()) {
    output.message = "MoveIt start state rejected: " + why; return output;
  }
  p.scene->setCurrentState(initial);
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout_s);
  std::size_t tried = 0;
  std::string last_error = "no bounded collision-free IK solution";
  for (const auto &candidate : candidates) {
    if (!rclcpp::ok() || std::chrono::steady_clock::now() >= deadline) break;
    const auto desired = transform(candidate);
    if (!desired.matrix().allFinite()) {last_error = "non-finite goal"; continue;}
    ++tried;
    std::optional<MoveItTransferHeightLimits> floors;
    if (height_limits) {
      floors = *height_limits;
    } else if (p.keep_above) {
      const auto &start_tip = initial.getGlobalLinkTransform("arm_left_L8_Link");
      floors = MoveItTransferHeightLimits{
        std::min(start_tip.translation().z(), candidate.z)-p.max_drop,
        std::min(p.hand_min_z(start_tip), p.hand_min_z(desired))-p.max_drop};
    }
    if (floors) {
      if (!std::isfinite(floors->tip_z) || !std::isfinite(floors->hand_z) ||
          floors->tip_z >= 2. || floors->hand_z >= 2.) {
        output.message = "invalid transfer height floors"; return output;
      }
      if (const auto why = p.invalid_height(initial, *floors); !why.empty()) {
        output.message = "MoveIt start state rejected: " + why; return output;
      }
    }
    moveit::core::RobotState goal(initial);
    const auto validity = [&](moveit::core::RobotState *state,
      const moveit::core::JointModelGroup *group, const double *values) {
        state->setJointGroupPositions(group, values);
        return p.invalid(*state).empty() && (!floors || p.invalid_height(*state, *floors).empty());
      };
    const double remaining = std::chrono::duration<double>(deadline - std::chrono::steady_clock::now()).count();
    if (remaining <= 0) break;
    if (fixed_goal_joints) {
      goal.setJointGroupPositions(p.group, fixed_goal_joints->data());
      if (const auto why = p.invalid(goal); !why.empty()) {last_error = "replan endpoint: " + why; continue;}
      if (floors) {
        if (const auto why = p.invalid_height(goal, *floors); !why.empty()) {last_error = why; continue;}
      }
    } else if (!goal.setFromIK(p.group, desired, "arm_left_L8_Link", std::min(.12, remaining), validity)) continue;
    // Prefer an equivalent endpoint away from a software limit. This is an IK
    // seed preference, not a change to model/command/feedback bounds: if the
    // refinement cannot find the same pose, retain the original valid solution.
    if (!fixed_goal_joints && p.joint_clearance(goal) < p.preferred_goal_clearance) {
      const auto original_goal = goal;
      for (int retry = 1; retry <= 4; ++retry) {
        const double left = std::chrono::duration<double>(deadline-std::chrono::steady_clock::now()).count();
        if (left <= .01) break;
        auto refined = original_goal;
        for (const auto &name : p.group->getVariableNames()) {
          const auto &bounds = p.model->getVariableBounds(name);
          const double q = refined.getVariablePosition(name);
          const double inset = std::min(2*retry*p.preferred_goal_clearance,
            (bounds.max_position_-bounds.min_position_)/2);
          if (q-bounds.min_position_ < p.preferred_goal_clearance) {
            refined.setVariablePosition(name, bounds.min_position_+inset);
          } else if (bounds.max_position_-q < p.preferred_goal_clearance) {
            refined.setVariablePosition(name, bounds.max_position_-inset);
          }
        }
        const auto preferred_validity = [&](moveit::core::RobotState *state,
          const moveit::core::JointModelGroup *group, const double *values) {
          return validity(state, group, values) && p.joint_clearance(*state) >= p.preferred_goal_clearance;
        };
        if (!refined.setFromIK(p.group, desired, "arm_left_L8_Link", std::min(.03, left), preferred_validity)) continue;
        refined.update();
        const auto reached = pose(refined.getGlobalLinkTransform("arm_left_L8_Link"));
        if (std::hypot(reached.x-candidate.x, reached.y-candidate.y, reached.z-candidate.z) <= .0005 &&
            orientation_distance(reached, candidate) <= .005) {
          goal = refined;
          break;
        }
      }
    }
    goal.update();
    const auto actual = pose(goal.getGlobalLinkTransform("arm_left_L8_Link"));
    if (std::hypot(actual.x-candidate.x, actual.y-candidate.y, actual.z-candidate.z) > .0005 ||
        orientation_distance(actual, candidate) > .005) {last_error = "KDL endpoint residual too large"; continue;}
    planning_interface::MotionPlanRequest request;
    request.group_name = "left_arm"; request.planner_id = "RRTConnect";
    request.allowed_planning_time = std::min(3., std::chrono::duration<double>(deadline - std::chrono::steady_clock::now()).count());
    if (request.allowed_planning_time <= 0) break;
    request.num_planning_attempts = 1;
    request.workspace_parameters.header.frame_id = "base_link";
    request.workspace_parameters.min_corner.x = -2; request.workspace_parameters.min_corner.y = -2;
    request.workspace_parameters.min_corner.z = -2;
    request.workspace_parameters.max_corner.x = 2; request.workspace_parameters.max_corner.y = 2;
    request.workspace_parameters.max_corner.z = 2;
    moveit::core::robotStateToRobotStateMsg(initial, request.start_state);
    request.goal_constraints.push_back(kinematic_constraints::constructGoalConstraints(goal, p.group, 1e-5));
    if (floors) request.path_constraints = p.height_constraints(*floors);
    planning_interface::MotionPlanResponse response;
    // Try the simple joint connection first. It uses exactly the same model,
    // world collisions and height floors; this is not an unchecked SDK MoveJ.
    // Avoid costly rejection sampling for short, already collision-free moves.
    double connection_delta = 0;
    for (const auto &name : p.group->getVariableNames()) {
      connection_delta = std::max(connection_delta,
        std::abs(goal.getVariablePosition(name)-initial.getVariablePosition(name)));
    }
    const auto connection_steps = std::max<std::size_t>(1, std::ceil(connection_delta/.005));
    bool direct = true;
    for (std::size_t i = 0; i <= connection_steps; ++i) {
      if (!rclcpp::ok() || std::chrono::steady_clock::now() >= deadline) {direct = false; break;}
      moveit::core::RobotState sample(initial);
      initial.interpolate(goal, double(i)/connection_steps, sample);
      if (!p.invalid(sample).empty() || (floors && !p.invalid_height(sample, *floors).empty())) {direct = false; break;}
    }
    if (direct) {
      response.trajectory = std::make_shared<robot_trajectory::RobotTrajectory>(p.model, "left_arm");
      response.trajectory->addSuffixWayPoint(initial, 0.);
      response.trajectory->addSuffixWayPoint(goal, 0.);
      RCLCPP_INFO(p.node->get_logger(), "MoveIt direct joint connection passed collision/height checks (%zu samples)",
        connection_steps+1);
    } else {
      request.allowed_planning_time = std::min(3.,
        std::chrono::duration<double>(deadline-std::chrono::steady_clock::now()).count());
      if (request.allowed_planning_time <= 0) break;
      if (!p.pipeline->generatePlan(p.scene, request, response) || !response.trajectory || response.trajectory->empty()) {
        last_error = floors ? "OMPL could not connect start to candidate above Arm_Tip/hand height floors" :
          "OMPL could not connect start to candidate"; continue;
      }
    }
    // TOTG can change the geometric path. Check the RETIMED path again below.
    trajectory_processing::TimeOptimalTrajectoryGeneration timing(.000001, p.dt, .000001);
    if (!timing.computeTimeStamps(*response.trajectory, .95, .95)) {last_error = "MoveIt time parameterization failed"; continue;}
    std::vector<TimedJointPoint> points;
    moveit::core::RobotState previous(initial);
    double elapsed = 0;
    double minimum_tip_z = std::numeric_limits<double>::infinity();
    double minimum_hand_z = std::numeric_limits<double>::infinity();
    bool valid = true;
    for (std::size_t i = 0; i < response.trajectory->getWayPointCount(); ++i) {
      elapsed += response.trajectory->getWayPointDurationFromPrevious(i);
      const auto &state = response.trajectory->getWayPoint(i);
      for (const auto &name : p.group->getVariableNames()) {
        if (!state.hasVelocities() || !state.hasAccelerations() ||
            !std::isfinite(state.getVariableVelocity(name)) || !std::isfinite(state.getVariableAcceleration(name)) ||
            std::abs(state.getVariableVelocity(name)) > p.velocity + 1e-8 ||
            std::abs(state.getVariableAcceleration(name)) > p.acceleration + 1e-8) {
          valid = false; last_error = "retimed velocity/acceleration exceeds configured limits"; break;
        }
      }
      if (!valid) break;
      TimedJointPoint point; point.time_s = elapsed;
      state.copyJointGroupPositions(p.group, point.joints.data());
      double delta = 0;
      for (std::size_t j = 0; j < 7; ++j) delta = std::max(delta,
        std::abs(point.joints[j] - previous.getVariablePosition(p.group->getVariableNames()[j])));
      const std::size_t steps = std::max<std::size_t>(1, std::ceil(delta / .005));
      for (std::size_t k = 0; k <= steps; ++k) {
        moveit::core::RobotState sample(previous);
        previous.interpolate(state, double(k)/steps, sample);
        if (const auto why = p.invalid(sample); !why.empty()) {last_error = "retimed path: " + why; valid = false; break;}
        if (floors) {
          if (const auto why = p.invalid_height(sample, *floors); !why.empty()) {
            last_error = "retimed path: " + why; valid = false; break;
          }
        }
        const auto &sample_tip = sample.getGlobalLinkTransform("arm_left_L8_Link");
        minimum_tip_z = std::min(minimum_tip_z, sample_tip.translation().z());
        minimum_hand_z = std::min(minimum_hand_z, p.hand_min_z(sample_tip));
      }
      if (!valid) break;
      points.push_back(point); previous = state;
    }
    if (!valid || points.size() < 2 || points.back().time_s <= 0) continue;
    // Validate the positions actually streamed, including time/order and speed.
    for (std::size_t i = 1; i < points.size() && valid; ++i) {
      const double dt = points[i].time_s - points[i-1].time_s;
      if (!std::isfinite(dt) || dt <= 0) {valid = false; break;}
      for (std::size_t j = 0; j < 7; ++j) {
        if (!std::isfinite(points[i].joints[j]) || std::abs(points[i].joints[j]-points[i-1].joints[j])/dt > p.velocity+1e-8) valid = false;
      }
    }
    const auto final_pose = forward_kinematics(points.back().joints);
    if (!valid || std::hypot(final_pose.x-candidate.x, final_pose.y-candidate.y, final_pose.z-candidate.z) > .0005 ||
        orientation_distance(final_pose, candidate) > .005) {last_error = "retimed speed or endpoint check failed"; continue;}
    moveit_msgs::msg::DisplayTrajectory display;
    display.model_id = p.model->getName();
    moveit::core::robotStateToRobotStateMsg(initial, display.trajectory_start);
    moveit_msgs::msg::RobotTrajectory trajectory;
    response.trajectory->getRobotTrajectoryMsg(trajectory);
    display.trajectory.push_back(trajectory); p.display->publish(display);
    output.success = true; output.goal = candidate; output.points = std::move(points);
    output.height_limits = floors;
    if (floors) {
      RCLCPP_INFO(p.node->get_logger(), "MoveIt transfer height check: minimum sampled Arm_Tip z=%.6f "
        "(floor=%.6f), hand bottom z=%.6f (floor=%.6f), base_link",
        minimum_tip_z, floors->tip_z, minimum_hand_z, floors->hand_z);
    }
    std::ostringstream endpoint;
    endpoint.precision(9);
    endpoint << "MoveIt endpoint joints=[";
    for (std::size_t j = 0; j < 7; ++j) endpoint << (j ? "," : "") << output.points.back().joints[j];
    moveit::core::RobotState final_state(initial);
    final_state.setJointGroupPositions(p.group, output.points.back().joints.data());
    endpoint << "]; nearest joint limit=" << p.joint_clearance(final_state)
      << " rad; preferred=" << p.preferred_goal_clearance << " rad";
    RCLCPP_INFO(p.node->get_logger(), "%s", endpoint.str().c_str());
    output.message = "MoveIt KDL/OMPL trajectory accepted; candidate " + std::to_string(tried) +
      ", " + std::to_string(output.points.size()) + " samples, " + std::to_string(elapsed) + " s";
    return output;
  }
  output.message = "MoveIt transfer failed after " + std::to_string(tried) + " candidates: " + last_error +
    "; no SDK IK / MoveJP fallback and no trajectory sent";
  return output;
}
}  // namespace lbot_control
