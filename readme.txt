📖 简介
LBOT ROS2 机械臂控制包是一套完整的双臂机器人控制解决方案，支持 LinkerBot 系列双臂机器人的运动控制、遥操作和灵巧手控制。本项目基于 ROS2 Jazzy 开发，提供了丰富的示例程序和完善的接口文档。

✨ 主要特性
🤖 双臂协同控制 - 支持左右臂独立或协同运动控制
🎮 遥操作支持 - 通过主从臂实现实时遥操作控制
🖐️ 灵巧手控制 - 支持 L6/L10 系列灵巧手控制
🔧 运动学计算 - 提供正/逆运动学计算服务
📐 工具坐标系管理 - 灵活的工具坐标系设置与切换
🔄 自动重连机制 - 断线自动重连，保证系统稳定性
📡 50Hz 状态发布 - 高频率实时状态反馈
📁 功能包结构
<TEXT>
lbot_ros2/
├── lbot_arm_interfaces/    # 接口定义包 - 自定义消息和服务
├── lbot_driver/            # 驱动包 - 核心驱动节点与机械臂通信
├── lbot_demo/              # 示例包 - 使用示例程序
└── lbot_teleop/            # 遥操作包 - 主从臂遥操作功能

📦 lbot_arm_interfaces
接口定义包，包含所有自定义消息(msg)和服务(srv)定义。
📨 消息类型 (Messages)
🔧 服务类型 (Services)

📦 lbot_driver
核心驱动包，负责与机械臂硬件通信，提供 ROS2 服务和话题接口。
主节点 (lbot_main_node) - 连接管理、状态发布、心跳检测、关节跟随
左臂服务节点 (lbot_left_arm_node) - 左臂所有服务
右臂服务节点 (lbot_right_arm_node) - 右臂所有服务

📦 lbot_demo
示例功能包，提供各种使用示例程序。

示例程序	描述
demo_movej	关节空间运动示例
demo_movejp	位姿运动示例
demo_movel	直线运动示例
demo_fk	正运动学计算示例
demo_ik	逆运动学计算示例
demo_tool_frame	工具坐标系管理示例
demo_hand_l6_control	L6灵巧手控制示例
demo_hand_l10_control	L10灵巧手控制示例
demo_emergency_enable	急停与使能控制示例

📦 lbot_teleop
遥操作功能包，实现主从臂遥操作控制。

🔧 环境要求
操作系统: Ubuntu 24.04
ROS2 版本: Jazzy
编译工具: colcon

🚀 快速开始
1. 安装依赖
<BASH>
# 安装 ROS2 Jazzy (如未安装)
sudo apt update && sudo apt install ros-jazzy-desktop

2. 安装动态库
<BASH>
cd ~/lbot_ws/src/lbot_driver/lib
sudo ./lib_install.sh

3. 编译工作空间
<BASH>
mkdir -p ~/ros2_ws/src
cp -r lbot_ros2 ~/ros2_ws/src
cd ~/ros2_ws
colcon build
source install/setup.bash
4. 配置网络
修改 lbot_driver/config/lbot_config.yaml 中的机械臂 IP 地址：

<YAML>
arm_ip: "192.168.10.21"  # 根据实际情况修改
5. 启动驱动节点
<BASH>
# 启动驱动
ros2 launch lbot_driver lbot_start_driver.launch.py
6. 运行示例程序
<BASH>
# 新终端
source ~/lbot_ws/install/setup.bash
# 运行关节运动示例
ros2 launch lbot_demo lbot_start_demo.launch.py

遥操作模式
遥操作主臂通过USB接口连接主机，机械臂控制器通过网线连接主机后，启动遥操作程序
<BASH>
# 启动驱动节点
ros2 launch lbot_driver lbot_start_driver.launch.py
# 启动遥操作节点（新终端）
ros2 launch lbot_teleop lbot_start_teleop.launch.py
🌐 多机器人支持
修改 launch 文件支持多台机器人：

<PYTHON>
robots = [
    {"name": "robot1", "arm_ip": "192.168.10.21"},
    {"name": "robot2", "arm_ip": "192.168.10.22"},
]
话题和服务将自动添加命名空间：

/robot1/left_arm/joint_states
/robot2/left_arm/joint_states
📋 话题与服务列表
完整话题列表
# 状态发布 (50Hz)
/{namespace}/left_arm/joint_states      # sensor_msgs/JointState
/{namespace}/right_arm/joint_states     # sensor_msgs/JointState
/{namespace}/left_arm/pose_states       # geometry_msgs/PoseStamped
/{namespace}/right_arm/pose_states      # geometry_msgs/PoseStamped
# 关节跟随
/{namespace}/left_arm/joint_follow      # lbot_arm_interfaces/FollowJoint
/{namespace}/right_arm/joint_follow     # lbot_arm_interfaces/FollowJoint
# 灵巧手控制
/{namespace}/left_hand/set_l6_joint     # std_msgs/UInt8MultiArray
/{namespace}/left_hand/set_l6_force     # std_msgs/UInt8MultiArray
/{namespace}/left_hand/set_l6_speed     # std_msgs/UInt8MultiArray
/{namespace}/left_hand/set_l10_joint    # std_msgs/UInt8MultiArray
/{namespace}/left_hand/set_l10_force    # std_msgs/UInt8MultiArray
/{namespace}/left_hand/set_l10_speed    # std_msgs/UInt8MultiArray
完整服务列表
# 运动控制
/{namespace}/left_arm/move_joint        # MoveJ
/{namespace}/left_arm/move_pose         # MoveJP
/{namespace}/left_arm/move_linear       # MoveL

# 运动学
/{namespace}/left_arm/forward_kinematics    # ForwardKinematics
/{namespace}/left_arm/inverse_kinematics    # InverseKinematics

# 工具坐标系
/{namespace}/left_arm/set_tool_frame        # SetFrame
/{namespace}/left_arm/get_tool_frame        # GetFrame
/{namespace}/left_arm/get_current_tool_frame # GetCurrentFrame
/{namespace}/left_arm/get_all_tool_frames   # GetAllFrames
/{namespace}/left_arm/change_tool_frame     # ChangeFrame
/{namespace}/left_arm/delete_tool_frame     # DeleteFrame

# 系统控制
/{namespace}/left_arm/set_zero          # SetZero
/{namespace}/left_arm/set_enable        # SetEnable
/{namespace}/left_arm/set_emergency_stop # SetEmergency

# 右臂服务同上，将 left_arm 替换为 right_arm
