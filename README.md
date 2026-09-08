# LinkerBot Competition Workspace

这是比赛开发主仓库。相机 SDK 与机器人 SDK 保持在仓库外部，业务代码只依赖它们的稳定接口；日常运行统一通过 `scripts/`，无需手工拼接 ROS 参数。

## 目录关系

相机 SDK、机器人 SDK 与本仓库应位于同一级目录：

```text
├── lbot_ws/            # 本 Git 仓库：比赛代码、配置、工具和入口
├── OrbbecSDK_ROS2/     # 外部相机 SDK/已编译工作区
└── Dexterous-Hand/     # 外部机器人 SDK
```

仓库内的 `apps/` 是应用入口，`config/` 是可调参数的唯一入口，`scripts/` 提供稳定命令，`src/lbot_vision/` 是 ROS 感知适配包，`tools/` 存放独立开发和标定工具，`artifacts/` 保存本地产物。

随仓库提供的机器人 SDK 组件包括：

| 路径 | 说明 |
| --- | --- |
| `src/lbot_arm_interfaces` | ROS 2 自定义消息和服务 |
| `src/lbot_driver` | 机械臂驱动、状态发布和服务 |
| `src/lbot_demo` | 示例程序 |
| `src/lbot_teleop` | 遥操作示例 |
| `assets/` | URDF、MJCF 和 STL 模型资源 |

## 快速开始

首次构建：

```bash
./scripts/build.sh
```

一条命令启动相机和螺母识别：

```bash
./scripts/run_perception.sh
```

该脚本运行无界面的感知服务。保持它运行，再在另一个终端启动查看器：

```bash
./scripts/show_camera.sh
```

默认查看 `/camera/color/image_raw`；查看识别标注画面：

```bash
./scripts/show_camera.sh --topic /nut_detection/debug_image
```

也可以分别启动组件：

```bash
./scripts/run_camera.sh
./scripts/run_detector.sh
```

躯干固定 Gemini2 的 R8 定位板外参标定、A4 打印和离线求解见 [相机外参标定](docs/camera_extrinsic_calibration.md)。入口为 `./scripts/calibrate_extrinsics.sh`，默认不发送机械臂运动命令。

运行仓库自检：

```bash
./scripts/test.sh
```

## 配置入口

- `config/workspace.env`：ROS、外部工作区路径和低内存构建并行度。
- `config/camera/gemini2.yaml`：相机 profile、流开关及内参文件。
- `config/vision/nut_detector.yaml`：识别阈值、话题和目标坐标系。
- `config/viewer/image.yaml`：图像查看器及默认实时图像话题。
- `config/control/nut_task.yaml`：桌面上方关节路线和机械臂速度。
- `config/experimental/nut_task.yaml`：旧运动原型参数，仅供参考。

## 机器人 SDK

使用机器人驱动前，请先阅读随 SDK 提供的开发环境、控制接口、机器人模型、灵巧手、相机与视觉传感器、手眼标定和安全操作文档。典型启动流程为：

```bash
source /opt/ros/jazzy/setup.bash
cd src/lbot_driver/lib && sudo ./lib_install.sh
cd ../..
colcon build
source install/setup.bash
ros2 launch lbot_driver lbot_start_driver.launch.py
```

默认机器人 IP 为 `192.168.10.21`，默认命名空间为 `/robot1`。关节单位为弧度（`rad`），位置单位为米（`m`）。

配置检查和机械臂路线运行统一使用 launch；默认只检查、不运动：

```bash
ros2 launch lbot_control lbot_start_control.launch.py
ros2 launch lbot_control lbot_start_control.launch.py mode:=enter execute_motion:=true
```

## 安全边界

当前默认运行路径只启动相机和视觉识别，不发送机械臂运动命令。旧的 `nut_task_node.cpp` 位于 `src/lbot_vision/experimental/`，其中包含占位姿态和坐标，不会被构建或作为比赛运行入口。正式运动控制器应通过独立机器人适配层接入，并默认保持执行开关关闭。

运动前确认急停未触发、机械臂已使能、工作区无人；首次运行应使用低速和小幅度动作。

继续开发前请阅读 [开发交接规范](HANDOFF.md)。更多信息见[架构说明](docs/architecture.md)、[开发约定](docs/development.md)、[运行与故障排查](docs/troubleshooting.md)和[迁移来源](docs/provenance.md)。
