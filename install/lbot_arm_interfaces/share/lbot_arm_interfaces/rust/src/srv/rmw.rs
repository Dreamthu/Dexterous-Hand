#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJ_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveJ_Request__init(msg: *mut MoveJ_Request) -> bool;
    fn lbot_arm_interfaces__srv__MoveJ_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveJ_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Request>);
    fn lbot_arm_interfaces__srv__MoveJ_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveJ_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJ_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJ_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub speed: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub acce: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub block: bool,

}



impl Default for MoveJ_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveJ_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveJ_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveJ_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveJ_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveJ_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveJ_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJ_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJ_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveJ_Response__init(msg: *mut MoveJ_Response) -> bool;
    fn lbot_arm_interfaces__srv__MoveJ_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveJ_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Response>);
    fn lbot_arm_interfaces__srv__MoveJ_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveJ_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveJ_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJ_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJ_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveJ_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveJ_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveJ_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveJ_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJ_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveJ_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveJ_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveJ_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJ_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveL_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveL_Request__init(msg: *mut MoveL_Request) -> bool;
    fn lbot_arm_interfaces__srv__MoveL_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveL_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveL_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveL_Request>);
    fn lbot_arm_interfaces__srv__MoveL_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveL_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveL_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveL_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveL_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub speed: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub acce: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub block: bool,

}



impl Default for MoveL_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveL_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveL_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveL_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveL_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveL_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveL_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveL_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveL_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveL_Response__init(msg: *mut MoveL_Response) -> bool;
    fn lbot_arm_interfaces__srv__MoveL_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveL_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveL_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveL_Response>);
    fn lbot_arm_interfaces__srv__MoveL_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveL_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveL_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveL_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveL_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveL_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveL_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveL_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveL_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveL_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveL_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveL_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveL_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveL_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveC_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveC_Request__init(msg: *mut MoveC_Request) -> bool;
    fn lbot_arm_interfaces__srv__MoveC_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveC_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveC_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveC_Request>);
    fn lbot_arm_interfaces__srv__MoveC_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveC_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveC_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveC_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveC_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub speed: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub acce: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub block: bool,

}



impl Default for MoveC_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveC_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveC_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveC_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveC_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveC_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveC_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveC_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveC_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveC_Response__init(msg: *mut MoveC_Response) -> bool;
    fn lbot_arm_interfaces__srv__MoveC_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveC_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveC_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveC_Response>);
    fn lbot_arm_interfaces__srv__MoveC_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveC_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveC_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveC_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveC_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveC_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveC_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveC_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveC_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveC_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveC_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveC_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveC_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveC_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJP_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveJP_Request__init(msg: *mut MoveJP_Request) -> bool;
    fn lbot_arm_interfaces__srv__MoveJP_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveJP_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Request>);
    fn lbot_arm_interfaces__srv__MoveJP_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveJP_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJP_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJP_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub speed: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub acce: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub block: bool,

}



impl Default for MoveJP_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveJP_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveJP_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveJP_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveJP_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveJP_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveJP_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJP_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJP_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__MoveJP_Response__init(msg: *mut MoveJP_Response) -> bool;
    fn lbot_arm_interfaces__srv__MoveJP_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__MoveJP_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Response>);
    fn lbot_arm_interfaces__srv__MoveJP_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoveJP_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoveJP_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJP_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJP_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveJP_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__MoveJP_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__MoveJP_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoveJP_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__MoveJP_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoveJP_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoveJP_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/MoveJP_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__MoveJP_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__InverseKinematics_Request__init(msg: *mut InverseKinematics_Request) -> bool;
    fn lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Request>);
    fn lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<InverseKinematics_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__InverseKinematics_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct InverseKinematics_Request {
    /// 此关节角度不设置会默认从机械臂读取当前角度，如果设置则基于此值为初始角度进行逆解
    pub joints: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,

}



impl Default for InverseKinematics_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__InverseKinematics_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__InverseKinematics_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for InverseKinematics_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for InverseKinematics_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for InverseKinematics_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/InverseKinematics_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__InverseKinematics_Response__init(msg: *mut InverseKinematics_Response) -> bool;
    fn lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Response>);
    fn lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<InverseKinematics_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<InverseKinematics_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__InverseKinematics_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct InverseKinematics_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for InverseKinematics_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__InverseKinematics_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__InverseKinematics_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for InverseKinematics_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__InverseKinematics_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for InverseKinematics_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for InverseKinematics_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/InverseKinematics_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__ForwardKinematics_Request__init(msg: *mut ForwardKinematics_Request) -> bool;
    fn lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Request>);
    fn lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ForwardKinematics_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__ForwardKinematics_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ForwardKinematics_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: rosidl_runtime_rs::Sequence<f32>,

}



impl Default for ForwardKinematics_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__ForwardKinematics_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__ForwardKinematics_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ForwardKinematics_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ForwardKinematics_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ForwardKinematics_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/ForwardKinematics_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__ForwardKinematics_Response__init(msg: *mut ForwardKinematics_Response) -> bool;
    fn lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Response>);
    fn lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ForwardKinematics_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ForwardKinematics_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__ForwardKinematics_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ForwardKinematics_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for ForwardKinematics_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__ForwardKinematics_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__ForwardKinematics_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ForwardKinematics_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ForwardKinematics_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ForwardKinematics_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ForwardKinematics_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/ForwardKinematics_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetFrame_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetFrame_Request__init(msg: *mut SetFrame_Request) -> bool;
    fn lbot_arm_interfaces__srv__SetFrame_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetFrame_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Request>);
    fn lbot_arm_interfaces__srv__SetFrame_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetFrame_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetFrame_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::super::msg::rmw::LbotFrame,

}



impl Default for SetFrame_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetFrame_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetFrame_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetFrame_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetFrame_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetFrame_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetFrame_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetFrame_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetFrame_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetFrame_Response__init(msg: *mut SetFrame_Response) -> bool;
    fn lbot_arm_interfaces__srv__SetFrame_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetFrame_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Response>);
    fn lbot_arm_interfaces__srv__SetFrame_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetFrame_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetFrame_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetFrame_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetFrame_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetFrame_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetFrame_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetFrame_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetFrame_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetFrame_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetFrame_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetFrame_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetFrame_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetString_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetString_Request__init(msg: *mut SetString_Request) -> bool;
    fn lbot_arm_interfaces__srv__SetString_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetString_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetString_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetString_Request>);
    fn lbot_arm_interfaces__srv__SetString_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetString_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetString_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetString_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetString_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,

}



impl Default for SetString_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetString_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetString_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetString_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetString_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetString_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetString_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetString_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetString_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetString_Response__init(msg: *mut SetString_Response) -> bool;
    fn lbot_arm_interfaces__srv__SetString_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetString_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetString_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetString_Response>);
    fn lbot_arm_interfaces__srv__SetString_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetString_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetString_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetString_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetString_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetString_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetString_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetString_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetString_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetString_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetString_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetString_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetString_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetString_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetFrame_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetFrame_Request__init(msg: *mut GetFrame_Request) -> bool;
    fn lbot_arm_interfaces__srv__GetFrame_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetFrame_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Request>);
    fn lbot_arm_interfaces__srv__GetFrame_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetFrame_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetFrame_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,

}



impl Default for GetFrame_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetFrame_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetFrame_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetFrame_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetFrame_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetFrame_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetFrame_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetFrame_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetFrame_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetFrame_Response__init(msg: *mut GetFrame_Response) -> bool;
    fn lbot_arm_interfaces__srv__GetFrame_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetFrame_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Response>);
    fn lbot_arm_interfaces__srv__GetFrame_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetFrame_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<GetFrame_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetFrame_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::super::msg::rmw::LbotFrame,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetFrame_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetFrame_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetFrame_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetFrame_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetFrame_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetFrame_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetFrame_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetFrame_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetFrame_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Request__init(msg: *mut GetCurrentFrame_Request) -> bool;
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Request>);
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetCurrentFrame_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetCurrentFrame_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetCurrentFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for GetCurrentFrame_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetCurrentFrame_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetCurrentFrame_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetCurrentFrame_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetCurrentFrame_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetCurrentFrame_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetCurrentFrame_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Response__init(msg: *mut GetCurrentFrame_Response) -> bool;
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Response>);
    fn lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetCurrentFrame_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<GetCurrentFrame_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetCurrentFrame_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetCurrentFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::super::msg::rmw::LbotFrame,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetCurrentFrame_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetCurrentFrame_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetCurrentFrame_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetCurrentFrame_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetCurrentFrame_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetCurrentFrame_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetCurrentFrame_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetCurrentFrame_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__ChangeFrame_Request__init(msg: *mut ChangeFrame_Request) -> bool;
    fn lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Request>);
    fn lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ChangeFrame_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__ChangeFrame_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ChangeFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,

}



impl Default for ChangeFrame_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__ChangeFrame_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__ChangeFrame_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ChangeFrame_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ChangeFrame_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ChangeFrame_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/ChangeFrame_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__ChangeFrame_Response__init(msg: *mut ChangeFrame_Response) -> bool;
    fn lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Response>);
    fn lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ChangeFrame_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ChangeFrame_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__ChangeFrame_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ChangeFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for ChangeFrame_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__ChangeFrame_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__ChangeFrame_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ChangeFrame_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__ChangeFrame_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ChangeFrame_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ChangeFrame_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/ChangeFrame_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__DeleteFrame_Request__init(msg: *mut DeleteFrame_Request) -> bool;
    fn lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Request>);
    fn lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DeleteFrame_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__DeleteFrame_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,

}



impl Default for DeleteFrame_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__DeleteFrame_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__DeleteFrame_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DeleteFrame_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DeleteFrame_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DeleteFrame_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/DeleteFrame_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__DeleteFrame_Response__init(msg: *mut DeleteFrame_Response) -> bool;
    fn lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Response>);
    fn lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DeleteFrame_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<DeleteFrame_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__DeleteFrame_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for DeleteFrame_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__DeleteFrame_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__DeleteFrame_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DeleteFrame_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__DeleteFrame_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DeleteFrame_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DeleteFrame_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/DeleteFrame_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetAllFrames_Request__init(msg: *mut GetAllFrames_Request) -> bool;
    fn lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Request>);
    fn lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetAllFrames_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetAllFrames_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetAllFrames_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for GetAllFrames_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetAllFrames_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetAllFrames_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetAllFrames_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetAllFrames_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetAllFrames_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetAllFrames_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__GetAllFrames_Response__init(msg: *mut GetAllFrames_Response) -> bool;
    fn lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Response>);
    fn lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetAllFrames_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<GetAllFrames_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__GetAllFrames_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetAllFrames_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub names: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetAllFrames_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__GetAllFrames_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__GetAllFrames_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetAllFrames_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__GetAllFrames_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetAllFrames_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetAllFrames_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/GetAllFrames_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetZero_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetZero_Request__init(msg: *mut SetZero_Request) -> bool;
    fn lbot_arm_interfaces__srv__SetZero_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetZero_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetZero_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetZero_Request>);
    fn lbot_arm_interfaces__srv__SetZero_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetZero_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetZero_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetZero_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetZero_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for SetZero_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetZero_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetZero_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetZero_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetZero_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetZero_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetZero_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetZero_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetZero_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetZero_Response__init(msg: *mut SetZero_Response) -> bool;
    fn lbot_arm_interfaces__srv__SetZero_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetZero_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetZero_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetZero_Response>);
    fn lbot_arm_interfaces__srv__SetZero_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetZero_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetZero_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetZero_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetZero_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetZero_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetZero_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetZero_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetZero_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetZero_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetZero_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetZero_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetZero_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetZero_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEmergency_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetEmergency_Request__init(msg: *mut SetEmergency_Request) -> bool;
    fn lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Request>);
    fn lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEmergency_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetEmergency_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEmergency_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub emergency: bool,

}



impl Default for SetEmergency_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetEmergency_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetEmergency_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEmergency_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEmergency_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEmergency_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetEmergency_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEmergency_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEmergency_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetEmergency_Response__init(msg: *mut SetEmergency_Response) -> bool;
    fn lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Response>);
    fn lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEmergency_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEmergency_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetEmergency_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEmergency_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetEmergency_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetEmergency_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetEmergency_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEmergency_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEmergency_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEmergency_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEmergency_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetEmergency_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEmergency_Response() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEnable_Request() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetEnable_Request__init(msg: *mut SetEnable_Request) -> bool;
    fn lbot_arm_interfaces__srv__SetEnable_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Request>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetEnable_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Request>);
    fn lbot_arm_interfaces__srv__SetEnable_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEnable_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Request>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetEnable_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEnable_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub enable: bool,

}



impl Default for SetEnable_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetEnable_Request__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetEnable_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEnable_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEnable_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEnable_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetEnable_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEnable_Request() }
  }
}


#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEnable_Response() -> *const std::ffi::c_void;
}

#[link(name = "lbot_arm_interfaces__rosidl_generator_c")]
extern "C" {
    fn lbot_arm_interfaces__srv__SetEnable_Response__init(msg: *mut SetEnable_Response) -> bool;
    fn lbot_arm_interfaces__srv__SetEnable_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Response>, size: usize) -> bool;
    fn lbot_arm_interfaces__srv__SetEnable_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Response>);
    fn lbot_arm_interfaces__srv__SetEnable_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEnable_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEnable_Response>) -> bool;
}

// Corresponds to lbot_arm_interfaces__srv__SetEnable_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEnable_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetEnable_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lbot_arm_interfaces__srv__SetEnable_Response__init(&mut msg as *mut _) {
        panic!("Call to lbot_arm_interfaces__srv__SetEnable_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEnable_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lbot_arm_interfaces__srv__SetEnable_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEnable_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEnable_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lbot_arm_interfaces/srv/SetEnable_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lbot_arm_interfaces__srv__SetEnable_Response() }
  }
}






#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveJ() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJ
#[allow(missing_docs, non_camel_case_types)]
pub struct MoveJ;

impl rosidl_runtime_rs::Service for MoveJ {
    type Request = MoveJ_Request;
    type Response = MoveJ_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveJ() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveL() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__MoveL
#[allow(missing_docs, non_camel_case_types)]
pub struct MoveL;

impl rosidl_runtime_rs::Service for MoveL {
    type Request = MoveL_Request;
    type Response = MoveL_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveL() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveC() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__MoveC
#[allow(missing_docs, non_camel_case_types)]
pub struct MoveC;

impl rosidl_runtime_rs::Service for MoveC {
    type Request = MoveC_Request;
    type Response = MoveC_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveC() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveJP() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__MoveJP
#[allow(missing_docs, non_camel_case_types)]
pub struct MoveJP;

impl rosidl_runtime_rs::Service for MoveJP {
    type Request = MoveJP_Request;
    type Response = MoveJP_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__MoveJP() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__InverseKinematics
#[allow(missing_docs, non_camel_case_types)]
pub struct InverseKinematics;

impl rosidl_runtime_rs::Service for InverseKinematics {
    type Request = InverseKinematics_Request;
    type Response = InverseKinematics_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__InverseKinematics() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__ForwardKinematics
#[allow(missing_docs, non_camel_case_types)]
pub struct ForwardKinematics;

impl rosidl_runtime_rs::Service for ForwardKinematics {
    type Request = ForwardKinematics_Request;
    type Response = ForwardKinematics_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__ForwardKinematics() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetFrame() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__SetFrame
#[allow(missing_docs, non_camel_case_types)]
pub struct SetFrame;

impl rosidl_runtime_rs::Service for SetFrame {
    type Request = SetFrame_Request;
    type Response = SetFrame_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetFrame() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetString() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__SetString
#[allow(missing_docs, non_camel_case_types)]
pub struct SetString;

impl rosidl_runtime_rs::Service for SetString {
    type Request = SetString_Request;
    type Response = SetString_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetString() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetFrame() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__GetFrame
#[allow(missing_docs, non_camel_case_types)]
pub struct GetFrame;

impl rosidl_runtime_rs::Service for GetFrame {
    type Request = GetFrame_Request;
    type Response = GetFrame_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetFrame() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__GetCurrentFrame
#[allow(missing_docs, non_camel_case_types)]
pub struct GetCurrentFrame;

impl rosidl_runtime_rs::Service for GetCurrentFrame {
    type Request = GetCurrentFrame_Request;
    type Response = GetCurrentFrame_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetCurrentFrame() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__ChangeFrame
#[allow(missing_docs, non_camel_case_types)]
pub struct ChangeFrame;

impl rosidl_runtime_rs::Service for ChangeFrame {
    type Request = ChangeFrame_Request;
    type Response = ChangeFrame_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__ChangeFrame() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__DeleteFrame
#[allow(missing_docs, non_camel_case_types)]
pub struct DeleteFrame;

impl rosidl_runtime_rs::Service for DeleteFrame {
    type Request = DeleteFrame_Request;
    type Response = DeleteFrame_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__DeleteFrame() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__GetAllFrames
#[allow(missing_docs, non_camel_case_types)]
pub struct GetAllFrames;

impl rosidl_runtime_rs::Service for GetAllFrames {
    type Request = GetAllFrames_Request;
    type Response = GetAllFrames_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__GetAllFrames() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetZero() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__SetZero
#[allow(missing_docs, non_camel_case_types)]
pub struct SetZero;

impl rosidl_runtime_rs::Service for SetZero {
    type Request = SetZero_Request;
    type Response = SetZero_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetZero() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetEmergency() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__SetEmergency
#[allow(missing_docs, non_camel_case_types)]
pub struct SetEmergency;

impl rosidl_runtime_rs::Service for SetEmergency {
    type Request = SetEmergency_Request;
    type Response = SetEmergency_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetEmergency() }
    }
}




#[link(name = "lbot_arm_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetEnable() -> *const std::ffi::c_void;
}

// Corresponds to lbot_arm_interfaces__srv__SetEnable
#[allow(missing_docs, non_camel_case_types)]
pub struct SetEnable;

impl rosidl_runtime_rs::Service for SetEnable {
    type Request = SetEnable_Request;
    type Response = SetEnable_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lbot_arm_interfaces__srv__SetEnable() }
    }
}


