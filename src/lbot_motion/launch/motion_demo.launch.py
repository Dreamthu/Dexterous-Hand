from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("motion", default_value="extend"),
        DeclareLaunchArgument("arm", default_value="right"),
        DeclareLaunchArgument("robot_namespace", default_value="robot1"),
        DeclareLaunchArgument("speed", default_value="0.5"),
        DeclareLaunchArgument("acce", default_value="0.5"),
        DeclareLaunchArgument("state_timeout", default_value="15.0"),
        DeclareLaunchArgument("service_timeout", default_value="10.0"),
        DeclareLaunchArgument("hand_speed", default_value="250"),
        DeclareLaunchArgument("hand_force", default_value="250"),
        DeclareLaunchArgument("hand_close_position", default_value="0"),
        DeclareLaunchArgument("hand_open_position", default_value="255"),
        DeclareLaunchArgument("hand_enabled", default_value="true"),
        Node(
            package="lbot_motion",
            executable="motion_demo_node",
            namespace=LaunchConfiguration("robot_namespace"),
            parameters=[{
                "motion": LaunchConfiguration("motion"),
                "arm": LaunchConfiguration("arm"),
                "speed": LaunchConfiguration("speed"),
                "acce": LaunchConfiguration("acce"),
                "state_timeout": LaunchConfiguration("state_timeout"),
                "service_timeout": LaunchConfiguration("service_timeout"),
                "hand_speed": LaunchConfiguration("hand_speed"),
                "hand_force": LaunchConfiguration("hand_force"),
                "hand_close_position": LaunchConfiguration("hand_close_position"),
                "hand_open_position": LaunchConfiguration("hand_open_position"),
                "hand_enabled": LaunchConfiguration("hand_enabled"),
            }],
            output="screen",
            emulate_tty=True,
        ),
    ])
