// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from lbot_arm_interfaces:msg/LbotFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/msg/lbot_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__TRAITS_HPP_
#define LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "lbot_arm_interfaces/msg/detail/lbot_frame__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'euler'
// Member 'position'
#include "geometry_msgs/msg/detail/vector3__traits.hpp"

namespace lbot_arm_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const LbotFrame & msg,
  std::ostream & out)
{
  out << "{";
  // member: name
  {
    out << "name: ";
    rosidl_generator_traits::value_to_yaml(msg.name, out);
    out << ", ";
  }

  // member: euler
  {
    out << "euler: ";
    to_flow_style_yaml(msg.euler, out);
    out << ", ";
  }

  // member: position
  {
    out << "position: ";
    to_flow_style_yaml(msg.position, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const LbotFrame & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: name
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "name: ";
    rosidl_generator_traits::value_to_yaml(msg.name, out);
    out << "\n";
  }

  // member: euler
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "euler:\n";
    to_block_style_yaml(msg.euler, out, indentation + 2);
  }

  // member: position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "position:\n";
    to_block_style_yaml(msg.position, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const LbotFrame & msg, bool use_flow_style = false)
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
  const lbot_arm_interfaces::msg::LbotFrame & msg,
  std::ostream & out, size_t indentation = 0)
{
  lbot_arm_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lbot_arm_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const lbot_arm_interfaces::msg::LbotFrame & msg)
{
  return lbot_arm_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<lbot_arm_interfaces::msg::LbotFrame>()
{
  return "lbot_arm_interfaces::msg::LbotFrame";
}

template<>
inline const char * name<lbot_arm_interfaces::msg::LbotFrame>()
{
  return "lbot_arm_interfaces/msg/LbotFrame";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::msg::LbotFrame>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lbot_arm_interfaces::msg::LbotFrame>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lbot_arm_interfaces::msg::LbotFrame>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // LBOT_ARM_INTERFACES__MSG__DETAIL__LBOT_FRAME__TRAITS_HPP_
