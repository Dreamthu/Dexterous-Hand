from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='lbot_vision',
            executable='nut_task_node',
            name='nut_task_node',
            output='screen',
            parameters=[PathJoinSubstitution([
                FindPackageShare('lbot_vision'), 'config', 'nut_task.yaml'
            ])],
        )
    ])
