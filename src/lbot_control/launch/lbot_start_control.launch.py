"""Start the arm driver and the configurable table-route motion node."""

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
    driver_node = Node(
        package="lbot_driver",
        executable="lbot_driver",
        namespace=LaunchConfiguration("robot_namespace"),
        parameters=[
            os.path.join(driver_share, "config", "lbot_config.yaml"),
            {"arm_ip": LaunchConfiguration("arm_ip")},
        ],
        condition=IfCondition(LaunchConfiguration("start_driver")),
        output="screen",
        emulate_tty=True,
    )
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
                "arm_ip", default_value="192.168.10.21", description="Robot controller IP"
            ),
            DeclareLaunchArgument(
                "robot_namespace", default_value="robot1", description="ROS robot namespace"
            ),
            DeclareLaunchArgument(
                "start_driver", default_value="true", description="Start lbot_driver"
            ),
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
            driver_node,
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
