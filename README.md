# LinkerBot Competition Workspace

这是比赛开发主仓库。相机 SDK 与机器人 SDK 保持在仓库外部，业务代码只依赖它们的稳定接口；相机/感知可使用 `scripts/`，机械臂运动和完整任务统一通过 ROS 2 `launch` 启动。

## 目录关系

`linkerbot_ws/` 是主要工作空间；`orbbec_ws/` 相机依赖与 `Dexterous-Hand/` 官方机器人依赖与它同级：

```text
├── linkerbot_ws/       # 主要工作空间：比赛代码、配置、工具和入口
├── orbbec_ws/          # 相机依赖工作空间
└── Dexterous-Hand/     # 官方机器人依赖
```

仓库内的 `apps/` 是应用入口，`config/` 是可调参数的唯一入口，`scripts/` 提供稳定命令，`linkerbot/` 是可复用 Python 适配层，`src/` 是 ROS 包，`tools/` 存放独立开发和标定工具，`artifacts/` 保存本地产物，`docs/` 保存说明文档。

任务所需的模型资源与 ROS 接口定义保留在仓库内：

| 路径 | 说明 |
| --- | --- |
| `src/lbot_arm_interfaces` | 左臂/左手任务控制链路使用的 ROS 2 接口定义 |
| `src/lbot_motion` | 左臂/左手 ROS 适配层，不包含任务逻辑 |
| `src/lbot_control` | 螺母识别抓放状态机与任务控制器 |
| `src/lbot_vision` | Gemini 2 螺母和蓝筐感知 |
| `src/lbot_rerun` | 三维任务观察节点，只读视觉与运动状态 |
| `linkerbot/` | 配置加载、命令适配、外参算法和离线回放共享库 |
| `开发资源/assets/` | 唯一的模型资产根目录，包含左臂、O6 手、工作站和螺母对象 |

## 快速开始

首次构建：

```bash
./scripts/build.sh
```

一条命令启动相机和螺母识别：

```bash
./scripts/run_perception.sh
```

该脚本会先启动相机，再自动发布已标定的
`base_link -> camera_link` 静态 TF，最后启动螺母识别。保持它运行，再在另一个终端启动查看器：

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

如果单独运行检测节点，而不是使用 `run_perception.sh`，需要另开终端发布已标定的外参：

```bash
./scripts/calibrate_extrinsics.sh publish \
  --result artifacts/calibration/extrinsics/20260910_eye_to_hand_v2/extrinsics.yaml
```

躯干固定 Gemini2 的 R8 定位板外参标定、A4 打印和离线求解见 [相机外参标定](docs/camera_extrinsic_calibration.md)。入口为 `./scripts/calibrate_extrinsics.sh`，默认不发送机械臂运动命令。

运行仓库自检：

```bash
./scripts/test.sh
```

遇到灰色窗口、深度流未启动或 rqt 卸载警告时，参见
[运行与故障排查](docs/troubleshooting.md)。

## 无硬件二维识别调试

无需 ROS、相机或机器人，可在安装 C++17 / CMake / OpenCV 4 开发库 / PyYAML 后使用：

```bash
bash scripts/build_offline.sh
bash scripts/test_offline.sh
bash scripts/run_detector_offline.sh artifacts/datasets/scene_001.png
```

Windows 提供同名 `.ps1` 入口。离线和现场 ROS 节点共用 C++ 检测核心，阈值仍只从
`config/vision/nut_detector.yaml` 读取。原图、配置快照、掩膜、候选拒绝原因和结果写入
被 Git 忽略的 `artifacts/offline_detection/`。

静态离线回放仍是逐张独立检测，不输出可执行抓取坐标。ROS 感知节点另外接入了固定任务专用的
视觉几何：使用 190×190 mm 黑框校正后的轮廓尺寸区分大中小，蓝筐通过矩形筛选后沿机器人 X 轴三等分。
详见 [尺寸校正与分格说明](docs/perspective_sizing_and_robot_x_slots.md)。

阶段顺序模块：初始稳定识别三颗后固定 `nut_large/nut_medium/nut_small`，只有外部明确的
`start/complete/retry/reset` 反馈才推进 3→2→1 阶段，目标消失不会自动判定完成。它不是
通用空间跟踪器，剩余目标仍按当前像素尺寸重新排序。详见
[离线识别说明](docs/offline_detection.md)、[螺母顺序身份与状态](docs/nut_sequence.md) 和
[2026-09-07 交接/提交说明](docs/nut_sequence_changes.md)。

## 配置入口

- `config/workspace.env`：ROS、外部工作区路径和低内存构建并行度。
- `config/camera/gemini2.yaml`：相机 profile、流开关及内参文件。
- `config/vision/nut_detector.yaml`：识别阈值、话题和目标坐标系。
- `config/vision/offline.yaml`：离线构建目录、输出目录和低并发构建设置。
- `config/viewer/image.yaml`：图像查看器及默认实时图像话题。
- `config/viewer/rerun.yaml`：Rerun 三维任务观察节点的话题、点云限额和 O6 显示参数。
- `config/control/nut_task.yaml`：桌面上方关节路线和机械臂速度。

## 机器人适配

Dexterous-Hand 的驱动、示例和遥操作包已从本仓库移除；本仓库只保留任务控制所需接口、模型和适配层。运行完整任务前，由现场提供已构建并运行的机器人驱动节点，其服务/话题命名必须满足 `lbot_motion` 的接口约定。

配置检查、机械臂路线和完整任务运行统一使用 ROS 2 launch；默认只检查、不运动：

```bash
ros2 launch lbot_control lbot_start_control.launch.py
ros2 launch lbot_control lbot_start_control.launch.py mode:=enter execute_motion:=true
# 先启动外部机器人驱动；视觉已接入的完整任务入口（execute_task 默认 false）
ros2 launch lbot_control lbot_task.launch.py execute_task:=true
```

任务执行的三维观察可另开终端启动，不会发送机械臂命令：

```bash
ros2 launch lbot_rerun lbot_rerun.launch.py
```

详见 [Rerun 任务可视化](docs/rerun_visualization.md)。

## 安全边界

正式运动控制器为 `lbot_control/nut_task_controller`，通过 `RosVisionSystem` 消费视觉结果，并默认保持 `execute_task` 关闭。旧的实验任务节点和占位配置已移除，不得重新加入正式入口。

运动前确认急停未触发、机械臂已使能、工作区无人；首次运行应使用低速和小幅度动作。

## 仓库产物与已知状态

`build/`、`install/` 和 `log/` 是 colcon 本地生成目录，已从 Git 索引移除但保留在本机。不要提交这些目录，也不要清理或回退其中已有的本地改动。源码树中的 `__pycache__/`、无关截图和本地 HSV 调参产物已移除。

当前 `./scripts/test.sh` 中有一个既有错误：`tests/test_rerun_core.py` 引用的 `lbot_rerun.core` 模块缺失，因此该项测试无法导入；其余测试通过。`./scripts/build.sh` 当前可通过 5 个 ROS 包构建。

继续开发前请阅读 [开发交接规范](HANDOFF.md)。更多信息见[架构说明](docs/architecture.md)、[开发约定](docs/development.md)、[运行与故障排查](docs/troubleshooting.md)和[迁移来源](docs/provenance.md)。
