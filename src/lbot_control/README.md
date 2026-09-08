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

当前 launch 用于跑通基础路线。完整抓放 launch 要等视觉适配器提供初始位姿和
`VisionSystem` 后再接入；核心状态机和抓放执行器不需要随视觉实现修改。
