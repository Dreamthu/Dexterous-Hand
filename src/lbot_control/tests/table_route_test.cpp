#include <cassert>
#include <limits>

#include "lbot_control/table_route.hpp"

int main()
{
  lbot_control::TableRouteDefinition definition;
  for (const auto *name : {"natural", "clear_table", "above_table", "camera_pose"}) {
    lbot_control::JointWaypoint waypoint;
    waypoint.name = name;
    waypoint.joints.fill(static_cast<double>(definition.waypoints.size()) * 0.1);
    definition.waypoints.push_back(waypoint);
  }

  const auto result = lbot_control::build_table_route(definition);
  assert(result.success);
  assert(result.route.enter.size() == 3);
  assert(result.route.leave.size() == 3);
  assert(result.route.enter.front().start == definition.waypoints.front().joints);
  assert(result.route.enter.back().goal == definition.waypoints.back().joints);
  assert(result.route.leave.front().start == definition.waypoints.back().joints);
  assert(result.route.leave.back().goal == definition.waypoints.front().joints);

  definition.waypoints.resize(1);
  assert(!lbot_control::build_table_route(definition).success);
  definition.waypoints.push_back(definition.waypoints.front());
  assert(!lbot_control::build_table_route(definition).success);
  definition.waypoints.back().name = "other";
  definition.waypoints.back().joints[3] = std::numeric_limits<double>::quiet_NaN();
  assert(!lbot_control::build_table_route(definition).success);
  return 0;
}
