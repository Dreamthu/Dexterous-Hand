# 机械臂运动模块协作说明

本工作区的比赛任务是：
比赛规则：
  • 黑框布置在桌面左侧，蓝色螺母筐布置在右侧；二者靠近参赛者一侧的边缘均与桌边齐平，边线相互平行。
  • 黑框右侧外边界与蓝色螺母筐左侧外边界的最近水平距离为【50 cm】。
  • 三颗螺母均单层【随机】平放；全部外轮廓须位于黑框内边界以内，不得压线、叠放或相互接触。
  • 抓取时，需要按照“大 中 小”的顺序去抓取，然后按照顺序放到蓝色螺母框的三个格子内（如图）。
  • 机器人每轮双臂均从自然下垂开始，双手不得接触桌面或任何道具。
左臂按“大、中、小”顺序从左侧黑框抓取三颗螺母，依次放入右侧蓝色螺母筐的三个格子。黑框与蓝框最近水平距离为 50 cm。当前仅实现左臂和左 O6 灵巧手；右臂不参与。

## 模块边界

- `src/lbot_motion`：仅 ROS 设备适配层。封装左臂 `MoveJ`、`MoveJP`、`MoveL`、IK、左臂关节
  状态、左手 O6 位置/速度/力矩话题及急停；不得加入比赛状态机、视觉判断或任务路线。
- `src/lbot_control`：比赛任务状态机、任务协调、抓放位姿生成和桌面路线。它是唯一依赖
  `lbot_motion` 的任务层。
- `src/lbot_control/src/ros_vision_system.cpp`：将 `lbot_vision` 的
  `/nut_detections/sequence`、`/nut_slots` 和 `/nut_detections/set_state` 适配到
  `VisionSystem`；不在视觉节点中发送运动命令。
- 视觉模块以后只实现 `lbot_control::VisionSystem`，不直接发送机械臂命令。其输入输出约定见
  `src/lbot_control/include/lbot_control/vision_system.hpp`。

## 必须保持的任务行为

```text
IDLE
  -> MOVE_ABOVE_TABLE
  -> PICK_AND_PLACE
  -> CHECK_PICK_RESULT
  -> RETURN_ABOVE_TABLE
  -> 下一颗 / 重试当前颗 / RETRACT
  -> COMPLETE
```

- 抓取顺序固定为大、中、小。
- 不检查双臂是否自然下垂，也不读取右臂状态。
- 一次抓放的机械动作是：张手、预抓、直线下降、闭手、直线抬升、到格子上方、直线下降、张手、
  直线撤离。
- 相机检查在“放置撤离位”执行、机械臂返回 `above_table` **之前**，避免 `above_table` 姿态遮挡
  黑框。检查只判断当前尺寸螺母是否仍在黑框，不检查蓝框中的放置结果。
- 对任务协调者而言，一次 `PICK_AND_PLACE` 完成后必定回到 `MOVE_ABOVE_TABLE` 的最后一个关节节点；
  即便视觉检查失败，也先尝试回到该位置再中止。
- `RETRACT` 必须严格反向执行 `MOVE_ABOVE_TABLE` 的完整关节节点路线。
- 运动、视觉或取消失败进入 `ABORTED`；停止路径使用左臂急停。

## 运动方式

- `MOVE_ABOVE_TABLE` / `RETRACT`：用户在 YAML 配置任意数量的 7 关节节点；控制层逐段调用
  `MoveJ` 并等待关节稳定。每段的轨迹插补由机器人驱动完成。
- 抓放的跨区域移动：`MoveJP`，输入末端 `x,y,z,roll,pitch,yaw`，由驱动进行逆解和关节插补。
- 抓取、抬升、放置、撤离：`MoveL`，末端直线运动。
- 左 O6 手一次下发 6 元素的位置、速度、力矩数组，顺序为
  `[thumb_yaw, thumb_pitch, index, middle, ring, pinky]`；取值为设备 `0..255`，不是弧度。没有手指
  位置反馈或夹持传感器，闭手/张手后只等待 `grip_settle`。

## 坐标与视觉接口

- 任务层只接收机器人控制器基座坐标系 `base_link` 下的结果，不能把相机像素或相机坐标直接传给
  抓放规划。
- `initial_scene()` 返回按大、中、小排列的三颗螺母位姿和三个格子位姿。
- `check_nut_in_source(NutSize)` 在放置撤离位调用，返回当前螺母是否仍在黑框。
- 抓放规划以检测到的螺母/格子位姿加高度偏移生成；参见
  `src/lbot_control/src/motion_plan.cpp`。

## 启动与安全

- 一律使用 ROS 2 launch 启动，不创建或使用独立 script 作为运动入口：
  `ros2 launch lbot_control lbot_start_control.launch.py`。
- 默认 `execute_motion:=false`，仅检查配置，不发送机械臂命令。
- 实机路线必须先在 `config/control/nut_task.yaml` 填入示教关节角，确认无误后才将
  `table_route_calibrated` 设为 `true`。
- 使用低速验证，先 `mode:=enter`，再 `mode:=leave`，最后才 `mode:=round_trip`。不要把 YAML 中
  的零值占位节点用于实机执行。

## 当前状态与后续工作

- `lbot_motion`、`lbot_control` 已可构建；`lbot_control` 有状态机、任务协调、路线和抓放规划测试。
- `lbot_start_control.launch.py` 仍用于验证 `MOVE_ABOVE_TABLE` / `RETRACT` 基础路线；新增
  `lbot_task.launch.py` 会启动驱动、`nut_detector_node` 和已连接的 `nut_task_controller`。
- 完整任务执行前仍需填写实机路线、手参数和工具/视觉标定；`execute_task` 默认关闭。
- 完整抓放接入前，需要标定并加载：桌面路线、工具抓取中心与朝向、预抓/抬升/释放高度、O6 张开值、
  大中小闭合值、手速度/力矩、等待时间，以及相机到基座的外参。
- 保持现有工作区的其他改动；`build/`、`install/`、`log/` 中存在用户已有的脏改动，不要清理或回退。
