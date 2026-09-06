from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='lbot_vision',
            executable='nut_detector_node',
            name='nut_detector_node',
            output='screen',
            parameters=[PathJoinSubstitution([
                FindPackageShare('lbot_vision'), 'config', 'nut_detector.yaml'
            ])],
        )
    ])
