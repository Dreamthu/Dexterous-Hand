import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    robots = ["robot1", ]

    nodes = []

    for robot in robots:
        nodes.append(
            Node(
                package="lbot_demo",

                # executable="demo_lkrs_movej",
                # name="demo_lkrs_movej_node",

                # executable="demo_lkrs_movejp",
                # name="demo_lkrs_movejp_node",

                # executable="demo_lkrs_movel",
                # name="demo_lkrs_movel_node",

                # executable="demo_lkls_movej",
                # name="demo_lkls_movej_node",

                # executable="demo_lkls_movejp",
                # name="demo_lkls_movejp_node",

                # executable="demo_lkls_movel",
                # name="demo_lkls_movel_node",

                executable="demo_lkls_i1_movej",
                name="demo_lkls_i1_movej_node",

                # executable="demo_lkls_i1_movejp",
                # name="demo_lkls_i1_movejp_node",

                # executable="demo_lkls_i1_movel",
                # name="demo_lkls_i1_movel_node",
                
                # executable="demo_fk",
                # name="fk_client_node",

                # executable="demo_ik",
                # name="ik_service_client",

                # executable="demo_tool_frame",
                # name="demo_tool_frame",

                # executable="demo_hand_l6_control",
                # name="demo_hand_l6_control",

                # executable="demo_hand_l10_control",
                # name="demo_hand_l10_control",

                # executable="demo_emergency_enable",
                # name="demo_emergency_enable",

                namespace=robot,
                output="screen",
            )
        )

    return LaunchDescription(nodes)

