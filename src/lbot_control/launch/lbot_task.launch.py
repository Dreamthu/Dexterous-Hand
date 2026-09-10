"""Start the driver, running vision detector, and connected nut task controller."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, RegisterEventHandler
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.events import Shutdown
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    driver_share = get_package_share_directory("lbot_driver")
    control_share = get_package_share_directory("lbot_control")
    vision_share = get_package_share_directory("lbot_vision")
    controller_node = Node(
        package="lbot_control", executable="nut_task_controller",
        parameters=[LaunchConfiguration("control_config"),
                    {"robot_namespace": LaunchConfiguration("robot_namespace"),
                     "execute_task": LaunchConfiguration("execute_task"),
                     "task_mode": LaunchConfiguration("task_mode")}],
        output="screen")

    return LaunchDescription([
        DeclareLaunchArgument("arm_ip", default_value="192.168.10.21"),
        DeclareLaunchArgument("robot_namespace", default_value="robot1"),
        DeclareLaunchArgument("start_driver", default_value="true"),
        DeclareLaunchArgument("execute_task", default_value="false"),
        DeclareLaunchArgument("task_mode", default_value="validate"),
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
        controller_node,
        RegisterEventHandler(
            OnProcessExit(
                target_action=controller_node,
                on_exit=[EmitEvent(event=Shutdown(reason="nut task controller finished"))])),
    ])
