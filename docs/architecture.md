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
    ├── OrbbecSDK_ROS2    外部相机驱动
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
3. `lbot_motion` 是底层机器人适配包，只提供左臂/左手命令和左臂状态读取，不包含任务逻辑。
4. `lbot_control` 保留任务状态机和执行器，通过 `lbot_motion::LeftArmMotionDevice` 访问机器人；
   `MOVE_ABOVE_TABLE` 状态内部按配置执行任意数量的关节节点。
5. `apps/` 只负责组合组件和清理进程，不实现识别或运动算法。

## 相机几何数据流

Orbbec 驱动从 Gemini 2 固件读取当前 profile 对应的厂内参和畸变，并发布彩色
`CameraInfo`。`nut_detector_node` 是 ROS adapter：它把 `K`、`D`、畸变模型和图像尺寸
转换为不依赖 ROS 的 `lbot_vision::CameraGeometry`。所有原始像素的三维反投影和米制
半径估算都经过该模块去畸变。

深度注册由驱动使用设备内部的彩色—深度外参完成。相机到机器人或现场的外参不属于
设备出厂参数，未来由独立工具求解并通过 TF 接入。两者不得放入同一配置文件或相互覆盖。

## 外部库

`../OrbbecSDK_ROS2` 和外部机器人 SDK 不属于本仓库，也不会由运行脚本更新、拉取或修改。
构建脚本只读取 `Dexterous-Hand/src/lbot_arm_interfaces` 并把生成物写入本仓库自己的
`build/install/log`。

## 参数

可调参数只放在根目录 `config/`。ROS package 内不安装私有 launch/config 副本，避免
出现两份参数来源。需要新增参数时，先加入对应 YAML，再由适配层传入节点。

## 安全

感知调试和运动控制保持独立。任何未来运动入口都必须具备：

- 默认 `execute_motion: false`；
- 急停、左臂关节状态和 TF 检查；
- 明确的超时与失败状态；
- 仿真/录包验证后才能连接真实机械臂。
