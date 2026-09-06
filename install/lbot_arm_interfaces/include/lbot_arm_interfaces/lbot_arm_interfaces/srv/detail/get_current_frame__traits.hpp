// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from lbot_arm_interfaces:srv/GetCurrentFrame.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lbot_arm_interfaces/srv/get_current_frame.hpp"


#ifndef LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__TRAITS_HPP_
#define LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "lbot_arm_interfaces/srv/detail/get_current_frame__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace lbot_arm_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetCurrentFrame_Request & msg,
  std::ostream & out)
{
  (void)msg;
  out << "null";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GetCurrentFrame_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  (void)msg;
  (void)indentation;
  out << "null\n";
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetCurrentFrame_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use lbot_arm_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lbot_arm_interfaces::srv::GetCurrentFrame_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  lbot_arm_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lbot_arm_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const lbot_arm_interfaces::srv::GetCurrentFrame_Request & msg)
{
  return lbot_arm_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lbot_arm_interfaces::srv::GetCurrentFrame_Request>()
{
  return "lbot_arm_interfaces::srv::GetCurrentFrame_Request";
}

template<>
inline const char * name<lbot_arm_interfaces::srv::GetCurrentFrame_Request>()
{
  return "lbot_arm_interfaces/srv/GetCurrentFrame_Request";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<lbot_arm_interfaces::srv::GetCurrentFrame_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'frame'
#include "lbot_arm_interfaces/msg/detail/lbot_frame__traits.hpp"

namespace lbot_arm_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetCurrentFrame_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: name
  {
    out << "name: ";
    rosidl_generator_traits::value_to_yaml(msg.name, out);
    out << ", ";
  }

  // member: frame
  {
    out << "frame: ";
    to_flow_style_yaml(msg.frame, out);
    out << ", ";
  }

  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GetCurrentFrame_Response & msg,
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

  // member: frame
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "frame:\n";
    to_block_style_yaml(msg.frame, out, indentation + 2);
  }

  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetCurrentFrame_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use lbot_arm_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lbot_arm_interfaces::srv::GetCurrentFrame_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  lbot_arm_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lbot_arm_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const lbot_arm_interfaces::srv::GetCurrentFrame_Response & msg)
{
  return lbot_arm_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lbot_arm_interfaces::srv::GetCurrentFrame_Response>()
{
  return "lbot_arm_interfaces::srv::GetCurrentFrame_Response";
}

template<>
inline const char * name<lbot_arm_interfaces::srv::GetCurrentFrame_Response>()
{
  return "lbot_arm_interfaces/srv/GetCurrentFrame_Response";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lbot_arm_interfaces::srv::GetCurrentFrame_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace lbot_arm_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetCurrentFrame_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GetCurrentFrame_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetCurrentFrame_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lbot_arm_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use lbot_arm_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lbot_arm_interfaces::srv::GetCurrentFrame_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  lbot_arm_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lbot_arm_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const lbot_arm_interfaces::srv::GetCurrentFrame_Event & msg)
{
  return lbot_arm_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lbot_arm_interfaces::srv::GetCurrentFrame_Event>()
{
  return "lbot_arm_interfaces::srv::GetCurrentFrame_Event";
}

template<>
inline const char * name<lbot_arm_interfaces::srv::GetCurrentFrame_Event>()
{
  return "lbot_arm_interfaces/srv/GetCurrentFrame_Event";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Event>
  : std::integral_constant<bool, has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Request>::value && has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<lbot_arm_interfaces::srv::GetCurrentFrame_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<lbot_arm_interfaces::srv::GetCurrentFrame>()
{
  return "lbot_arm_interfaces::srv::GetCurrentFrame";
}

template<>
inline const char * name<lbot_arm_interfaces::srv::GetCurrentFrame>()
{
  return "lbot_arm_interfaces/srv/GetCurrentFrame";
}

template<>
struct has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame>
  : std::integral_constant<
    bool,
    has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame_Request>::value &&
    has_fixed_size<lbot_arm_interfaces::srv::GetCurrentFrame_Response>::value
  >
{
};

template<>
struct has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame>
  : std::integral_constant<
    bool,
    has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Request>::value &&
    has_bounded_size<lbot_arm_interfaces::srv::GetCurrentFrame_Response>::value
  >
{
};

template<>
struct is_service<lbot_arm_interfaces::srv::GetCurrentFrame>
  : std::true_type
{
};

template<>
struct is_service_request<lbot_arm_interfaces::srv::GetCurrentFrame_Request>
  : std::true_type
{
};

template<>
struct is_service_response<lbot_arm_interfaces::srv::GetCurrentFrame_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // LBOT_ARM_INTERFACES__SRV__DETAIL__GET_CURRENT_FRAME__TRAITS_HPP_
