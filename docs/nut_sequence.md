# 螺母顺序身份与抓取状态

本文说明固定比赛任务使用的轻量顺序模块。它解决“第一颗取走后，画面只剩两颗时如何继续保持大/中/小身份”的问题，但**不是通用多目标跟踪器**，也不会发送机械臂运动命令。

尺寸校正与机器人 X 轴分格的实现见 [几何说明](perspective_sizing_and_robot_x_slots.md)。

## 1. 设计结论

本任务固定按“大 → 中 → 小”抓取，因此采用阶段驱动的 `3 → 2 → 1 → 0` 方案：

| 固定 ID | 标签 | 初始状态 |
|---:|---|---|
| 1 | `nut_large` | `pending` |
| 2 | `nut_medium` | `pending` |
| 3 | `nut_small` | `pending` |

- 第 0 阶段期望看见 3 颗，按透视校正后的毫米尺寸从大到小绑定 ID 1、2、3。
- 只有收到 ID 1 的明确 `complete` 后，才进入期望 2 颗的阶段；此时剩余两颗按透视校正后的毫米尺寸从大到小映射为 ID 2、3。
- 只有收到 ID 2 的明确 `complete` 后，才进入期望 1 颗的阶段；唯一观测固定映射为 ID 3。
- 只有收到 ID 3 的明确 `complete` 后，本轮才是 `round_completed`。

**检测数量只验证当前阶段，不能决定任务进度。** 例如初始只检测到 2 颗时，状态是 `observation_count_mismatch`，ID 1 仍为 `pending`，绝不能把“少了一颗”解释为“大螺母已完成”。

## 2. 稳定观测

默认采用时间窗口累计确认，当前帧满足以下条件且命中次数足够时，设置 `observation_valid=true`：

1. 黑框存在，图像尺寸和圆观测有效；
2. 当前观测数等于 `expected_count`；
3. 在最近 3 秒内，同一组位置累计被识别到 3 次，允许中间黑框丢失或检测数量不足；
4. 各帧目标中心可一一匹配，位移不超过 `sequence_max_center_shift_px`；半径波动不清空计数；
5. 时间戳严格递增，同一帧不能重复计数。过期命中不计入窗口。

当前默认不要求相邻校正尺寸至少相差 10%，仍按当前帧透视校正后的毫米尺寸从大到小排序。
漏检帧本身不会发布有效坐标；`retry`、`complete` 和 `reset` 会清空累计记录，下一阶段重新确认。
把 `sequence_confirmation_window_ms` 设为 `0.0` 可恢复连续帧模式，此时半径变化阈值和
`sequence_max_gap_ms` 重新参与稳定性判断。

默认参数位于唯一配置源 `config/vision/nut_detector.yaml`：

```yaml
sequence_stable_frames: 3
sequence_confirmation_window_ms: 3000.0
sequence_max_center_shift_px: 40.0
sequence_max_radius_change_ratio: 0.20
sequence_min_size_gap_ratio: 0.0
sequence_max_gap_ms: 1000.0
```

位置匹配仍使用像素，大小排序使用毫米尺寸，连续帧模式的尺寸变化比例也按毫米尺寸计算。相机视角、距离或倾角变化后需要现场重新验证，不代表真实物理尺寸分类。

## 3. 状态与事件

每个目标只有三种任务状态：

```text
pending -> start -> in_progress
in_progress -> complete -> completed
in_progress -> retry -> pending
```

事件通过 `/nut_detections/set_state` 的 `lbot_vision/srv/SetNutState` 发送：

- `start`：控制器准备开始抓当前目标；必须有最新稳定且未过期的可见观测。
- `complete`：控制器明确确认抓取/放置成功；这是唯一推进到下一阶段的方式。
- `retry`：本次动作失败，目标回到 `pending`，必须重新取得稳定观测才能再次 `start`。
- `reset`：开始新一轮，`target_id` 必须为 0，三个目标恢复 `pending`。

`session_id` 在视觉节点每次启动时随机生成，控制器必须使用状态话题中当前值；`round_id` 防止上一轮反馈污染新一轮；`event_sequence` 必须递增。最后一个已接受事件的完全相同重试会返回 `already_applied`，避免 service 重试重复推进。

目标在画面中消失、漏检或被手臂遮挡，只会令观测无效，**不会自动变成 `completed`**。`complete` 可以在动作结束后由外部控制器反馈；视觉节点本身不判断机械臂动作是否成功。

## 4. ROS 接口

### `/nut_detections/sequence`

类型：`lbot_vision/msg/NutSequenceState`

关键字段：

- `session_id`、`round_id`：当前节点会话和比赛轮次；
- `initialized`：是否曾通过初始三颗稳定识别；
- `observation_valid`：当前这一帧/阶段是否允许控制器使用；
- `observed_count`、`expected_count`：实际和当前阶段期望数量；
- `current_target_id`：当前只允许抓取的 ID，完成后为 0；
- `status`：`waiting/stabilizing/ready/in_progress/mismatch/completed` 等原因；
- `targets`：三个固定 ID 的标签、任务状态、可见性、像素观测及可选三维位置。

只有同时满足以下条件时，`targets[i].position` 才可作为当前定位结果：

```text
observation_valid == true
目标 visible == true
目标 position_valid == true
```

### `/nut_detections`

原有 `geometry_msgs/PoseArray` 继续保留，但现在只发布当前阶段剩余目标，顺序按固定 ID：

```text
初始：nut_large, nut_medium, nut_small
大完成后：nut_medium, nut_small
中完成后：nut_small
全部完成：空数组
```

`PoseArray` 没有 ID/状态字段，新的控制器应优先消费结构化的 `/nut_detections/sequence`。

### 手工接口示例

先查看当前 `session_id` 和 `round_id`：

```bash
ros2 topic echo /nut_detections/sequence
```

假设当前会话为 `SESSION`、轮次为 1：

```bash
ros2 service call /nut_detections/set_state lbot_vision/srv/SetNutState \
  "{session_id: SESSION, round_id: 1, event_sequence: 1, target_id: 1, action: start}"
ros2 service call /nut_detections/set_state lbot_vision/srv/SetNutState \
  "{session_id: SESSION, round_id: 1, event_sequence: 2, target_id: 1, action: complete}"
```

然后等待 `expected_count=2` 且 `observation_valid=true`，再对 ID 2 发送新的 `start`。不要预先一次性发送全部事件。

## 5. 模块边界

纯 C++ 核心位于：

```text
src/lbot_vision/include/lbot_vision/nut_sequence.hpp
src/lbot_vision/src/core/nut_sequence.cpp
```

它接收时间戳、图像尺寸、`cv::Vec3f(u,v,radius_px)` 观测及逐个对应的校正毫米尺寸 `sizes_mm`，不依赖 ROS、相机 SDK、深度、文件系统或机械臂。ROS 节点只负责消息转换、图像时效检查、深度定位和事件 service。

本实现刻意没有做：

- 通用跨帧最近邻/匈牙利匹配或 ReID；
- 通过物理直径分类；
- YOLO 模型；
- 根据目标消失自动判定成功；
- 机械臂规划、抓取或放置控制；
- 格子占用检测和“已放入对应格子”的视觉复核。

因此它适用于当前规则固定、每阶段只移除当前最大剩余螺母的任务。若将来允许任意顺序、目标移动、相机移动或螺母尺寸像素差不足，应替换为真实尺寸估计或通用关联方案，而不是继续增加阶段特例。

## 6. 现场验收

现场队友至少验证：

1. ROS Jazzy 完整 `./scripts/test.sh`、`./scripts/build.sh`；
2. 相机静止时初始 3 颗连续稳定后固定为 ID 1/2/3；
3. 遮挡或漏检到 2 颗不会自动推进；
4. `start -> retry` 后仍是同一阶段，重新稳定后才能再次开始；
5. ID 1 `complete` 后，剩余两颗映射为 `nut_medium/nut_small`；
6. ID 2 `complete` 后，最后一颗保持 `nut_small`；
7. 重复发送同一事件不会推进两次，旧 session/round/sequence 会被拒绝；
8. RGB/深度时间差、图像过期、无 TF、无有效深度时不发布伪造抓取位置；
9. 整个视觉节点不发送任何机械臂命令。

本地 Windows 环境只能验证 ROS-free 核心，不能代替上述 ROS、Gemini 2 和真机验收。
