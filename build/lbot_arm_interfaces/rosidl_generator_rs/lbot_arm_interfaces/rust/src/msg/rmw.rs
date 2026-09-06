#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__ArmState() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__msg__ArmState__init(msg: *mut ArmState) -> bool;
    fn lbot_arm_interfaces__msg__ArmState__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ArmState>, size: usize) -> bool;
    fn lbot_arm_interfaces__msg__ArmState__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ArmState>);
    fn lbot_arm_interfaces__msg__ArmState__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ArmState>, out_seq: *mut rosidl_runtime_rs::Sequence<ArmState>) -> bool;
}

// Corresponds to lbot_arm_interfaces__msg__ArmState
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ArmState {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub pose: geometry_msgs::msg::rmw::Pose,

}



impl Default for ArmState {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__msg__ArmState__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__msg__ArmState__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ArmState {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__ArmState__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__ArmState__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__ArmState__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ArmState {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ArmState where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/msg/ArmState";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__ArmState() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__LbotPose() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__msg__LbotPose__init(msg: *mut LbotPose) -> bool;
    fn lbot_arm_interfaces__msg__LbotPose__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<LbotPose>, size: usize) -> bool;
    fn lbot_arm_interfaces__msg__LbotPose__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<LbotPose>);
    fn lbot_arm_interfaces__msg__LbotPose__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<LbotPose>, out_seq: *mut rosidl_runtime_rs::Sequence<LbotPose>) -> bool;
}

// Corresponds to lbot_arm_interfaces__msg__LbotPose
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LbotPose {

    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,

}



impl Default for LbotPose {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__msg__LbotPose__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__msg__LbotPose__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for LbotPose {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotPose__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotPose__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotPose__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for LbotPose {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for LbotPose where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/msg/LbotPose";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__LbotPose() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__LbotFrame() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__msg__LbotFrame__init(msg: *mut LbotFrame) -> bool;
    fn lbot_arm_interfaces__msg__LbotFrame__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<LbotFrame>, size: usize) -> bool;
    fn lbot_arm_interfaces__msg__LbotFrame__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<LbotFrame>);
    fn lbot_arm_interfaces__msg__LbotFrame__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<LbotFrame>, out_seq: *mut rosidl_runtime_rs::Sequence<LbotFrame>) -> bool;
}

// Corresponds to lbot_arm_interfaces__msg__LbotFrame
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LbotFrame {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,

}



impl Default for LbotFrame {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__msg__LbotFrame__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__msg__LbotFrame__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for LbotFrame {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotFrame__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotFrame__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__LbotFrame__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for LbotFrame {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for LbotFrame where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/msg/LbotFrame";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__LbotFrame() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__FollowJoint() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__msg__FollowJoint__init(msg: *mut FollowJoint) -> bool;
    fn lbot_arm_interfaces__msg__FollowJoint__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<FollowJoint>, size: usize) -> bool;
    fn lbot_arm_interfaces__msg__FollowJoint__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<FollowJoint>);
    fn lbot_arm_interfaces__msg__FollowJoint__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<FollowJoint>, out_seq: *mut rosidl_runtime_rs::Sequence<FollowJoint>) -> bool;
}

// Corresponds to lbot_arm_interfaces__msg__FollowJoint
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct FollowJoint {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub follow: bool,

}



impl Default for FollowJoint {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__msg__FollowJoint__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__msg__FollowJoint__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for FollowJoint {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__FollowJoint__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__FollowJoint__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__msg__FollowJoint__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for FollowJoint {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for FollowJoint where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/msg/FollowJoint";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__msg__FollowJoint() }
  }
}


