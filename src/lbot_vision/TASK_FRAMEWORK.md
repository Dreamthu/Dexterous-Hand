# 黑框螺母任务代码框架

本文档描述当前工作区中 Gemini2 固定斜视相机对应的任务层设计。任务层必须与图像识别、机械臂驱动分离；识别调试阶段不能启动任何运动代码。

## 1. 节点分工

```text
Orbbec Gemini2
    │  /camera/color/image_raw
    │  /camera/depth/image_raw
    │  /camera/color/camera_info
    ▼
nut_detector_node
    │  /nut_detections       PoseArray: large, medium, small
    │  /nut_slots            PoseArray: slot 1, slot 2, slot 3
    │  /nut_detections/status String(JSON)
    ▼
nut_task_controller_node       （建议新增，默认 execute_task=false）
    │
    ├── /robot1/left_arm/move_pose       MoveJP
    ├── /robot1/left_arm/move_linear     MoveL
    ├── /robot1/left_hand/set_l6_joint   UInt8MultiArray
    ├── /robot1/left_arm/joint_states    JointState
    └── /robot1/right_arm/joint_states   JointState
```

固定相机意味着观察阶段不需要移动右臂。右臂全程保持自然下垂，抓取只使用左臂；如果现场指定另一只手，只通过参数切换服务和话题前缀。

## 2. 状态机

```text
IDLE
  │ execute_task=true 且安全门通过
  ▼
WAIT_SCENE ──(视觉无效/TF缺失)──> ABORT
  │ 连续 stable_frames 帧得到合法场景
  ▼
CHECK_START_POSE
  │ 左右臂均接近 natural_down_*_joints
  ▼
PLAN
  │ 只接受 base_frame 坐标；验证三个螺母和三个格子都在工作空间
  ▼
PICK_LARGE → PLACE_LARGE → VERIFY_LARGE
  ▼
PICK_MEDIUM → PLACE_MEDIUM → VERIFY_MEDIUM
  ▼
PICK_SMALL → PLACE_SMALL → VERIFY_SMALL
  ▼
COMPLETE
```

任意动作服务失败、关节状态离开允许范围、检测数量/顺序变化、急停触发或 TF 超时都进入 `ABORT`。`ABORT` 只停止后续动作并请求急停，不尝试自动恢复或重新使能。

## 3. 单颗螺母动作序列

对 `i = 0,1,2`，识别节点的 `poses[i]` 分别代表大、中、小：

1. `MoveJP(pregrasp)`：目标点上方 `pregrasp_height_m`，速度低，阻塞等待。
2. `MoveL(grasp)`：沿工具 Z 方向下降到螺母抓取高度；不使用图像坐标直接加固定世界 Z，抓取高度应由桌面平面/深度估计得到。
3. 发布闭合手指命令，等待 `grip_settle_ms`。
4. `MoveL(lift)`：垂直抬升到 `lift_height_m`，确认已离开桌面。
5. `MoveJP(slot_pregrasp[i])`：移动到对应格子上方。
6. `MoveL(slot[i])`：下降到格子放置高度。
7. 发布张开手指命令，等待物体落入格子。
8. `MoveL(slot_retreat[i])`：垂直撤离。
9. 重新获取视觉结果，确认源位置不再有该螺母且格子深度/轮廓发生变化。

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

- 相机状态为 `{"status":"ok"}`，连续 `stable_frames` 帧一致。
- `/nut_detections` 和 `/nut_slots` 的 `header.frame_id` 等于 `base_torso_root`。
- 三颗螺母排序为大、中、小，完整轮廓在黑框内，互不接触。
- 黑框、蓝筐和桌面高度均在标定范围内。
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

- `nut_detector_node`：已经实现，负责识别和发布结果，不会移动机械臂。
- `nut_task_node`：旧的实验性节点，包含占位观察位姿和占位放置坐标，不应直接用于比赛。
- 下一步应新增 `nut_task_controller_node`，只实现上述状态机，并保留 `execute_task=false` 的默认值。
- 在真实执行前，应先用假话题/录包测试控制器，再以低速、小幅度和空场景进行现场验证。
