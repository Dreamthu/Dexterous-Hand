#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to lbot_arm_interfaces__srv__MoveJ_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJ_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: Vec<f32>,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveJ_Request::default())
  }
}

impl rosidl_runtime_rs::Message for MoveJ_Request {
  type RmwMsg = super::srv::rmw::MoveJ_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
        speed: msg.speed,
        acce: msg.acce,
        block: msg.block,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveJ_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJ_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveJ_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveJ_Response::default())
  }
}

impl rosidl_runtime_rs::Message for MoveJ_Response {
  type RmwMsg = super::srv::rmw::MoveJ_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveL_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveL_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveL_Request::default())
  }
}

impl rosidl_runtime_rs::Message for MoveL_Request {
  type RmwMsg = super::srv::rmw::MoveL_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        speed: msg.speed,
        acce: msg.acce,
        block: msg.block,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveL_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveL_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveL_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveL_Response::default())
  }
}

impl rosidl_runtime_rs::Message for MoveL_Response {
  type RmwMsg = super::srv::rmw::MoveL_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveC_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveC_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveC_Request::default())
  }
}

impl rosidl_runtime_rs::Message for MoveC_Request {
  type RmwMsg = super::srv::rmw::MoveC_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        speed: msg.speed,
        acce: msg.acce,
        block: msg.block,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveC_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveC_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveC_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveC_Response::default())
  }
}

impl rosidl_runtime_rs::Message for MoveC_Response {
  type RmwMsg = super::srv::rmw::MoveC_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveJP_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJP_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveJP_Request::default())
  }
}

impl rosidl_runtime_rs::Message for MoveJP_Request {
  type RmwMsg = super::srv::rmw::MoveJP_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        speed: msg.speed,
        acce: msg.acce,
        block: msg.block,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      speed: msg.speed,
      acce: msg.acce,
      block: msg.block,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__MoveJP_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoveJP_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for MoveJP_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::MoveJP_Response::default())
  }
}

impl rosidl_runtime_rs::Message for MoveJP_Response {
  type RmwMsg = super::srv::rmw::MoveJP_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__InverseKinematics_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct InverseKinematics_Request {
    /// 此关节角度不设置会默认从机械臂读取当前角度，如果设置则基于此值为初始角度进行逆解
    pub joints: Vec<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,

}



impl Default for InverseKinematics_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::InverseKinematics_Request::default())
  }
}

impl rosidl_runtime_rs::Message for InverseKinematics_Request {
  type RmwMsg = super::srv::rmw::InverseKinematics_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__InverseKinematics_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct InverseKinematics_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: Vec<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for InverseKinematics_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::InverseKinematics_Response::default())
  }
}

impl rosidl_runtime_rs::Message for InverseKinematics_Response {
  type RmwMsg = super::srv::rmw::InverseKinematics_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__ForwardKinematics_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ForwardKinematics_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: Vec<f32>,

}



impl Default for ForwardKinematics_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ForwardKinematics_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ForwardKinematics_Request {
  type RmwMsg = super::srv::rmw::ForwardKinematics_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__ForwardKinematics_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ForwardKinematics_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for ForwardKinematics_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ForwardKinematics_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ForwardKinematics_Response {
  type RmwMsg = super::srv::rmw::ForwardKinematics_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetFrame_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::msg::LbotFrame,

}



impl Default for SetFrame_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetFrame_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetFrame_Request {
  type RmwMsg = super::srv::rmw::SetFrame_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Owned(msg.frame)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Borrowed(&msg.frame)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      frame: super::msg::LbotFrame::from_rmw_message(msg.frame),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetFrame_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetFrame_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetFrame_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetFrame_Response {
  type RmwMsg = super::srv::rmw::SetFrame_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetString_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetString_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,

}



impl Default for SetString_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetString_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetString_Request {
  type RmwMsg = super::srv::rmw::SetString_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetString_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetString_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetString_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetString_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetString_Response {
  type RmwMsg = super::srv::rmw::SetString_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetFrame_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,

}



impl Default for GetFrame_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetFrame_Request::default())
  }
}

impl rosidl_runtime_rs::Message for GetFrame_Request {
  type RmwMsg = super::srv::rmw::GetFrame_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetFrame_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::msg::LbotFrame,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetFrame_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetFrame_Response::default())
  }
}

impl rosidl_runtime_rs::Message for GetFrame_Response {
  type RmwMsg = super::srv::rmw::GetFrame_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Owned(msg.frame)).into_owned(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Borrowed(&msg.frame)).into_owned(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      frame: super::msg::LbotFrame::from_rmw_message(msg.frame),
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetCurrentFrame_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetCurrentFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for GetCurrentFrame_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetCurrentFrame_Request::default())
  }
}

impl rosidl_runtime_rs::Message for GetCurrentFrame_Request {
  type RmwMsg = super::srv::rmw::GetCurrentFrame_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetCurrentFrame_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetCurrentFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub frame: super::msg::LbotFrame,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetCurrentFrame_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetCurrentFrame_Response::default())
  }
}

impl rosidl_runtime_rs::Message for GetCurrentFrame_Response {
  type RmwMsg = super::srv::rmw::GetCurrentFrame_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Owned(msg.frame)).into_owned(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        frame: super::msg::LbotFrame::into_rmw_message(std::borrow::Cow::Borrowed(&msg.frame)).into_owned(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      frame: super::msg::LbotFrame::from_rmw_message(msg.frame),
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__ChangeFrame_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ChangeFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,

}



impl Default for ChangeFrame_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ChangeFrame_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ChangeFrame_Request {
  type RmwMsg = super::srv::rmw::ChangeFrame_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__ChangeFrame_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ChangeFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for ChangeFrame_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ChangeFrame_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ChangeFrame_Response {
  type RmwMsg = super::srv::rmw::ChangeFrame_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__DeleteFrame_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteFrame_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,

}



impl Default for DeleteFrame_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::DeleteFrame_Request::default())
  }
}

impl rosidl_runtime_rs::Message for DeleteFrame_Request {
  type RmwMsg = super::srv::rmw::DeleteFrame_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__DeleteFrame_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteFrame_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for DeleteFrame_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::DeleteFrame_Response::default())
  }
}

impl rosidl_runtime_rs::Message for DeleteFrame_Response {
  type RmwMsg = super::srv::rmw::DeleteFrame_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetAllFrames_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetAllFrames_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for GetAllFrames_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetAllFrames_Request::default())
  }
}

impl rosidl_runtime_rs::Message for GetAllFrames_Request {
  type RmwMsg = super::srv::rmw::GetAllFrames_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__GetAllFrames_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetAllFrames_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub names: Vec<std::string::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for GetAllFrames_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetAllFrames_Response::default())
  }
}

impl rosidl_runtime_rs::Message for GetAllFrames_Response {
  type RmwMsg = super::srv::rmw::GetAllFrames_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        names: msg.names
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        names: msg.names
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      names: msg.names
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetZero_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetZero_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for SetZero_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetZero_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetZero_Request {
  type RmwMsg = super::srv::rmw::SetZero_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetZero_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetZero_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetZero_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetZero_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetZero_Response {
  type RmwMsg = super::srv::rmw::SetZero_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetEmergency_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEmergency_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub emergency: bool,

}



impl Default for SetEmergency_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEmergency_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetEmergency_Request {
  type RmwMsg = super::srv::rmw::SetEmergency_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        emergency: msg.emergency,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      emergency: msg.emergency,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      emergency: msg.emergency,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetEmergency_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEmergency_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetEmergency_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEmergency_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetEmergency_Response {
  type RmwMsg = super::srv::rmw::SetEmergency_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetEnable_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEnable_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub enable: bool,

}



impl Default for SetEnable_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEnable_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetEnable_Request {
  type RmwMsg = super::srv::rmw::SetEnable_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        enable: msg.enable,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      enable: msg.enable,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      enable: msg.enable,
    }
  }
}


// Corresponds to lbot_arm_interfaces__srv__SetEnable_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEnable_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SetEnable_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEnable_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetEnable_Response {
  type RmwMsg = super::srv::rmw::SetEnable_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
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


