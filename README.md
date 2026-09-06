# LBot ROS2 SDK 资料总览

## 这份资料是什么

本目录是本次赛事提供给参赛队的 ROS2 SDK 工作空间：

```bash
/home/simon/lbot_ws
```

它主要用于控制双臂机器人、灵巧手、遥操作，以及查看随包提供的机器人模型资源。

## 目录结构

| 路径 | 说明 |
| --- | --- |
| `src/lbot_arm_interfaces` | ROS2 自定义消息和服务 |
| `src/lbot_driver` | 机械臂驱动，负责连接控制器、发布状态、提供服务 |
| `src/lbot_demo` | 示例程序 |
| `src/lbot_teleop` | 遥操作示例 |
| `开发资源/机械臂控制接口文档v1.0.5.pdf` | 底层接口说明 |
| `开发资源/机器人控制平台说明文档v1.1.1.pdf` | Web 控制平台说明 |
| `开发资源/assets` | URDF、MJCF、STL 模型资源 |

## 推荐阅读顺序

1. `开发环境说明.txt`
2. `控制接口说明.txt`
3. `机器人模型说明.txt`
4. `灵巧手说明.txt`
5. `相机与视觉传感器说明.txt`
6. `手眼标定说明.txt`
7. `安全操作手册.txt`
8. `现场快速检查表.txt`
9. `常见问题FAQ.txt`

## 快速启动

```bash
cd /home/simon/lbot_ws
source /opt/ros/jazzy/setup.bash

cd src/lbot_driver/lib
sudo ./lib_install.sh

cd /home/simon/lbot_ws
colcon build
source install/setup.bash

ros2 launch lbot_driver lbot_start_driver.launch.py
```

默认机器人 IP：

```text
192.168.10.21
```

默认命名空间：

```text
/robot1
```

## 常用命令

| 目的 | 命令 |
| --- | --- |
| 查看节点 | `ros2 node list` |
| 查看话题 | `ros2 topic list` |
| 查看服务 | `ros2 service list` |
| 查看左臂关节状态 | `ros2 topic echo /robot1/left_arm/joint_states` |
| 启动驱动 | `ros2 launch lbot_driver lbot_start_driver.launch.py` |
| 启动示例 | `ros2 launch lbot_demo lbot_start_demo.launch.py` |
| 启动遥操作 | `ros2 launch lbot_teleop lbot_start_teleop.launch.py` |

## 视觉说明

本次资料记录的相机型号是 Orbbec Gemini2。相机 SDK 不在当前 `lbot_ws/src` 里，参赛队需要按 `相机与视觉传感器说明.txt` 单独安装 Orbbec ROS2 或 Python SDK。

如果比赛任务需要视觉抓取，请同时确认：

- Gemini2 图像、深度图、点云能正常发布。
- 相机内参已经保存。
- 相机到机器人 base 的外参已经完成标定。

## 重要提醒

- 运动前确认急停未触发、机械臂已使能、工作区无人。
- 关节角单位是弧度 `rad`，不是角度。
- 位置单位是米 `m`。
- 首次运行请使用低速、小幅度动作。
- 本资料以当前目录里的 SDK 和模型文件为准。
