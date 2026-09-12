# LinkerBot 比赛工作空间 — Agent 快速入门

## 项目概述

本仓库是机器人比赛的主开发工作空间。目标是让左臂从桌面左侧黑框中按**大→中→小**顺序抓取三颗螺母，依次放入右侧蓝色螺母筐的三个格子内。

**技术栈**：ROS 2 Jazzy、C++17、Python 3、OpenCV、MoveIt 2、colcon 构建系统。

**硬件**：双臂协作机器人（仅用左臂+左 O6 灵巧手）、Orbbec Gemini 2 深度相机。

## 仓库目录结构

```
lbot_ws/
├── AGENTS.md              # ← 本文件：Agent 工作指南
├── README.md              # 项目总览和快速开始
├── HANDOFF.md             # 开发交接规范（依赖方向、目录职责、完成定义）
├── config/                # 所有可调参数的唯一来源
│   ├── camera/            #   Gemini 2 相机 profile
│   ├── vision/            #   nut_detector.yaml（识别阈值/H/B/G话题/TF）
│   ├── control/           #   nut_task.yaml（路线示教/运动参数/手参数）
│   ├── calibration/       #   外参标定结果
│   └── workspace.env      #   环境路径配置
├── src/                   # ROS 2 package 源码
│   ├── lbot_motion/       #   [设备层] 左臂 MoveJ/MoveJP/MoveL/关节状态/急停
│   ├── lbot_control/      #   [任务层] 状态机、任务协调、抓放规划、视觉适配
│   ├── lbot_vision/       #   [感知层] 2D 检测、尺寸校正、ROS 检测节点
│   ├── lbot_driver/       #   机械臂驱动（外部 SDK 封装）
│   ├── lbot_arm_interfaces/ # ROS 2 自定义消息和服务
│   ├── lbot_demo/         #   示例程序
│   ├── lbot_teleop/       #   遥操作
│   └── lbot_rerun/        #   Rerun 三维可视化
├── apps/                  # Python 应用入口和进程编排
├── linkerbot/             # 可复用 Python 适配层（配置解析、进程命令）
├── scripts/               # 用户稳定入口脚本（run_camera.sh 等）
│   └── _common.sh         #   共享环境加载（source ROS + camera + robot workspace）
├── tools/                 # 独立标定/诊断工具
├── artifacts/             # 标定结果、采集数据（Git忽略内容物）
├── tests/                 # 配置级和入口级测试
├── docs/                  # 架构决策、接口说明
├── assets/                # URDF/MJCF/STL 模型资源
├── build/ install/ log/   # 构建产物（Git忽略）
└── 开发环境说明.md        # 网络配置、IP、示例程序
```

## 模块依赖关系

```
scripts → apps → linkerbot 适配层 → ROS 节点/外部 SDK
                               \→ 纯算法模块
```

- **`lbot_motion`**：仅封装左臂硬件。不包含任务逻辑、视觉或路线。
- **`lbot_control`**：任务状态机 + 抓放规划 + `VisionSystem` 接口。唯一依赖 `lbot_motion` 的任务层。
- **`lbot_vision`**：纯视觉。发布 `/nut_detections/sequence`、`/nut_slots`、debug 图像。**不发送运动命令**。
- **视觉适配**：`lbot_control/src/ros_vision_system.cpp` 将 `lbot_vision` 的话题和服务适配到 `VisionSystem` 抽象接口。

**Agent 最常修改的目录**：
| 目录 | 改什么 |
|---|---|
| `src/lbot_vision/src/core/nut_detector.cpp` | 2D 检测算法（黑框/篮框/螺母） |
| `src/lbot_vision/src/ros/nut_detector_node.cpp` | ROS 节点逻辑、槽位缓存 |
| `src/lbot_control/src/ros_left_arm_motion_system.cpp` | 运动执行 |
| `src/lbot_control/src/motion_plan.cpp` | 抓放位姿生成 |
| `config/vision/nut_detector.yaml` | 识别参数 |
| `config/control/nut_task.yaml` | 示教路线、手参数、运动参数 |

## 当前开发重点

1. **视觉识别优化**（进行中）：解决螺母靠近黑框丢失、两螺母粘连、篮框槽位划分不稳定等问题。`nut_detector.cpp` 已有 ROI 内缩放宽、分水岭分裂、篮框 HSV 后备通路等改动。
2. **三颗螺母完整抓取**（下一步）：实现大→中→小全流程的稳定执行。
3. **篮框槽位锁定**（已实现）：`nut_detector_node` 中 `cached_slots_` 在首次成功计算三个格子位置后永久缓存，直到 reset。篮框物理固定，不需要每帧重检测。

## 任务行为

```
IDLE → MOVE_ABOVE_TABLE → PICK_AND_PLACE → CHECK_PICK_RESULT
  → RETURN_ABOVE_TABLE → 下一颗/重试/RETRACT → COMPLETE
```

- 抓取顺序：大→中→小。仅左臂参与。
- 单次抓放流程：张手→预抓→直线下降→闭手→直线抬升→MoveJP 到格子上方→直线下降→张手→撤离。
- 相机检查在放置撤离位执行，机械臂返回 `above_table` 之前。仅检查黑框中螺母是否消失。
- 运动失败进入 `ABORTED`，通过左臂急停。
- `MoveJ`：关节运动（示教路线）。`MoveJP`：末端位姿运动（跨区域）。`MoveL`：直线运动（抓放）。

## 开发环境

| 项目 | 值 |
|---|---|
| OS | Ubuntu 24.04 (WSL) |
| ROS 2 | Jazzy (`/opt/ros/jazzy/setup.bash`) |
| 编译器 | GCC (C++17) |
| Python | 3.12 |
| 机器人控制器 IP | `192.168.10.21` |
| 默认命名空间 | `/robot1` |
| 相机工作空间 | `~/orbbec_ws`（与本仓库同级） |
| 外部 SDK | `Dexterous-Hand/`（同级目录，只读） |

环境变量通过 `config/workspace.env` 和 `scripts/_common.sh` 统一管理。**不要**在脚本中硬编码绝对路径。

## 常用命令

### 构建和测试

```bash
# 初次构建或全量构建
./scripts/build.sh

# 仅构建单个包（如改完视觉代码后）
colcon build --packages-select lbot_vision --cmake-args -DCMAKE_BUILD_TYPE=Release --allow-overriding lbot_vision

# 同样构建任务层
colcon build --packages-select lbot_control --cmake-args -DCMAKE_BUILD_TYPE=Release --allow-overriding lbot_vision

# 运行视觉模块的单元测试
./build/lbot_vision/camera_geometry_test
./build/lbot_vision/nut_sequence_test
./build/lbot_vision/planar_geometry_test

# 仓库自检
./scripts/test.sh

# 无硬件离线识别测试
bash scripts/build_offline.sh
bash scripts/test_offline.sh
bash scripts/run_detector_offline.sh artifacts/datasets/scene_001.png
```

### source 环境

```bash
source /opt/ros/jazzy/setup.bash
source ~/lbot_ws/install/setup.bash
# 或一键加载所有：
source ~/lbot_ws/scripts/_common.sh
```

### 启动机器人

```bash
# 仅启动驱动（不运动）
ros2 launch lbot_driver lbot_start_driver.launch.py

# 基础路线验证（不运动）
ros2 launch lbot_control lbot_start_control.launch.py

# 基础路线验证（运动，低速先验证）
ros2 launch lbot_control lbot_start_control.launch.py mode:=enter execute_motion:=true
```

### 启动视觉

```bash
# 一条命令：相机驱动+检测节点+debug图窗
./scripts/run_perception.sh
# 另开终端看图：
./scripts/show_camera.sh --topic /nut_detection/debug_image

# 或者分步：
./scripts/run_camera.sh       # 终端1：相机
./scripts/run_detector.sh     # 终端2：检测节点
./scripts/show_camera.sh      # 终端3：原始图像（加 --topic 看 debug）
```

### 完整任务

```bash
# execute_task 默认 false，只验证不运动
ros2 launch lbot_control lbot_task.launch.py start_driver:=true execute_task:=true show_image:=true
```

### ROS 2 调试

```bash
ros2 topic list | grep -E "camera|nut_detection|nut_slots"
ros2 topic echo /nut_detections/status      # 状态 JSON
ros2 topic echo /nut_detections/sequence    # 序列状态
ros2 topic echo /nut_slots                  # 三格位姿
ros2 node list
ros2 service list | grep nut
```

### 查看日志

```bash
# ROS 2 日志
cat log/latest_*/nut_detector_node*.log
# 或实时查看
tail -f log/latest_*/nut_detector_node*.log
```

### Git 操作

```bash
git status --short
git diff src/lbot_vision/src/core/nut_detector.cpp
git log --oneline -10
```

## Agent 工作规范

### 修改代码前

- 先定位相关模块，参考上述"最常修改的目录"表格
- 阅读 `HANDOFF.md` 了解目录职责和依赖方向
- 改动 `nut_detector.cpp` 前先确认 `.bak` 备份已存在

### 修改时

- 优先复用现有函数和配置结构（如 `DetectorConfig` 的 X-macro 字段系统）
- C++ 代码先改后编译验证，Python 代码改完即可测
- 视觉算法改动后至少跑 `camera_geometry_test`、`nut_sequence_test`、`planar_geometry_test`
- 任务层改动后验证 `colcon build` 全量通过
- YAML 参数改动后不需要重新编译，重启对应节点即可

### 安全红线

- **不要**执行 `git reset --hard`、`git checkout --` 等破坏性操作
- **不要**清理或回退 `build/`、`install/`、`log/` 中的已有改动
- **不要**在视觉节点中发送机械臂运动命令
- **不要**用零值占位关节角执行真实运动
- **运动开关默认必须为 `false`**：`execute_motion:=false`、`execute_task:=false`
- 修改运动相关配置后，必须先用低速和 `mode:=enter` 单程验证
- **不要**删除 `AGENTS.md` 中已有的有效信息

### 修改完成后

- 明确说明：修改了哪些文件、解决了什么问题、如何验证
- 如果涉及 `.yaml` 参数变更，说明新旧值的差异和理由

## 重要配置文件清单

| 文件 | 作用 | 修改频率 |
|---|---|---|
| `config/vision/nut_detector.yaml` | 识别参数（HSV/阈值/话题/TF） | 中 |
| `config/control/nut_task.yaml` | 示教路线、手参数、运动参数 | 中 |
| `config/camera/gemini2.yaml` | 相机 profile | 低 |
| `config/workspace.env` | 环境路径 | 低 |
| `config/viewer/image.yaml` | 图像查看器话题 | 低 |

## 不应随意修改的文件/目录

- `src/lbot_driver/`：外部 SDK 封装，与机器人固件耦合
- `src/lbot_arm_interfaces/`：消息定义变更会影响所有包
- `install/`、`build/`、`log/`：构建产物，已 Git 忽略
- `artifacts/calibration/extrinsics/`：标定结果，仅通过标定流程修改
- `apps/`、`linkerbot/`：Python 适配层，接口变更需要同步更新 `scripts/`

## 视觉数据流速查

```
相机图像 (color/depth/CameraInfo)
  → nut_detector_node: 2D检测 → 序列稳定 → 深度采样 → TF变换
  → 发布: /nut_detections/sequence (NutSequenceState)
         /nut_slots               (PoseArray ×3)  ← 首次成功后缓存
         /nut_detections/large    (PointStamped)
  → RosVisionSystem: initial_scene() / check_nut_in_source()
  → TaskCoordinator: 规划抓放位姿 → 执行运动
```

## 坐标系约定

- 任务层只接收 `base_link` 坐标系下的结果
- 视觉节点内部处理像素坐标，输出前通过 TF 转换到 `base_link`
- TF 缺失时视觉节点不会把相机坐标冒充基座坐标
- 篮框三个槽位沿机器人 X 轴（`base_link`）等分
