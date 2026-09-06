# lbot_motion 项目说明

`lbot_motion` 是本工作空间的机械臂与灵巧手运动控制包。它把 ROS 设备接口、运动命令和动作时序规划分开，便于复用底层控制代码，也便于以后增加新的动作。

本包使用已有的 `lbot_driver` 和 `lbot_arm_interfaces`，不直接连接机械臂 SDK。机械臂运动通过 `MoveJ` 服务完成，灵巧手通过 L6 ROS 话题完成。

## 1. 目录结构

```text
lbot_motion/
├── config/motion_demo.yaml              # 参数模板
├── launch/motion_demo.launch.py         # 示例动作启动文件
├── include/lbot_motion/
│   ├── motion_device.hpp                # 底层机械臂/手接口
│   ├── motion_planner.hpp               # 通用时间轴规划接口
│   └── camera_motion_plan.hpp           # 相机动作定义接口
├── src/
│   ├── motion_device.cpp                # MoveJ、关节状态、L6话题实现
│   ├── motion_planner.cpp               # 按毫秒执行命令
│   ├── camera_motion_plan.cpp           # 可编辑的拓展/收回动作
│   └── motion_demo_node.cpp             # ROS2 示例节点
└── README.md
```

## 2. 控制接口

当节点运行在 `robot1` 命名空间时，机械臂接口为：

```text
/robot1/left_arm/joint_states
/robot1/right_arm/joint_states
/robot1/left_arm/move_joint
/robot1/right_arm/move_joint
```

关节顺序为 J1 到 J7，角度单位为弧度。`MoveJ` 请求包含 7 个目标角度、速度、加速度和 `block` 标志。

灵巧手接口为：

```text
/robot1/left_hand/set_l6_joint
/robot1/left_hand/set_l6_speed
/robot1/left_hand/set_l6_force
/robot1/right_hand/set_l6_joint
/robot1/right_hand/set_l6_speed
/robot1/right_hand/set_l6_force
```

L6 六个通道顺序为 `[thumb_yaw, thumb_pitch, index, middle, ring, pinky]`，位置、速度和力矩均为 `0..255`。

## 3. 软件架构

### MotionDevice

[motion_device.hpp](/home/lzy/lbot_ws/src/lbot_motion/include/lbot_motion/motion_device.hpp) 提供可复用底层库：

- `ArmController`：订阅关节状态并封装 `MoveJ` 服务；
- `HandController`：封装 L6 速度、力矩和位置话题；
- `MotionDevice`：统一管理左右臂和左右手。

### MotionPlanner

[motion_planner.hpp](/home/lzy/lbot_ws/src/lbot_motion/include/lbot_motion/motion_planner.hpp) 接收带 `time_ms` 的命令并按时间执行：

- 同一时刻同一侧机械臂的多个关节合并为一次七关节 MoveJ；
- 左右侧维护各自目标值；
- 手部命令发布六通道位置，未指定通道保持上一次值；
- 机械臂 MoveJ 使用 `block=true`，等待驱动返回成功或失败。

## 4. 命令格式

动作格式为：

```text
(左右侧, 机械臂/手, 关节, 运行值, 速度, 加速度, 运行时刻ms)
```

对应的 C++ 接口：

```cpp
using namespace lbot_motion;

planner.add(MotionCommand::arm_delta(
  1000, Side::Left, 2, 0.5, 0.2, 0.2));  // J3 增加 0.5 rad

planner.add(MotionCommand::arm_target(
  1000, Side::Left, 3, -1.0, 0.2, 0.2)); // J4 绝对目标

planner.add(MotionCommand::hand(
  1000, Side::Left, 0, 120, 180, 100));  // 左手拇指侧摆
```

机械臂关节索引为 `0..6`（J1..J7），手部索引为 `0..5`。

## 5. 如何拓展动作

当前相机动作定义在 [camera_motion_plan.cpp](/home/lzy/lbot_ws/src/lbot_motion/src/camera_motion_plan.cpp)。编辑其中的 `extension` 和 `times_ms`：

```cpp
const std::vector<std::pair<std::size_t, double>> extension = {
  {0, 20.0},   // J1 +20°
  {3, -90.0},  // J4 -90°
  {0, 75.0},   // J1 +75°
  {5, 90.0}    // J6 +90°
};

const std::array<uint64_t, 4> times_ms = {{0, 3000, 12000, 20000}};
```

`extension[i]` 在 `times_ms[i]` 时刻运行。示例文件的角度使用度，内部会自动转换为弧度。增加 J2 在 6000 ms 旋转 30°：

```cpp
// extension 增加 {1, 30.0}，times_ms 增加 6000
```

`retract` 会反向遍历动作并取反，不需要单独编写收回动作。灵巧手在 0 ms 设置为闭合值或打开值。

## 6. 参数调试

参数入口是 [motion_demo.launch.py](/home/lzy/lbot_ws/src/lbot_motion/launch/motion_demo.launch.py)，默认模板是 [motion_demo.yaml](/home/lzy/lbot_ws/src/lbot_motion/config/motion_demo.yaml)。

| 参数 | 默认值 | 说明 |
|---|---:|---|
| `arm` | `right` | `left`、`right`，支持 `l`、`r` |
| `motion` | `extend` | `extend` 或 `retract` |
| `speed` | `0.2` | 机械臂速度 |
| `acce` | `0.2` | 机械臂加速度 |
| `state_timeout` | `15.0` | 等待关节状态的秒数 |
| `service_timeout` | `10.0` | 等待 MoveJ 服务的秒数 |
| `hand_enabled` | `true` | 是否控制灵巧手 |
| `hand_speed` | `250` | 灵巧手速度 |
| `hand_force` | `250` | 灵巧手力矩 |
| `hand_close_position` | `0` | 闭合位置 |
| `hand_open_position` | `255` | 打开位置 |

启动时覆盖参数：

```bash
ros2 launch lbot_motion motion_demo.launch.py \
  arm:=right motion:=extend speed:=0.1 acce:=0.1 hand_enabled:=false
```

## 7. 编译与运行

先启动驱动：

```bash
cd /home/lzy/lbot_ws
source /opt/ros/jazzy/setup.bash
ros2 launch lbot_driver lbot_start_driver.launch.py
```

新终端编译并运行：

```bash
cd /home/lzy/lbot_ws
colcon build --packages-select lbot_motion
source install/setup.bash
ros2 launch lbot_motion motion_demo.launch.py arm:=right motion:=extend
```

## 8. 调试与故障排查

```bash
ros2 service list | grep move_joint
ros2 topic echo /robot1/right_arm/joint_states
```

运行节点时应看到：

```text
Running extend timeline on right arm
Sending right MoveJ ...
right MoveJ accepted and completed
```

- 关节状态超时：检查驱动、命名空间和状态话题；
- MoveJ 不可用：检查 `/robot1/right_arm/move_joint` 或左臂服务；
- 驱动返回失败：检查使能、急停、速度、加速度和目标角度；
- 灵巧手不动作：检查 `set_l6_*` 话题和六元素数组；
- 动作时间延迟：阻塞式 MoveJ 会使后续时间点顺延。

## 9. 安全注意事项

- 首次运行使用低速度、低加速度和小角度；
- 确认工作区无人、机械臂已使能、急停已解除；
- 修改动作后先单独测试一个关节；
- 灵巧手首次闭合使用较小位置值和力矩；
- 不要在机械臂运动过程中覆盖正在使用的规划代码。
