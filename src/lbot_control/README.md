# lbot_control

`lbot_control` 只保留比赛所需的最小任务流程：左臂进入桌面、按大中小抓放、检查抓取结果、
收回左臂。

## 单包编译

在工作区根目录执行，包内最多同时运行 2 个编译任务：

```bash
source /opt/ros/jazzy/setup.bash
MAKEFLAGS="-j2" CMAKE_BUILD_PARALLEL_LEVEL=2 \
  colcon build --packages-select lbot_control --executor sequential
source install/local_setup.bash
```

单包编译要求本机已安装与当前源码匹配的 `lbot_motion`、`lbot_vision` 等依赖。
从其他机器复制的 `build/`、`install/` 可能包含旧机器的绝对路径和失效符号链接；
依赖需要重新构建时，可指定新的 `--build-base build/local_dependencies`，保留旧缓存。
工作区脚本通过自身位置定位根目录，`config/workspace.env` 中的机器人工作区使用 `.`，
直接读取本工作区的 `src/lbot_arm_interfaces`。

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

仅在 `route_control_hand: true` 时，每个方向的路线都会先向左 O6 下发 `[0, 0, 0, 0, 0, 0]`，再执行关节路线；只有该方向
路线成功后下发 `hand_open`，路线失败时先下发 `hand_open` 再请求左臂急停。`round_trip` 在 enter 成功后张手，leave 开始前
再次收手，leave 成功后再次张手。
手指顺序为 `[thumb_yaw, thumb_pitch, index, middle, ring, pinky]`，取值范围为 `0..255`。

### 已抓起螺母后的单次搬运

`config/control/slot_transfer.yaml` 保存本次 X 最大槽位上方的固定关节解。复用现有入口，
从当前状态直接执行一段 MoveJ，目标网页末端 XYZ 为
`[0.384756, -0.083086, -0.140000]` m，RPY 为 `[1.570796, 0.2, -2.6]` rad。
该配置的 `route_control_hand: false`，不会发送手指命令，也没有下降、松手或回程步骤。
仅等待关节到位后结束；运动失败走左臂急停路径，不自动解除急停。

当前独立运行的驱动使用 Fast DDS；调用端需要使用相同中间件。在工作区根目录运行：

```bash
source /opt/ros/jazzy/setup.bash
source install/local_setup.bash
RMW_IMPLEMENTATION=rmw_fastrtps_cpp ros2 launch lbot_control lbot_start_control.launch.py \
  start_driver:=false mode:=enter execute_motion:=true \
  config_file:="$(pwd)/config/control/slot_transfer.yaml"
```

`start_driver:=false` 复用现有驱动。此配置是已检查场景的固定目标，不会重新读取视觉。
`joint_limit_margin_rad` 默认仍为 `0.05`；本配置按用户选择设为 `0.0`，保留原始关节限位。

当前 `lbot_start_control.launch.py` 用于跑通基础路线；视觉已经通过
`RosVisionSystem` 接入，并可使用 `lbot_task.launch.py` 同时启动驱动、视觉节点和任务控制器。
完整任务执行仍需先填写实机路线、灵巧手和工具标定参数。

`lbot_task.launch.py` 默认同时打开检测画面 `/nut_detection/debug_image`，
显示黑框、螺母和料筐的检测标记；窗口只订阅图像，不会额外启动检测节点。
相机仍由终端 A 的 `./scripts/run_camera.sh` 启动，终端 B 继续发布已保存的外参。
终端 C 的无运动校验命令为：

```bash
ros2 launch lbot_control lbot_task.launch.py \
  start_driver:=true execute_task:=true task_mode:=validate
```

可加 `show_image:=false` 关闭窗口，或加 `image_topic:=/camera/color/image_raw`
查看原始彩色画面。任务控制器退出时，检测节点和图像窗口随本次 launch 一起关闭。
无图形桌面时使用 `show_image:=false`。

完整抓放视觉默认使用“3 秒内累计命中 3 次”的确认条件，允许中间短暂漏检，
不再要求相邻螺母的像素尺寸至少相差 10%。次数和时间窗口在
`config/vision/nut_detector.yaml` 中分别由 `sequence_stable_frames`、
`sequence_confirmation_window_ms` 控制。初始场景仍需三颗螺母与三个格子的
有效三维坐标；控制端等待同一帧的完整坐标，不会因中间的二维检测消息提前失败。
坐标系、关节限位、IK 和已有标定条件继续生效。
`task_mode:=pregrasp` 执行“单次捕捉大螺母坐标 → 示教起臂 → MoveJP 到保存的目标上方”，
再按 `approach_grasp_enabled` 和 `approach_place_enabled` 执行下述大螺母抓取、抬升和放置序列；
`full` 才执行大、中、小三颗螺母的完整任务。

### 先捕捉目标，再起臂和位姿接近

```bash
ros2 launch lbot_control lbot_task.launch.py \
  start_driver:=true execute_task:=true task_mode:=pregrasp
```

该模式先检查桌面路线标定和关节限位，等待 `/nut_detections/large` 的一次有效三维观测，
将大螺母的 `base_link` 坐标保存为本次固定目标。该独立话题按当前画面中的轮廓尺寸选取最大候选，
不等待时间窗口累计确认，不要求三颗螺母同时可见，也不依赖料筐和三个格子。仍要求有效深度、
相机到基座变换和新鲜图像；抓取前应保证大螺母可见，避免把剩余较小轮廓当作最大候选。
启用 `approach_place_enabled` 时，还会在起臂前接收 `/nut_slots` 的一次新鲜、完整的三坑位观测，
按 `approach_place_slot_selection: max_x` 选择 `base_link` 的 X 最大坑位，保存其坐标和原始编号。
设为 `farthest` 可按 `hypot(x, y)` 选择距基座原点最远的坑位。
该坑位话题独立于三颗螺母的数量、身份和稳定判定；任一坑位缺少有效三维坐标时等待新观测。
大螺母和坑位都保存后，执行与 `lbot_start_control.launch.py mode:=enter` 相同的 MoveJ 路线，
不会先检查后续三颗螺母的全部抓放 IK。到达最后一个示教节点并停稳后，读取左臂实际关节角，
通过 FK 获取末端位置和朝向，用此前保存的目标生成 MoveJP 终点。起臂后不再等待视觉，
后续遮挡和检测结果变化不会覆盖保存的大螺母及坑位坐标；从捕捉到放置完成期间应保持它们不动。
当前配置的 `approach_reference_z_rpy` 保存网页左臂 Arm_Tip 的 `[z, roll, pitch, yaw]`，
2026-09-10 22:28:23（本机时间）读取为
`[-0.3655917354267519, 1.0796495600495548, 0.12230988346329275, -1.184440455645692]`。
当前启用 `approach_target_is_tcp: true`，目标位置表示**左手掌心参考点**：
`p_palm = [大螺母 x, 大螺母 y, 保存的 z]`；RPY 仍使用保存的 Arm_Tip 朝向数值。
程序发送给 `move_pose` 的位置为 `p_Arm_Tip = p_palm - R(rpy) * tcp_offset`，
三个位置分量都参与补偿，RPY 不变。保存的 `z = -0.3655917354267519 m` 现在表示掌心高度，
不再表示机械臂末端法兰高度；到位后的 FK 高度因此会不同。

掌心参考点根据 `Dexterous-Hand/开发资源/assets` 中 O6 左手模型推导：
URDF 没有单独的掌心 TCP，因此取掌体 STL 的 Y/Z 包围盒中心，并沿 +X 掌侧取最外表面交点。
这是可调整的模型几何参考点，不是模型作者定义的抓取中心，也不是实测标定值。
手掌坐标系内的点约为 `[0.013985, 0.000761, 0.056365] m`；经过模型的手安装变换
`rpy=[3.1416, 0, -1.5708]`，得到 Arm_Tip 内偏移
`[-0.0007607737012547839, -0.013984802575456453, -0.05636500385395298] m`。
来源、网格哈希和算法记录见 `config/control/left_palm_tcp_model.json`；
`artifacts/calibration/tool/left_palm_tcp_model.png` 标出了所选参考点。
2026-09-10 22:54:28 采集了用户确认的“大螺母正上方”末端位置，用户确认螺母未移动，
采用上次任务保存的螺母 XY `[0.329154, 0.468984] m` 做了单姿态 XY 修正。
本次基座 XY 残差为 `[0.0666076534, 0.0005672473] m`，模型基座 Z 分量保持不变。
更新后的工具局部偏移为 `[0.011485531567894372, -0.009052663438149304, -0.12165361905422836] m`，
已用于运行配置；原始模型偏移仍保存在 `left_palm_tcp_model.json`。
采样姿态、目标来源、修正前后数据见 `config/control/left_palm_tcp_xy_alignment.json`。
计算为 `t_new = t_old + R_sample^T * [dx_base, dy_base, 0]`：
因此工具坐标系的三个分量都可能变化，但本次并未标定高度。
这是一组在采样姿态下满足 XY 对齐的等效偏移，不是完整的三维 TCP 标定，
也可能包含视觉/模型误差；迁移到不同朝向的精度未验证。
程序原有参考 Z 和 RPY 保持不变，不会自动改为本次采样的末端姿态。

需使用零工具偏移的 `Arm_Tip`（读取控制器时已确认），避免控制器工具帧再次补偿。
TCP 的轴方向沿用 Arm_Tip，只有原点平移，并未将 RPY 重新解释成 URDF 手掌坐标系的角度。

用原始模型重新推导（只读模型，不连接机器人）：

```bash
python3 scripts/derive_palm_tcp.py \
  --workstation ../Dexterous-Hand/开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf \
  --output config/control/left_palm_tcp_model.json \
  --plot artifacts/calibration/tool/left_palm_tcp_model.png
```

参考 z/RPY 是本次保存的固定值，下次启动不会自动重新读取网页。
完整抓放同样以这些 TCP 偏移换算目标位置，但继续使用完整抓放的独立 RPY 配置。
将 `approach_target_is_tcp` 设为 `false` 可恢复直接使用大螺母 x/y 和保存的末端 z/RPY；
将 `approach_reference_z_rpy` 设为空列表可恢复 enter 高度/朝向与 TCP 补偿方式。

接近段直接调用 `/robot1/left_arm/move_pose`（`MoveJP`），由驱动调用
`lbot_move_pose`，以阻塞方式执行控制器规划的运动。接近段不调用独立 IK 服务，
也不做逐点 IK 的关节裕量/跳变筛选。目标掌心 z 和末端 RPY 使用上述固定参考，
但 MoveJP 使用关节插补，不保证中途保持水平或固定朝向；应确保手臂运动空间有足够间隙。
速度、加速度分别使用 `joint_speed`、`joint_acceleration`。
仍检查示教路线和起点关节限位、到位状态、目标有限性、TCP 高度间隔
（至少 `pregrasp_height_m`）以及发命令前状态未变化。控制器负责该位姿的求解与执行。
当前配置启用 `approach_grasp_enabled: true`，接近后通过实际末端反馈确认到位，再继续执行：

1. 下发 `approach_hand_ready: [240, 30, 180, 180, 180, 180]`。
2. 等待 `grip_settle_ms`（当前 1000 ms）。
3. 通过阻塞 MoveL 将**网页末端 Arm_Tip** Z 改为 `approach_grasp_z_m: -0.380`，保持 X/Y/RPY 不变。
4. 实际末端反馈确认下降到位后下发 `approach_hand_close: [0, 30, 0, 0, 0, 0]`，等待相同时间。
5. 当前 `approach_place_enabled: true`：再次读取到位反馈，在实际末端 Z 上增加
   `approach_place_lift_m: 0.12` m，通过 MoveL 垂直抬升；X/Y/RPY 使用此时实际值。
6. 保存抬升到位的**实测 Arm_Tip Z/RPY**。按 `approach_place_route.names` 的顺序经过已配置的过渡点；
   关节值以当前 YAML 为准。`slot_transfer_planner: moveit` 时，每段使用 MoveIt 规划到指定七关节值，
   检查高度和碰撞后用 `joint_follow` 执行；设为 SDK 时这些过渡段仍使用 MoveJ。
   每个途经点都等关节稳定并复核实测状态后，再发送下一段。日志标注点名和 `1/2`、`2/2`。
7. 等待当前关节停稳，以实测关节作为 MoveIt 起点，计算启动前缓存的 **base_link X 最大槽位**上方目标。
   终点 Z 始终等于步骤 6 保存的抬升高度。先尝试原 RPY，再按旋转角从小到大尝试附近朝向；
   `approach_place_max_orientation_change_rad: 1.7453292519943295` 限制姿态差 100°。
   当前规划起点的实测朝向及其向原朝向靠近的候选一并参与，按角度差从小到大尝试；每次重算掌心 XY 补偿。
   当前 `slot_transfer_planner: moveit`：KDL 求逆解，OMPL RRTConnect 规划关节路径，TOTG 分配时间。
   路径经时间分配后再次进行限位和碰撞检查；找不到路径则停止，不退回 SDK IK 或 MoveJP。
8. 在起点、中点、终点用控制器 FK 核对 MoveIt 模型后，以 50 Hz 向
   `/robot1/left_arm/joint_follow` 发送七关节弧度值。连续执行期间检查反馈新鲜度、跟随误差和发送超时。
   最后确认实测关节和末端位姿到位。当前 `approach_place_release_enabled: true`，到位后发送
   `approach_hand_release: [240, 30, 180, 180, 180, 180]` 松手，等待 1000 ms 后结束。
   设为 false 只跳过槽位阶段的松手；任务退出收尾仍下发 `hand_open`。

截图示教点 J3=-2.715，实测约 -2.715038，超出旧任务层下限 -2.69；当前 YAML 的
`left_joint_min[2]` 为 -2.72，`joint_limit_margin_rad` 为 0，其余上下限保持原值。
这些参数只用于任务层检查，不写入控制器限位。所有途经点在启动时校验，任一点配置非法都在起臂前退出。
规划预算由 `approach_place_planning_timeout_ms: 15000` 限制（在服务调用间检查；单次调用另受
`service_timeout_ms` 限制）。运动失败或实测未到位时，先下发 `hand_open`，随后请求左臂急停并退出；
不等待 `grip_settle_ms`，也不以关节反馈有效为张手前提。张手发送失败仍尝试急停，分别记录错误。
正常任务结束也下发 `hand_open` 并等待 `grip_settle_ms`，随后才关闭 ROS。
已开始动作后的 C++ 异常退出同样执行张手、急停收尾；重复停止请求不会重复执行收尾。
纯校验、禁用执行及尚未开始动作的失败不发送手或臂命令。断电、强制终止或 ROS 已失效时无法保证收尾送达，
O6 无位置反馈，日志中的张手表示已发送命令。

这三个数组是左 O6 通道 1～6 的设备值（0..255），使用 `hand_speed`、`hand_force`；
与完整任务的 `hand_open` / `hand_closed_*` 配置独立。
该开关只启用用户指定的大螺母动作，不将完整任务的 `hand_calibrated` 标志改为 true。
`approach_grasp_z_frame: arm_tip` 表示 `-0.380` 是 base_link 下网页末端的绝对高度，
该下降终点直接使用这个 Z，不再扣除掌心偏移；上方接近目标仍使用掌心补偿。
以当前配置换算出的上方末端 Z 约 `-0.299320 m` 计算，下降约 80.7 mm。
显式选择 `palm` 可使用掌心高度解释。放置 XY 继续沿用 `approach_target_is_tcp`：
当前为 true，发送的末端 XY = 坑位 XY − **放置目标姿态**下旋转后的掌心偏移 XY，
使掌心/抓取中心对准坑位。不会将坑位深度或原来的接近参考 Z 用作放置高度。
省略 `approach_place_route` 和旧参数 `approach_place_waypoint` 可恢复直接搬运；只有此时
`approach_place_reference_z_rpy: [z, roll, pitch, yaw]` 才可设置独立的 Arm_Tip 终点 Z/RPY。
有途经点时忽略独立 Z/RPY 参数，以保证抬升高度不被覆盖。旧的单点 `approach_place_waypoint`
仍兼容，但不能与 `approach_place_route` 同时配置；路线中每个名称须唯一且有对应的七关节数组。
槽位选择 `farthest` 可恢复按 base XY 距离选择。
增加过渡位能改变求解初值，但不保证原高度的槽位目标可达。
MoveJP 只约束终点，不保证搬运中途保持水平或固定姿态。
每次 MoveJP / MoveL 返回成功后，都需要 `/robot1/left_arm/pose_states` 连续
`pose_stable_samples: 3` 帧新鲜反馈确认：位置误差不超过 `pose_tolerance_m: 0.001` m，
姿态四元数的角度误差不超过 `pose_tolerance_rad: 0.02` rad。坐标系必须为 `base_link`，
旧时间戳或无效反馈不能作为到位依据。等待超过 `pose_arrival_timeout_ms: 10000` 会中止，
不会发送下一步动作；日志会输出实际 XYZ、目标 XYZ 和误差。
手指通过话题下发，没有手指到位/抓住物体的传感器确认；等待时间不代表确认抓取成功。
到达槽位后不自动撤回。仅将 `approach_place_enabled` 设为 false 可恢复闭手后停止；
两个开关都设为 false 可恢复只到目标上方停止。放置开关要求同时启用抓取开关。

捕捉超时会在起臂前退出；起臂后若 MoveJP / MoveL 被拒绝、超时或状态异常，控制器会请求急停并退出，
不会自动重试或改发另一种运动。接近失败不会下发手指命令，下降失败不会继续闭手，
抬升或搬运失败不会松手。缺少大螺母或坑位坐标会在起臂前退出。
完整抓放仍需要正确的 TCP、抓取朝向和手参数。

只预览目标和检查示教路线、不执行运动时，使用：

```bash
ros2 launch lbot_control lbot_task.launch.py \
  start_driver:=true execute_task:=true task_mode:=validate_pregrasp
```

`validate_pregrasp` 以最后一个示教节点的关节角进行 FK 并计算接近、下降、抬升和坑位目标，
不调用 IK、MoveJP、MoveL，也不等待末端到位或下发手指命令；
成功仅表示配置和目标检查通过，不代表控制器已确认可达性或运动路径。
实际执行仍检查起臂后已到达示教关节节点，再发送组合后的目标。原有 `validate` 仍校验完整抓放计划。
任务 launch 使用 `control_config:=...` 指定配置，基础路线 launch 使用 `config_file:=...`；
两者默认均加载当前包安装的 `nut_task.yaml`，不需要写机器相关的绝对路径。

固定亮度阈值未找到黑框时，检测器会尝试局部对比度提取细边线，仍检查区域形状和内部亮度。
为容忍细黑框边线的短暂漏检，检测器默认保留最近识别到的黑框区域 3 秒
（`config/vision/nut_detector.yaml` 的 `frame_hold_ms`），画面会标记 `frame region held`。
期间每帧重新检测螺母并计算深度坐标；不会沿用旧的螺母位置。区域超时、
图像尺寸变化或时间戳回退后会失效。任务等待视觉场景的上限为 30 秒，
控制台会分别提示等待视觉、IK 校验和开始运动，便于定位停在哪一步。

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
| 相机外参 | 相机内参、相机到 `base_link` 的外参 | 让 `VisionSystem` 输出控制器基座坐标系下的螺母和格子位姿 |

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

### MoveIt 2 与 joint_follow

现有入口不变（先启动相机和外参发布）：

```bash
source /opt/ros/jazzy/setup.bash
source install/local_setup.bash
ros2 launch lbot_control lbot_task.launch.py \
  start_driver:=true execute_task:=true task_mode:=pregrasp
```

`slot_transfer_planner: moveit` 负责抬升后到槽位的搬运；当前按配置依次经过两个过渡点。初始接近仍用 MoveJP，
下降和抬升仍用 MoveL。抬升后的示教途经点也经过 MoveIt 规划并使用 joint_follow 执行。
显式传 `slot_transfer_planner:=sdk` 可选旧实现，
但新实现失败时不会自动切回。默认 `execute_task:=false` 不执行动作。

专用模型在启动时从同目录树内的 Dexterous-Hand CAD 生成，不改原始 URDF，也不硬编码用户名。
找不到模型时传 `moveit_model_source:=/实际路径/workstation.urdf`。
左臂七个 CAD 轴全部反向，使模型关节角直接对应控制器弧度值；基座根设为 `base_link`，
末端为 `arm_left_L8_Link`，对应零偏移 Arm_Tip。控制器 FK 一致性检查不通过则拒绝发送轨迹。
每段发送前校验起点、中点、终点的 SDK FK。若驱动已返回 `success=false`（例如 SDK TCP 超时），
同一只读查询间隔 150 ms、最多尝试 3 次，且重试前必须仍有新鲜、限位内的左臂反馈。
ROS 请求仍未返回、非有限结果或 FK 不一致时直接失败；三次查询失败也进入原急停路径。
不会重试运动、跳过 FK 检查或在失败后松手。日志显示失败采样点、轨迹时间和尝试次数。

当前碰撞模型包含躯干、左臂和手部凸包。`moveit_hand_collision_model: cad_swept` 使用 CAD 中
12 个左手碰撞网格、所有手指关节的完整范围（包含 mimic 范围），生成覆盖所有姿态的固定凸包，
再增加 10 mm 模型余量；不依赖手指反馈，也不把闭手命令当作实际手指位置。
生成器 `scripts/derive_hand_envelope.py` 对关节区间求解析极值；模型和来源校验值保存在
`config/control/left_hand_collision_envelope.json`。启动时核对 CAD 与网格校验值，来源变化必须重新生成。
这避免原大盒子空角处造成的碰撞误报；安装偏差、线缆和抓住物体的完整几何仍需另外建模。
显式选择 `box` 才使用旧的 `moveit_hand_envelope`，其值为 Arm_Tip 下的 `[中心x,y,z,尺寸x,y,z]`。
已测量的静态障碍可通过 `moveit_obstacle_boxes` 的多组 `[x,y,z,size_x,size_y,size_z]` 加入。

当前启用 `moveit_transfer_keep_above: true`，每段搬运分别约束 Arm_Tip 和手部碰撞模型最低点：
`Z_min = min(该段起点高度, 该段终点高度) - moveit_transfer_max_drop_m`，允许下探量当前为
`0.005` m。这会排除向下兜的路径，但不强制恒定高度或先额外抬升。凸包所有顶点都有约束，
避免旋转手腕时末端原点不降、手指模型却下探。OMPL 规划和时间化后的采样复核都使用同一高度下限；
起点重规划保留原下限。日志 `MoveIt transfer height check` 给出路径最低高度与约束值。

当前启用 `moveit_point_cloud_enabled: true`，订阅 `/camera/depth/points`。
`lbot_task.launch.py` 可传 `use_pointcloud:=false` 临时关闭本次任务的点云障碍检查，或传 `true` 开启；
不传则沿用 YAML。关闭后仍保留自碰撞、限位和高度检查，但不再依据点云避让环境物体。
该开关不改变任务阶段，`task_mode:=pregrasp` 仍从重新读取目标、起臂和抓取开始，不是持物续跑入口。
启用点云时，任务先缓存大螺母和槽位，
再在起臂前读取新点云，并按该帧时间戳通过 TF 转入 `base_link`。使用实测左臂关节进行自身过滤，
仅保留 `moveit_point_cloud_crop` 范围内的环境点；当前体素边长 15 mm，额外向周围膨胀 15 mm
（按整格向上取整）。点云缺失、过期、外参不可用或过滤后点数不足时，在 enter 前退出。
抬升后的两个过渡段及最终槽位段使用同一份 OctoMap，不会在手臂遮挡相机后重新取景。
地图通过 `~/moveit_planning_scene` 发布，供可视化检查。

这是运动前的静态环境快照：只覆盖相机实际看到且处于裁剪范围内的障碍，无法保证遮挡区域、
运动后新出现的障碍或被抓物体的完整几何已建模。初始 enter、接近和垂直抓取/抬升仍由 SDK 执行，
这些段尚未使用点云规划。要单独验证实时点云规划，可给下面的只读预览入口增加 `use_pointcloud:=true`；
此时起点关节还用于去除点云中的机器人自身，应填写采集时实际左臂关节值。

MoveIt 使用 KDL、OMPL RRTConnect 和 TOTG，路径时间化后再次检查位置限位、速度、加速度和采样碰撞。
先检查起终点关节之间的直接连接；满足同一碰撞和高度约束时直接时间化，否则交给 OMPL 搜索绕行路径。
`moveit_goal_preferred_clearance_rad: 0.005` 优先寻找同一目标 XYZ/RPY 下距离软限位至少 0.005 rad
的等价关节解，减少终点贴限位时实测误差触发停机的情况。这只是选解偏好；若无法优化，保留原有效解，
不会缩小模型范围或放宽命令、反馈的限位检查。设为 `0.0` 可关闭偏好。重规划保持已选定的终点关节解。
`moveit_joint_velocity_rad_s` / `moveit_joint_acceleration_rad_s2` 是绝对关节单位，当前上限均为 0.12。
跟随器按实际经过时间插值发送，不补发积压旧点；反馈超过 200 ms、跟随误差超过 0.08 rad、
发送延迟超过 60 ms 或目标未到位均报错，任务请求左臂急停并保持抓紧。
反馈失败日志包含接收年龄、样本序号和最近关节值，并区分超时与具体关节越界。
完整轨迹发送后，连续 3 个新鲜反馈满足关节到位条件，或其 FK 满足目标末端位置 1 mm、
姿态 0.02 rad 的条件，即进入驱动实际位姿的独立到位确认。这样允许冗余关节存在小偏差但末端
已经到位的情况；跟踪误差、反馈超时和软限位检查仍生效。最终必须通过驱动实际位姿确认才松手。
收尾超时会列出七关节的目标、实际值和误差，以及实际关节 FK 的位置/姿态误差。
急停接口超时表示停机请求未获确认，不代表停机已成功。
这是普通 ROS/Linux 定时发送，不是硬实时控制。驱动消息的 `follow` 布尔值目前被忽略，
停止依赖原有急停接口，不能把 `follow=false` 当成停机命令。

仅做离线规划（不启动驱动，不读取相机，不发送任何机械臂/手命令）：

```bash
ros2 launch lbot_control lbot_moveit_preview.launch.py \
  start_joints:='[2.681,3.158,-2.315,-1.074,1.293,-0.427,0.174]' \
  goal_pose:='[0.175644,0.520002,-0.259817,1.083344,0.122257,-1.185933]' \
  slot_xy:='[0.361268,-0.197967]'
```

以上是故障日志中的记录值，**不是实时场景**。提供 `slot_xy` 时，它表示掌心目标 XY，
`goal_pose` 只提供抬升后的 Arm_Tip Z/RPY 参考；不提供 `slot_xy` 时，直接规划到六维 Arm_Tip 位姿。
结果输出到 `/tmp/lbot_moveit_trajectory.csv`，也发布到预览节点的 `~/moveit_trajectory`；规划失败不写新轨迹。

起点同步：开始规划前等待关节在 500 ms 窗口内变化不超过 0.001 rad。若规划和 FK 核对后，
实测起点偏移超过 0.005 rad，最多两次从最新停稳位置重新规划到同一个终点；每次重新检查路径。
不会把旧轨迹首点直接改成当前位置，也不会直接跳回旧起点。日志打印各关节 planned/actual/delta。
重规划仅发生在第一条 joint_follow 指令之前；已经开始跟随后的错误仍请求急停退出。
