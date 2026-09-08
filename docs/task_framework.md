# 黑框螺母任务流程

任务层只保留三个外部输入：三颗螺母位姿、三个格子位姿，以及抓放后“目标螺母是否仍在
黑框内”的判断。视觉消息、检测算法和 TF 转换放在后续视觉适配器中。

```text
<<<<<<< HEAD
视觉适配器                         lbot_control
-----------                        ------------
initial_scene() ------------------> TaskCoordinator
check_nut_in_source() ------------> CHECK_PICK_RESULT
=======
Orbbec Gemini2
    │  /camera/color/image_raw
    │  /camera/depth/image_raw
    │  /camera/color/camera_info
    ▼
nut_detector_node
    │  /nut_detections          PoseArray: 当前阶段剩余目标，按固定 ID 排序
    │  /nut_detections/sequence NutSequenceState: ID、状态、期望数量和可选三维位置
    │  /nut_detections/set_state SetNutState: start/complete/retry/reset 外部反馈
    │  /nut_slots               PoseArray: slot 1, slot 2, slot 3
    │  /nut_detections/status   String(JSON，兼容诊断)
    ▼
nut_task_controller_node       （仍待实现，默认 execute_task=false）
    │
    ├── /robot1/left_arm/move_pose       MoveJP
    ├── /robot1/left_arm/move_linear     MoveL
    ├── /robot1/left_hand/set_l6_joint   UInt8MultiArray
    ├── /robot1/left_arm/joint_states    JointState
    └── /robot1/right_arm/joint_states   JointState
>>>>>>> origin/branch
```

状态机为：

```text
<<<<<<< HEAD
IDLE -> MOVE_ABOVE_TABLE -> PICK_AND_PLACE -> CHECK_PICK_RESULT
                              ^                    |
                              |                    v
                              +------------- RETURN_ABOVE_TABLE

CHECK_PICK_RESULT -- 仍在黑框 ---------> RETURN_ABOVE_TABLE -> PICK_AND_PLACE（重试）
CHECK_PICK_RESULT -- 已取走且还有目标 --> RETURN_ABOVE_TABLE -> PICK_AND_PLACE（下一尺寸）
CHECK_PICK_RESULT -- 三颗均取走 -------> RETURN_ABOVE_TABLE -> RETRACT -> COMPLETE
```

抓放顺序固定为大、中、小。一次 `PICK_AND_PLACE` 连续完成抓取和放置；在放置撤离位（相机
可见且尚未回到黑框上方）立即检查源黑框，然后回到 `MOVE_ABOVE_TABLE` 的最终关节位置。
若目标仍在，重复当前目标。任务结束后，`RETRACT` 反向执行进入桌面的全部关节节点。
=======
IDLE
  │ execute_task=true 且安全门通过
  ▼
WAIT_SCENE ──(视觉无效/TF缺失)──> ABORT
  │ sequence.initialized=true、observation_valid=true、current_target_id=1
  ▼
CHECK_START_POSE
  │ 左右臂均接近 natural_down_*_joints
  ▼
PLAN
  │ 只接受 base_frame 坐标；验证三个螺母和三个格子都在工作空间
  ▼
START_LARGE → PICK/PLACE_LARGE → COMPLETE_LARGE
  │ 等待 expected_count=2 且 current_target_id=2 的新稳定观测
  ▼
START_MEDIUM → PICK/PLACE_MEDIUM → COMPLETE_MEDIUM
  │ 等待 expected_count=1 且 current_target_id=3 的新稳定观测
  ▼
START_SMALL → PICK/PLACE_SMALL → COMPLETE_SMALL
  ▼
COMPLETE
```

控制器开始每颗动作前向视觉 service 发送 `start`，动作失败发送 `retry`，明确验证成功后才发送
`complete`。检测数量不符合当前 `expected_count` 时停止抓取并等待/超时进入 `ABORT`，绝不能据此
发送 `complete`。任意动作服务失败、关节状态离开允许范围、急停触发或 TF 超时都进入 `ABORT`。
`ABORT` 只停止后续动作并请求急停，不尝试自动恢复或重新使能。
>>>>>>> origin/branch

当前没有视觉实现。后续只需：

<<<<<<< HEAD
1. 实现 `VisionSystem::initial_scene()`，返回基座坐标系下的 `SceneObservation`；
2. 实现 `VisionSystem::check_nut_in_source(NutSize)`；
3. 在 ROS 任务节点中创建 `TaskCoordinator` 并循环调用 `step()`。

视觉适配器不直接发送机械臂命令，运动层也不依赖视觉消息类型。
=======
对固定 ID `i = 1,2,3`，控制器从 `/nut_detections/sequence` 选择
`current_target_id == i` 且 `observation_valid/visible/position_valid` 都为真的目标。对应格子索引为
`i-1`；不要根据可变长度 `PoseArray` 的下标重新猜身份：

1. `MoveJP(pregrasp)`：目标点上方 `pregrasp_height_m`，速度低，阻塞等待。
2. `MoveL(grasp)`：沿工具 Z 方向下降到螺母抓取高度；不使用图像坐标直接加固定世界 Z，抓取高度应由桌面平面/深度估计得到。
3. 发布闭合手指命令，等待 `grip_settle_ms`。
4. `MoveL(lift)`：垂直抬升到 `lift_height_m`，确认已离开桌面。
5. `MoveJP(slot_pregrasp[i])`：移动到对应格子上方。
6. `MoveL(slot[i])`：下降到格子放置高度。
7. 发布张开手指命令，等待物体落入格子。
8. `MoveL(slot_retreat[i])`：垂直撤离。
9. 由控制器结合动作返回、源位置和格子复核决定结果：成功则发送 `complete`，失败发送 `retry`。
   仅仅“源位置目标消失”不能作为成功证据。

格子顺序必须由蓝色筐的图像长边方向确定，不能用固定的 `x + 0.08 * i`。`nut_detector_node` 已发布 `/nut_slots`，控制器应使用其实际三维坐标。

## 4. 安全门

控制器启动时默认只监视，不执行运动：

```yaml
execute_task: false
enable_on_start: false
require_emergency_clear: true
require_both_arms_natural_down: true
```

只有操作者显式设置 `execute_task:=true`，且满足以下条件才允许第一条运动命令：

- `/nut_detections/sequence` 为当前 session/round，且 `observation_valid=true`。
- `/nut_detections` 和 `/nut_slots` 的 `header.frame_id` 等于 `base_torso_root`。
- 初始阶段三颗按像素尺寸稳定绑定固定 ID；后续阶段实际数量等于 `expected_count`。
- 黑框、蓝筐和桌面高度均在已验证的工作范围内。
- 左右臂关节角均在各自 `natural_down_*_joints` 的容差内。
- 所有预抓点、抓取点、抬升点和放置点通过工作空间/碰撞检查。
- 急停已清除；控制器不自动清除急停。

## 5. 建议的控制器参数

```yaml
nut_task_controller_node:
  ros__parameters:
    execute_task: false
    pick_arm: left
    base_frame: base_torso_root
    natural_down_left_joints: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    natural_down_right_joints: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    joint_tolerance_rad: 0.08
    stable_frames: 8
    action_speed: 0.10
    action_accel: 0.10
    pregrasp_height_m: 0.08
    lift_height_m: 0.15
    grasp_clearance_m: 0.012
    place_clearance_m: 0.04
    grip_settle_ms: 500
    service_timeout_ms: 3000
    verify_timeout_ms: 3000
    max_xy_error_m: 0.006
    required_gap_m: 0.50
```

自然下垂关节角、工具中心点偏置、桌面高度和抓取姿态必须由现场标定填写，不能沿用示例零值。

## 6. 当前工作区的使用边界

- `nut_detector_node`：已经实现二维识别、3→2→1 固定任务身份/状态、深度定位和显式反馈
  service，不会移动机械臂。
- `nut_task_node`：旧的实验性节点，包含占位观察位姿和占位放置坐标，不应直接用于比赛。
- 下一步应新增 `nut_task_controller_node`，消费结构化 sequence 状态并回传事件；仍须保留
  `execute_task=false` 的默认值。
- 在真实执行前，应先用假话题/录包测试控制器，再以低速、小幅度和空场景进行现场验证。
>>>>>>> origin/branch
