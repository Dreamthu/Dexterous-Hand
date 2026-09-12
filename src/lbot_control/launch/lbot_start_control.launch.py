"""Start the configurable table-route motion node against an external robot adapter."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.events import Shutdown
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    control_share = get_package_share_directory("lbot_control")
    control_node = Node(
        package="lbot_control",
        executable="motion_bringup_node",
        parameters=[
            LaunchConfiguration("config_file"),
            {
                "robot_namespace": LaunchConfiguration("robot_namespace"),
                "execute_motion": LaunchConfiguration("execute_motion"),
                "mode": LaunchConfiguration("mode"),
            },
        ],
        output="screen",
        emulate_tty=True,
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "execute_motion",
                default_value="false",
                description="Allow real MoveJ commands",
            ),
            DeclareLaunchArgument(
                "mode",
                default_value="check",
                description="check, enter, leave, or round_trip",
            ),
            DeclareLaunchArgument(
                "config_file",
                default_value=os.path.join(control_share, "config", "nut_task.yaml"),
            ),
            control_node,
            RegisterEventHandler(
                OnProcessExit(
                    target_action=control_node,
                    on_exit=[
                        EmitEvent(
                            event=Shutdown(reason="motion bring-up node finished")
                        )
                    ],
                )
            ),
        ]
    )
