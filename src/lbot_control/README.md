# lbot_control

`lbot_control` 只保留比赛所需的最小任务流程：左臂进入桌面、按大中小抓放、检查抓取结果、
收回左臂。

## 状态机

```text
IDLE
  -> MOVE_ABOVE_TABLE
  -> PICK_AND_PLACE
  -> CHECK_PICK_RESULT
       ├─ 螺母仍在黑框：RETURN_ABOVE_TABLE -> 重试同一颗
       ├─ 螺母已不在黑框：RETURN_ABOVE_TABLE -> 处理下一颗
       └─ 小螺母也已不在黑框：RETURN_ABOVE_TABLE -> RETRACT
  -> COMPLETE
```

任何运动失败或取消都会进入 `ABORTED`。不检查双臂是否自然下垂，也不读取右臂状态。

`PICK_AND_PLACE` 的一次协调动作包含：打开手、到预抓点、下降、闭合、抬升、到对应格子、
下降、打开、撤离；随后在相机可见的放置撤离位立即执行 `CHECK_PICK_RESULT`，再执行
`RETURN_ABOVE_TABLE` 回到 `MOVE_ABOVE_TABLE` 的最终关节位置。也就是说，视觉检查发生在
同一次抓放动作内部，之后该动作才结束，因此不会被方框上方的机械臂遮挡，且每次抓放结束都
停在 `above_table`。不需要灵巧手传感器，也不检查蓝框中的放置结果。

视觉侧以后实现 [`VisionSystem`](include/lbot_control/vision_system.hpp) 即可。它先通过
`initial_scene()` 提供三颗螺母和三个格子的基座坐标，再通过 `check_nut_in_source()` 返回：

- `present()`：螺母仍在黑框，状态机重试当前尺寸；
- `removed()`：螺母已不在黑框，状态机进入下一尺寸；
- `fail()`：本次视觉判断无效，任务终止且机械臂保持在方框上方。

任务层只接受已经转换到机器人基座坐标系的结果，不依赖具体视觉话题或消息格式。

## 桌面路线

编辑根目录 `config/control/nut_task.yaml`：

```yaml
table_route_calibrated: false
route:
  names: [natural_down, outside_table, above_table]
  natural_down:  [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  outside_table: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  above_table:   [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
hand_open: [255, 40, 255, 255, 255, 255]
```

节点数量可以任意调整。进入时按列表正序执行，`RETRACT` 严格按反序执行。每个节点必须是
7 个弧度值。完成示教后才能把 `table_route_calibrated` 改为 `true`。

默认 launch 只检查配置，不运动：

```bash
ros2 launch lbot_control lbot_start_control.launch.py
```

基础路线实机测试：

```bash
ros2 launch lbot_control lbot_start_control.launch.py \
  mode:=enter execute_motion:=true
ros2 launch lbot_control lbot_start_control.launch.py \
  mode:=leave execute_motion:=true
ros2 launch lbot_control lbot_start_control.launch.py \
  mode:=round_trip execute_motion:=true
```

基础路线执行时，每个方向的路线都会先向左 O6 下发 `[0, 0, 0, 0, 0, 0]`，再执行关节路线；只有该方向
路线成功后才下发 `hand_open`，路线失败时保持收手。`round_trip` 在 enter 成功后张手，leave 开始前
再次收手，leave 成功后再次张手。
手指顺序为 `[thumb_yaw, thumb_pitch, index, middle, ring, pinky]`，取值范围为 `0..255`。

当前 `lbot_start_control.launch.py` 用于跑通基础路线；视觉已经通过
`RosVisionSystem` 接入，并可使用 `lbot_task.launch.py` 同时启动驱动、视觉节点和任务控制器。
完整任务执行仍需先填写实机路线、灵巧手和工具标定参数。

完整任务启动（默认仍为检查模式，不执行运动）：

```bash
ros2 launch lbot_control lbot_task.launch.py start_driver:=false execute_task:=false
```

确认所有标定和安全条件后，再设置 `start_driver:=true execute_task:=true`。任务控制器从
`/nut_detections/sequence` 和 `/nut_slots` 获取位姿，并通过 `/nut_detections/set_state` 发送
`start/complete/retry` 反馈；视觉检查在放置撤离位、返回 `above_table` 前完成。

## 实机需要测量和标定的参数

先只标定桌面路线并通过上面的 launch 完成 `enter`、`leave`、`round_trip` 低速验证；在此之前
不要把 `table_route_calibrated` 设为 `true`。完整抓放前需要以下数据。

| 项目 | 需要得到的数据 | 当前用途 |
| --- | --- | --- |
| 桌面路线 | 从 `natural_down` 到 `above_table` 的任意数量 7 关节节点，单位 rad | 填入 `route.names` 及每个同名数组；进入路线正序、收回路线反序 |
| 安全性 | 每个路线节点与桌边、黑框、蓝框、线缆的安全间隙 | 决定是否需要增加中间关节节点；禁止接近关节极限 |
| 工具安装 | 法兰到抓取中心的位置偏移、手指夹持中心、工具朝向 | 使视觉中的螺母中心可转换为机械手实际抓取中心 |
| 抓放姿态 | 抓取时的 `roll`、`pitch`、`yaw`，以及螺母姿态到工具 yaw 的偏移 | 配置 `tool_roll_rad`、`tool_pitch_rad`、`tool_yaw_offset_rad` |
| 动作高度 | 螺母上方预抓高度、抓住后的抬升高度、格子内释放高度 | 配置 `pregrasp_height_m`、`lift_height_m`、`slot_release_offset_m`；单位 m |
| 黑框和蓝框 | 桌面高度、黑框内边界、三个格子中心和格子底面高度 | 供视觉/临时人工输入产生螺母和格子的基座坐标 |
| 灵巧手 | 张开数组、适用于大/中/小螺母的三组闭合数组 | 配置 `hand_open` 和 `hand_closed[large/medium/small]`；每组是 6 个 `0..255` 值 |
| 灵巧手力度 | 合适的速度、力矩和张闭稳定等待时间 | 配置 `hand_speed`、`hand_force`、`grip_settle`；先从低速度、低力矩空载测试 |
| 相机外参 | 相机内参、相机到 `base_torso_root` 的外参 | 让 `VisionSystem` 输出基座坐标系下的螺母和格子位姿 |

O6 左手数组顺序固定为：

```text
[thumb_yaw, thumb_pitch, index, middle, ring, pinky]
```

手的位置、速度、力矩都是设备值 `0..255`，不是弧度。手没有夹持传感器；应以相机的
`check_nut_in_source()` 判断抓取是否成功。视觉检查发生在放置后的撤离位、返回 `above_table` 前，
因此相机标定时应确认该姿态下黑框完整可见。

以上参数均由 `lbot_task.launch.py` 加载，默认控制参数文件是工作区根目录的
`config/control/nut_task.yaml`；也可以通过 `control_config:=/绝对路径/文件.yaml` 指定副本。
视觉节点参数位于 `config/vision/nut_detector.yaml`，可通过
`vision_config:=/绝对路径/文件.yaml` 覆盖。
