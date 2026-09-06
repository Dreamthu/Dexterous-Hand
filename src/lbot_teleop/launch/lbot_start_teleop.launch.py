import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    robots = ["robot1", ]

    nodes = []

    for robot in robots:
        nodes.append(
            Node(
                package="lbot_teleop",
                executable="demo_teleop",
                name="demo_teleop_node",
                namespace=robot,
                output="screen",
            )
        )

    return LaunchDescription(nodes)

