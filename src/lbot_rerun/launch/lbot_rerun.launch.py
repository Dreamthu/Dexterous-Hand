"""Start the read-only LinkerBot Rerun visualization bridge."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    share = get_package_share_directory("lbot_rerun")
    default_config = os.path.join(share, "config", "rerun.yaml")
    default_urdf = os.path.join(
        share, "assets", "workstations", "lkls73_i1_o6_bimanual", "workstation.urdf"
    )
    return LaunchDescription(
        [
            DeclareLaunchArgument("config", default_value=default_config),
            DeclareLaunchArgument("robot_namespace", default_value="robot1"),
            DeclareLaunchArgument("viewer_mode", default_value="spawn"),
            DeclareLaunchArgument("urdf_path", default_value=default_urdf),
            Node(
                package="lbot_rerun",
                executable="rerun_visualizer",
                name="rerun_visualizer",
                output="screen",
                emulate_tty=True,
                parameters=[
                    LaunchConfiguration("config"),
                    {
                        "robot_namespace": LaunchConfiguration("robot_namespace"),
                        "viewer_mode": LaunchConfiguration("viewer_mode"),
                        "urdf_path": LaunchConfiguration("urdf_path"),
                    },
                ],
            ),
        ]
    )
