"""Start only the vision detector and an optional image viewer for
offline tuning — no driver, no motion, no task controller."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    vision_share = get_package_share_directory("lbot_vision")

    return LaunchDescription([
        DeclareLaunchArgument(
            "vision_config",
            default_value=os.path.join(vision_share, "config", "nut_detector.yaml"),
            description="Path to nut_detector YAML config"),
        DeclareLaunchArgument(
            "show_image", default_value="true",
            description="Open rqt_image_view for the debug overlay"),
        DeclareLaunchArgument(
            "image_topic", default_value="/nut_detection/debug_image",
            description="Image topic to display"),

        Node(
            package="lbot_vision", executable="nut_detector_node",
            parameters=[LaunchConfiguration("vision_config")],
            output="screen"),

        Node(
            package="rqt_image_view", executable="rqt_image_view",
            arguments=[LaunchConfiguration("image_topic")],
            condition=IfCondition(LaunchConfiguration("show_image")),
            output="screen"),
    ])
