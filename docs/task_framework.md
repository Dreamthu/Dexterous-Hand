# 黑框螺母任务流程

视觉节点 `nut_detector_node` 负责图像、深度、TF 和螺母/格子定位；运动任务节点
`nut_task_controller` 通过 `RosVisionSystem` 读取视觉结果，再由 `TaskCoordinator` 调用左臂
运动系统。视觉节点不直接发送机械臂命令。

```text
相机图像/深度/CameraInfo → nut_detector_node
  /nut_detections/sequence  NutSequenceState（固定 ID、阶段状态、三维位置）
  /nut_slots                 PoseArray（3 个格子，基座坐标）
  /nut_detections/set_state  SetNutState（start/complete/retry/reset）
          ↓
RosVisionSystem → TaskCoordinator → RosLeftArmMotionSystem → lbot_motion → 外部机器人驱动
```

## 状态机与视觉时机

```text
IDLE → MOVE_ABOVE_TABLE → PICK_AND_PLACE
                              ↓
                 放置撤离位执行相机检查
                              ↓
                       RETURN_ABOVE_TABLE
                         ↙             ↘
                    重试当前颗       下一颗 / RETRACT
```

抓取顺序固定为大、中、小。一次协调动作在机械层完成张手、预抓、下降、闭手、抬升、移动到格子、
释放和撤离；机械臂停在放置撤离位时，视觉判断当前螺母是否仍在黑框，然后才回到
`MOVE_ABOVE_TABLE` 的最后一个关节节点。因此相机检查不会被 `above_table` 姿态遮挡，且每次
抓放协调动作结束时机械臂都回到黑框上方。

视觉判断结果的处理方式：仍在黑框发送 `retry` 并重试当前尺寸；已移除发送 `complete` 并进入下一
尺寸；结果无效或超时则先返回 `above_table`，再进入 `ABORTED`。任务结束后 `RETRACT` 严格反向
执行进入桌面的所有关节节点。当前不检查右臂自然下垂、不读取右臂状态，也不检查蓝框放置结果；
灵巧手没有反馈传感器。

## 视觉输入契约

`RosVisionSystem` 使用结构化的 `/nut_detections/sequence`，不依赖不带 ID 的检测数组来猜测螺母身份：

- 初始场景要求 `initialized=true`、`observation_valid=true`、`expected_count=3`，三个目标均有
  `visible=true`、`position_valid=true` 的位置；
- 目标 ID 固定为 1/2/3，对应大/中/小；
- `/nut_slots` 必须包含 3 个 Pose，且其 `header.frame_id` 与目标位置均为 `base_link`；
- 所有坐标必须已经完成相机到机器人基座的 TF 转换。

每颗动作开始前，控制器发送带当前 `session_id`、`round_id` 和递增 `event_sequence` 的 `start`。
动作完成后，适配器等待新的视觉时间戳：当前阶段数量未减少且目标可见时发送 `retry`；数量恰好
减少一颗并得到 `observation_count_mismatch` 时发送 `complete`。短暂的无图像、TF 或深度异常会继续
等待，直到视觉超时。

## 运动动作

`MOVE_ABOVE_TABLE` 和 `RETRACT` 使用 `MoveJ` 执行 YAML 中的 7 关节节点；节点之间的轨迹由机器人
驱动插补。抓放跨区域移动使用 `MoveJP`，接近桌面、抬升、释放和撤离使用 `MoveL`。路线配置位于
`config/control/nut_task.yaml`。

## 启动

基础路线测试：

```bash
ros2 launch lbot_control lbot_start_control.launch.py mode:=round_trip execute_motion:=true
```

连接视觉的完整任务入口：

```bash
ros2 launch lbot_control lbot_task.launch.py execute_task:=true
```

`execute_task` 默认是 `false`。执行前必须完成机械臂路线、工具坐标、抓放高度、灵巧手开合值、
相机内外参和 TF 标定。旧的 `src/lbot_vision/experimental/nut_task_node.cpp` 含占位坐标，不得用于
比赛运行。
