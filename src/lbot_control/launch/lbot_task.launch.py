"""Start the driver, vision detector, image viewer, and nut task controller."""

import os
import importlib.util
import yaml

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, RegisterEventHandler, OpaqueFunction
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.events import Shutdown
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def controller_actions(context):
    control_share = get_package_share_directory("lbot_control")
    config = LaunchConfiguration("control_config").perform(context)
    with open(config, encoding="utf-8") as stream:
        task = yaml.safe_load(stream)["/**"]["ros__parameters"]
    backend = LaunchConfiguration("slot_transfer_planner").perform(context) or task.get("slot_transfer_planner", "sdk")
    extra = {"slot_transfer_planner": backend}
    use_pointcloud = LaunchConfiguration("use_pointcloud").perform(context).strip().lower()
    if use_pointcloud:
        if use_pointcloud not in ("true", "false"):
            raise RuntimeError("use_pointcloud must be true, false, or empty to use control_config")
        extra["moveit_point_cloud_enabled"] = use_pointcloud == "true"
    if backend == "moveit":
        spec = importlib.util.spec_from_file_location("lbot_moveit_config", os.path.join(control_share, "python", "moveit_config.py"))
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        extra.update(module.parameters(task, LaunchConfiguration("moveit_model_source").perform(context)))
        for name in ("robot_description", "robot_description_semantic"):
            extra[name] = ParameterValue(extra[name], value_type=str)
    controller_node = Node(
        package="lbot_control", executable="nut_task_controller",
        parameters=[config, extra,
                    {"robot_namespace": LaunchConfiguration("robot_namespace"),
                     "execute_task": LaunchConfiguration("execute_task"),
                     "task_mode": LaunchConfiguration("task_mode")}],
        output="screen")
    return [controller_node, RegisterEventHandler(OnProcessExit(target_action=controller_node,
        on_exit=[EmitEvent(event=Shutdown(reason="nut task controller finished"))]))]


def generate_launch_description():
    driver_share = get_package_share_directory("lbot_driver")
    control_share = get_package_share_directory("lbot_control")
    vision_share = get_package_share_directory("lbot_vision")

    return LaunchDescription([
        DeclareLaunchArgument("arm_ip", default_value="192.168.10.21"),
        DeclareLaunchArgument("robot_namespace", default_value="robot1"),
        DeclareLaunchArgument("start_driver", default_value="true"),
        DeclareLaunchArgument("execute_task", default_value="false"),
        DeclareLaunchArgument("task_mode", default_value="validate"),
        DeclareLaunchArgument("slot_transfer_planner", default_value="",
                              description="moveit or sdk; empty uses control_config"),
        DeclareLaunchArgument("use_pointcloud", default_value="",
                              description="true/false overrides point-cloud obstacles for this run; empty uses control_config"),
        DeclareLaunchArgument("moveit_model_source", default_value="",
                              description="Optional path to Dexterous-Hand workstation.urdf"),
        DeclareLaunchArgument(
            "show_image", default_value="true",
            description="Open the live image viewer alongside the task"),
        DeclareLaunchArgument(
            "image_topic", default_value="/nut_detection/debug_image",
            description="Image topic to display; defaults to the detection overlay"),
        DeclareLaunchArgument(
            "control_config", default_value=os.path.join(control_share, "config", "nut_task.yaml")),
        DeclareLaunchArgument(
            "vision_config", default_value=os.path.join(vision_share, "config", "nut_detector.yaml")),
        Node(
            package="lbot_driver", executable="lbot_driver",
            namespace=LaunchConfiguration("robot_namespace"),
            parameters=[os.path.join(driver_share, "config", "lbot_config.yaml"),
                        {"arm_ip": LaunchConfiguration("arm_ip")}],
            condition=IfCondition(LaunchConfiguration("start_driver")),
            output="screen"),
        Node(
            package="lbot_vision", executable="nut_detector_node",
            parameters=[LaunchConfiguration("vision_config")],
            condition=IfCondition(LaunchConfiguration("execute_task")),
            output="screen"),
        Node(
            package="rqt_image_view", executable="rqt_image_view",
            arguments=[LaunchConfiguration("image_topic")],
            condition=IfCondition(LaunchConfiguration("show_image")),
            output="screen"),
        OpaqueFunction(function=controller_actions),
    ])
