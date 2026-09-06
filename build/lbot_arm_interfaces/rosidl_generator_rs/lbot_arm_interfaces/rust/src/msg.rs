#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to lbot_arm_interfaces__msg__ArmState

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ArmState {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: Vec<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub pose: geometry_msgs::msg::Pose,

}



impl Default for ArmState {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ArmState::default())
  }
}

impl rosidl_runtime_rs::Message for ArmState {
  type RmwMsg = super::msg::rmw::ArmState;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
    }
  }
}


// Corresponds to lbot_arm_interfaces__msg__LbotPose

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LbotPose {

    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,

}



impl Default for LbotPose {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::LbotPose::default())
  }
}

impl rosidl_runtime_rs::Message for LbotPose {
  type RmwMsg = super::msg::rmw::LbotPose;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
    }
  }
}


// Corresponds to lbot_arm_interfaces__msg__LbotFrame

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LbotFrame {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub euler: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,

}



impl Default for LbotFrame {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::LbotFrame::default())
  }
}

impl rosidl_runtime_rs::Message for LbotFrame {
  type RmwMsg = super::msg::rmw::LbotFrame;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.euler)).into_owned(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        euler: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.euler)).into_owned(),
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      euler: geometry_msgs::msg::Vector3::from_rmw_message(msg.euler),
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
    }
  }
}


// Corresponds to lbot_arm_interfaces__msg__FollowJoint

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct FollowJoint {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joints: Vec<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub follow: bool,

}



impl Default for FollowJoint {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::FollowJoint::default())
  }
}

impl rosidl_runtime_rs::Message for FollowJoint {
  type RmwMsg = super::msg::rmw::FollowJoint;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.into(),
        follow: msg.follow,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joints: msg.joints.as_slice().into(),
      follow: msg.follow,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joints: msg.joints
          .into_iter()
          .collect(),
      follow: msg.follow,
    }
  }
}


