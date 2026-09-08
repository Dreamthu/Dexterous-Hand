# 黑框螺母任务流程

任务层只保留三个外部输入：三颗螺母位姿、三个格子位姿，以及抓放后“目标螺母是否仍在
黑框内”的判断。视觉消息、检测算法和 TF 转换放在后续视觉适配器中。

```text
视觉适配器                         lbot_control
-----------                        ------------
initial_scene() ------------------> TaskCoordinator
check_nut_in_source() ------------> CHECK_PICK_RESULT
```

状态机为：

```text
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

当前没有视觉实现。后续只需：

1. 实现 `VisionSystem::initial_scene()`，返回基座坐标系下的 `SceneObservation`；
2. 实现 `VisionSystem::check_nut_in_source(NutSize)`；
3. 在 ROS 任务节点中创建 `TaskCoordinator` 并循环调用 `step()`。

视觉适配器不直接发送机械臂命令，运动层也不依赖视觉消息类型。
