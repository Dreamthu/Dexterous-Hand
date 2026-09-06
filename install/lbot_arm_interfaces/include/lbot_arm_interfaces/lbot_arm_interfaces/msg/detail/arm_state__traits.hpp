// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from lbot_arm_interfaces:msg/ArmState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/arm_state.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__TRAITS_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "lbot_arm_interfaces/msg/detail/arm_state__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'euler'
#include "geometry_msgs/msg/detail/vector3__traits.hpp"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__traits.hpp"

namespace lbot_arm_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const ArmState & msg,
  std::ostream & out)
{
  out << "{";
  // member: joints
  {
    if (msg.joints.size() == 0) {
      out << "joints: []";
    } else {
      out << "joints: [";
      size_t pending_items = msg.joints.size();
      for (auto item : msg.joints) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: euler
  {
    out << "euler: ";
    to_flow_style_yaml(msg.euler, out);
    out << ", ";
  }

  // member: pose
  {
    out << "pose: ";
    to_flow_style_yaml(msg.pose, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ArmState & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: joints
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.joints.size() == 0) {
      out << "joints: []\n";
    } else {
      out << "joints:\n";
      for (auto item : msg.joints) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: euler
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "euler:\n";
    to_block_style_yaml(msg.euler, out, indentation + 2);
  }

  // member: pose
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pose:\n";
    to_block_style_yaml(msg.pose, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ArmState & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace lbot_arm_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use lbot_arm_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lbot_arm_interfaces::msg::ArmState & msg,
  std::ostream & out, size_t indentation = 0)
{
  lbot_arm_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lbot_arm_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const lbot_arm_interfaces::msg::ArmState & msg)
{
  return lbot_arm_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<lbot_arm_interfaces::msg::ArmState>()
{
  return "lbot_arm_interfaces::msg::ArmState";
}

template<>
inline const char * name<lbot_arm_interfaces::msg::ArmState>()
{
  return "lbot_arm_interfaces/msg/ArmState";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::msg::ArmState>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lbot_arm_interfaces::msg::ArmState>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lbot_arm_interfaces::msg::ArmState>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__ARM_STATE__TRAITS_HPP_
