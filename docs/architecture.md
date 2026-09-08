# 架构说明

## 设计目标

仓库把外部 ROS SDK、ROS 通信细节、可复用算法和用户入口分开，避免比赛逻辑直接
依赖某个启动文件或终端命令。

```text
scripts/*.sh
    │
    ▼
apps/*.py                 应用生命周期、进程编排
    │
    ▼
linkerbot/runtime.py      ROS 命令与配置适配边界
    │
    ├── orbbec_ws         外部相机驱动
    └── lbot_vision       仓库内 ROS 感知适配包
            │
            ▼
       CameraInfo 适配 -> 纯相机几何 -> 图像/深度识别逻辑
```

用户只依赖 `scripts/` 的稳定入口。ROS package 名、节点可执行文件路径和参数文件路径等
细节集中在内部适配层。

## 依赖方向

1. `tools/` 可以依赖 ROS 图像消息，但不能依赖比赛任务控制器。
2. `lbot_vision` 负责输入图像/深度并输出结构化检测，不发送机器人动作。
3. 后续运动控制应新增独立 package，例如 `src/linkerbot_control`。
4. `linkerbot_control` 只能通过一个机器人适配类访问 `lbot_arm_interfaces`，任务状态机
   不直接创建 ROS service client。
5. `apps/` 只负责组合组件和清理进程，不实现识别或运动算法。

## 相机几何数据流

Orbbec 驱动从 Gemini 2 固件读取当前 profile 对应的厂内参和畸变，并发布彩色
`CameraInfo`。`nut_detector_node` 是 ROS adapter：它把 `K`、`D`、畸变模型和图像尺寸
转换为不依赖 ROS 的 `lbot_vision::CameraGeometry`。所有原始像素的三维反投影和米制
半径估算都经过该模块去畸变。

深度注册由驱动使用设备内部的彩色—深度外参完成。相机到机器人或现场的外参不属于
设备出厂参数，未来由独立工具求解并通过 TF 接入。两者不得放入同一配置文件或相互覆盖。

## 外部库

`../orbbec_ws` 和 `../Dexterous-Hand` 不属于本仓库，也不会由运行脚本更新、拉取或修改。
构建脚本只读取 `Dexterous-Hand/src/lbot_arm_interfaces` 并把生成物写入本仓库自己的
`build/install/log`。

## 参数

可调参数只放在根目录 `config/`。ROS package 内不安装私有 launch/config 副本，避免
出现两份参数来源。需要新增参数时，先加入对应 YAML，再由适配层传入节点。

## 安全

感知调试和运动控制保持独立。任何未来运动入口都必须具备：

- 默认 `execute_task: false`；
- 急停、关节状态、工作空间和 TF 检查；
- 明确的超时与失败状态；
- 仿真/录包验证后才能连接真实机械臂。

## 二维核心与离线适配（任务一）

`src/lbot_vision/detector_core.cmake` 定义共享 `lbot_vision_detector` 目标，由 ROS package
和 `tools/offline_detection` 的独立 CMake 工程复用。`detect_2d` 只处理 BGR8 与普通配置结构。
场景几何、轮廓过滤、Hough fallback、掩膜和候选诊断均在此库中，ROS 节点不再保留第二份算法。

离线入口遵循 `scripts -> apps -> linkerbot/offline.py -> C++ 文件适配 -> 检测核心`。
Python 只负责配置、路径、构建/子进程和复现记录，不实现识别算法。
算法参数类型来自同一个字段声明表，数值只维护在中央 YAML；生成 JSON 仅作为运行快照。

二维结果不依赖篮筐存在、深度或 CameraInfo；三维与 TF 留在 ROS adapter，已有 CameraGeometry
不变。详细边界见 [离线识别说明](offline_detection.md)。

## 阶段顺序核心

`lbot_vision_sequence` 是第二个 ROS-free 核心，只接收带时间戳的二维圆观测。它不做通用位置
关联，而是针对固定“大→中→小”任务维护三个固定 ID，并在显式完成反馈后分别验证 3、2、1
颗剩余目标。检测数量只用于 fail-closed 验证，不能改变任务阶段。ROS adapter 通过结构化消息和
service 暴露状态；感知节点仍不发送机械臂动作。详细接口见 [螺母顺序身份与状态](nut_sequence.md)。
