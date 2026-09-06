"""Start the driver and execute a small, measured-pose motion test."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    driver_config = os.path.join(
        get_package_share_directory("lbot_driver"), "config", "lbot_config.yaml"
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "arm_ip",
                default_value="192.168.10.21",
                description="Robot controller IP address",
            ),
            Node(
                package="lbot_driver",
                executable="lbot_driver",
                namespace="robot1",
                parameters=[driver_config, {"arm_ip": LaunchConfiguration("arm_ip")}],
                output="screen",
                emulate_tty=True,
            ),
            Node(
                package="lbot_demo",
                executable="demo_motion_test",
                namespace="robot1",
                output="screen",
                emulate_tty=True,
            ),
        ]
    )
