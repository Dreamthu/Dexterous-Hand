#include "lbot_control/table_route.hpp"

#include <algorithm>
#include <cmath>
#include <set>

namespace lbot_control {
namespace {

bool finite_joints(const JointConfiguration &joints)
{
  return std::all_of(
    joints.begin(), joints.end(), [](double value) {return std::isfinite(value);});
}

JointPathSegment segment(const JointWaypoint &start, const JointWaypoint &goal)
{
  return {start.name + "_to_" + goal.name, start.joints, goal.joints};
}

}  // namespace

TableRouteResult build_table_route(const TableRouteDefinition &definition)
{
  TableRouteResult result;
  if (definition.waypoints.size() < 2) {
    result.message = "table route needs at least natural-down and table-above waypoints";
    return result;
  }

  std::set<std::string> names;
  for (const auto &waypoint : definition.waypoints) {
    if (waypoint.name.empty()) {
      result.message = "table route contains an empty waypoint name";
      return result;
    }
    if (!names.insert(waypoint.name).second) {
      result.message = "duplicate table-route waypoint: " + waypoint.name;
      return result;
    }
    if (!finite_joints(waypoint.joints)) {
      result.message = "non-finite joint value in waypoint: " + waypoint.name;
      return result;
    }
  }

  result.route.enter.reserve(definition.waypoints.size() - 1);
  for (std::size_t index = 1; index < definition.waypoints.size(); ++index) {
    result.route.enter.push_back(
      segment(definition.waypoints[index - 1], definition.waypoints[index]));
  }

  result.route.leave.reserve(result.route.enter.size());
  for (auto item = result.route.enter.rbegin(); item != result.route.enter.rend(); ++item) {
    result.route.leave.push_back({
      item->name + "_reverse", item->goal, item->start});
  }

  result.success = true;
  result.message = "table route generated";
  return result;
}

}  // namespace lbot_control
