#ifndef LBOT_CONTROL__TABLE_ROUTE_HPP_
#define LBOT_CONTROL__TABLE_ROUTE_HPP_

#include <array>
#include <string>
#include <vector>

namespace lbot_control {

using JointConfiguration = std::array<double, 7>;

struct JointWaypoint
{
  std::string name;
  JointConfiguration joints{};
};

struct JointPathSegment
{
  std::string name;
  JointConfiguration start{};
  JointConfiguration goal{};
};

// The first waypoint is the natural-down pose and the final waypoint is the
// working pose above the table. Any number of intermediate waypoints may be
// inserted by configuration without changing the task state machine.
struct TableRouteDefinition
{
  std::vector<JointWaypoint> waypoints;
};

struct TableRoutePlan
{
  std::vector<JointPathSegment> enter;
  std::vector<JointPathSegment> leave;
};

struct TableRouteResult
{
  bool success{false};
  std::string message;
  TableRoutePlan route;
};

TableRouteResult build_table_route(const TableRouteDefinition &definition);

}  // namespace lbot_control

#endif  // LBOT_CONTROL__TABLE_ROUTE_HPP_
