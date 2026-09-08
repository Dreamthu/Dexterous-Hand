# lbot_motion

`lbot_motion` 是机械臂驱动与任务控制器之间的窄适配层。它不包含比赛状态机、视觉逻辑、
固定动作时间轴或任务路点。

## 职责

唯一公共类为
[`LeftArmMotionDevice`](include/lbot_motion/left_arm_motion_device.hpp)：

- 订阅左臂关节状态；
- 封装左臂 `MoveJ`、`MoveJP`、`MoveL` 和逆解服务；
- 发布左 O6 手的位置、速度和力矩；
- 只提供左臂急停请求，不会自动解除急停。

包内不存在任何右臂状态订阅、运动客户端或右手控制发布器。

## 与 lbot_control 的边界

```text
lbot_control                         lbot_motion
-------------------------------      ----------------------------
状态机和大→中→小顺序                 ROS service/topic 名称
可配置桌面路线与抓放动作             左臂 MoveJ/MoveJP/MoveL 请求
IK结果和关节范围检查                  左臂 IK 请求
抓手配方选择                         左 O6 原始六通道发布
异常决策                             左臂急停请求
```

`lbot_control::RosLeftArmMotionSystem` 是两层之间的适配器。桌面路线的节点数量和内容均在
控制层配置；本包不会保存路点。状态机仅依赖
`lbot_control::MotionSystem` 抽象接口，不会直接访问本包或 ROS。

## 调用约束

阻塞服务调用应在工作线程中执行，同时由另一个 ROS executor 线程处理 service response 和
关节状态回调。参数单位如下：

- 关节和欧拉角：rad；
- 笛卡尔位置：m；
- O6 位置、速度、力矩：0～255。

本包只负责可靠转发命令，不负责碰撞检查。真实动作必须先由 `lbot_control` 完成整条路线的
工作空间、IK、关节限位和外部碰撞验证。
