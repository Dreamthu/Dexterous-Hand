import launch
import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    # YAML 默认模板文件
    base_yaml_file = os.path.join(
        get_package_share_directory('lbot_driver'),
        'config', 'lbot_config.yaml'
    )

    # 定义机器人列表，每个机器人名字和 IP
    robots = [
        {"name": "robot1", "arm_ip": "192.168.10.21"},
        # {"name": "robot2", "arm_ip": "192.168.10.22"},
    ]

    nodes = []

    for robot in robots:
        # 每个机器人只需要启动一个 lbot_driver 可执行文件
        # 这个可执行文件内部会创建三个节点：主节点、左臂服务节点、右臂服务节点
        driver_node = Node(
            package='lbot_driver',
            executable='lbot_driver',
            namespace=robot["name"],        # 设置 namespace
            parameters=[
                base_yaml_file,             # 默认 YAML 文件
                {                           # 覆盖参数
                    "arm_ip": robot["arm_ip"]
                }
            ],
            output='screen',
            emulate_tty=True,              # 更好的日志输出格式
        )
        nodes.append(driver_node)

    return LaunchDescription(nodes)
