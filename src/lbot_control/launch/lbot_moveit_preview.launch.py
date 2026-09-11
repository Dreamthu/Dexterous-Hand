"""Offline MoveIt planning. Does not start a driver or send motion commands."""
import importlib.util
import os
import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def actions(context):
    share = get_package_share_directory("lbot_control")
    value = lambda name: LaunchConfiguration(name).perform(context)
    with open(value("control_config"), encoding="utf-8") as stream:
        task = yaml.safe_load(stream)["/**"]["ros__parameters"]
    spec = importlib.util.spec_from_file_location("lbot_moveit_config", os.path.join(share, "python", "moveit_config.py"))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    config = module.parameters(task, value("moveit_model_source"))
    for name in ("robot_description", "robot_description_semantic"):
        config[name] = ParameterValue(config[name], value_type=str)
    extra = {
        "preview_start_joints": [float(v) for v in yaml.safe_load(value("start_joints"))],
        "preview_goal_pose": [float(v) for v in yaml.safe_load(value("goal_pose"))],
        "preview_orientation_tolerance_rad": float(value("orientation_tolerance_rad") or task.get("approach_place_max_orientation_change_rad", .35)),
        "preview_timeout_s": float(value("timeout_s")),
        "preview_output": value("output"),
        "preview_capture_cloud": value("use_pointcloud").lower() == "true",
        "moveit_point_cloud_enabled": value("use_pointcloud").lower() == "true",
    }
    slot = yaml.safe_load(value("slot_xy"))
    if slot:
        extra["preview_slot_xy"] = [float(v) for v in slot]
    return [Node(package="lbot_control", executable="moveit_transfer_preview",
                 parameters=[value("control_config"), config, extra], output="screen")]


def generate_launch_description():
    share = get_package_share_directory("lbot_control")
    return LaunchDescription([
        DeclareLaunchArgument("control_config", default_value=os.path.join(share, "config", "nut_task.yaml")),
        DeclareLaunchArgument("moveit_model_source", default_value=""),
        DeclareLaunchArgument("start_joints", description="Seven controller joint angles, radians"),
        DeclareLaunchArgument("goal_pose", description="Arm_Tip [x,y,z,roll,pitch,yaw] in base_link"),
        DeclareLaunchArgument("slot_xy", default_value="[]", description="Optional palm target XY; preserves goal_pose Z/RPY reference"),
        DeclareLaunchArgument("orientation_tolerance_rad", default_value="", description="Empty uses the task YAML bound"),
        DeclareLaunchArgument("timeout_s", default_value="15.0"),
        DeclareLaunchArgument("use_pointcloud", default_value="false",
                              description="Read the live cloud with the supplied start joints; no robot commands"),
        DeclareLaunchArgument("output", default_value="/tmp/lbot_moveit_trajectory.csv"),
        OpaqueFunction(function=actions),
    ])
