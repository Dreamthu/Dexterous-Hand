# LinkerBot Competition Workspace

这是比赛开发主仓库。相机 SDK 与机器人 SDK 保持在仓库外部，业务代码只依赖它们的
稳定接口；日常运行统一通过 `scripts/`，无需手工输入 `ros2 run` 或拼接 ROS 参数。

## 目录关系

默认配置使用以下同级目录；实际目录名可通过本地配置覆盖：

```text
├── linkerbot_ws/       # 本 Git 仓库：比赛代码、配置、工具和入口
├── OrbbecSDK_ROS2/     # 静态外部相机 SDK/已编译工作区
└── lbot_ws/            # 静态外部机器人 SDK/工作区，包含 src/lbot_arm_interfaces
```

仓库内部：

```text
linkerbot_ws/
├── apps/               # 应用入口实现，封装 ROS 进程细节
├── config/             # 所有可调整参数的唯一入口
│   ├── calibration/
│   ├── camera/
│   ├── experimental/
│   ├── viewer/
│   ├── vision/
│   └── workspace.env
├── docs/               # 架构、开发约定和任务设计
├── linkerbot/          # 可复用的运行时适配代码
├── scripts/            # 用户直接执行的稳定命令
├── src/lbot_vision/    # ROS 感知适配包
├── tools/              # 独立开发/标定工具
├── tests/              # 配置与入口测试
└── artifacts/          # 标定结果和运行日志（默认不提交 Git）
```

## 快速开始

外部路径以 `config/workspace.env` 为默认值。本机目录不同时，新建被 Git 忽略的
`config/workspace.local.env`，只填写需要覆盖的变量，例如：

```bash
LINKERBOT_CAMERA_WORKSPACE=../OrbbecSDK_ROS2
LINKERBOT_ROBOT_WORKSPACE=../Dexterous-Hand
```

相对路径始终相对本仓库根目录；不要将个人绝对路径提交到共享配置。外部机器人工作区
必须包含 `src/lbot_arm_interfaces/package.xml`。本仓库自身即使也叫 `Dexterous-Hand`，
仍不能代替外部机器人 SDK。

首次构建：

```bash
cd linkerbot_ws
./scripts/build.sh
```

一条命令启动相机和螺母识别：

```bash
./scripts/run_perception.sh
```

该脚本运行无界面的感知服务，不会自动弹出图像窗口。保持它运行，再用另一个终端启动
查看器。

只启动其中一个组件：

```bash
./scripts/run_camera.sh
./scripts/run_detector.sh
```

查看实时图像：

```bash
./scripts/show_camera.sh
```

默认打开 `/camera/color/image_raw`。查看识别标注画面时可以运行：

```bash
./scripts/show_camera.sh --topic /nut_detection/debug_image
```

躯干固定 Gemini2 的 R8 定位板外参标定、A4 打印和离线求解见
[`docs/camera_extrinsic_calibration.md`](docs/camera_extrinsic_calibration.md)。入口为
`./scripts/calibrate_extrinsics.sh`，默认不发送机械臂运动命令。

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

Windows 提供同名 `.ps1` 入口。配置加载、离线检测和 `--help` 不依赖 POSIX 相机锁；
真实相机入口仍要求 Linux/POSIX 与 ROS，Windows 上会明确拒绝启动。离线和现场 ROS 节点共用 C++ 检测核心，阈值仍只从
`config/vision/nut_detector.yaml` 读取。原图、配置快照、掩膜、候选拒绝原因和结果写入
被 Git 忽略的 `artifacts/offline_detection/`。

静态离线回放仍是逐张独立检测，不输出可执行抓取坐标。ROS 感知节点另外接入了固定任务专用的
阶段顺序模块：初始稳定识别三颗后固定 `nut_large/nut_medium/nut_small`，只有外部明确的
`start/complete/retry/reset` 反馈才推进 3→2→1 阶段，目标消失不会自动判定完成。它不是
通用空间跟踪器，剩余目标仍按当前像素尺寸重新排序。详见
[离线识别说明](docs/offline_detection.md)、[螺母顺序身份与状态](docs/nut_sequence.md) 和
[2026-09-08 交接/提交说明](docs/nut_sequence_changes.md)。

## 配置入口

- `config/workspace.env`：ROS、外部工作区路径和低内存构建并行度。
- `config/camera/gemini2.yaml`：相机 profile、流开关及 IMU 接口；内参来自驱动 CameraInfo。
- `config/vision/nut_detector.yaml`：识别阈值、话题和目标坐标系。
- `config/vision/offline.yaml`：离线构建目录、输出目录和低并发构建设置。
- `config/viewer/image.yaml`：图像查看器及默认实时图像话题。
- `config/experimental/nut_task.yaml`：仅用于保存旧运动原型参数，不属于运行入口。


## 安全边界

当前默认运行路径只启动相机和视觉识别，不发送机械臂运动命令。原来的
`nut_task_node.cpp` 含有占位坐标，保存在
`src/lbot_vision/experimental/` 供设计参考，但不会被构建。正式运动控制器必须通过
独立机器人适配层接入，并默认保持执行开关关闭。

继续开发前请阅读 [开发交接规范](HANDOFF.md)。更多信息见
[架构说明](docs/architecture.md)、[开发约定](docs/development.md)、
[运行与故障排查](docs/troubleshooting.md) 和 [迁移来源](docs/provenance.md)。

本次 main/branch 整合结果、验证边界和 PR 验收清单见
[2026-09-08 整合记录](docs/integration_20260908.md)。
