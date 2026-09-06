# 常见问题 FAQ

## 进不去 Web 控制平台

默认地址：

```text
http://192.168.10.21:8000
```

先检查：

```bash
ping 192.168.10.21
```

常见原因：

| 原因 | 处理 |
| --- | --- |
| 电脑不在同一网段 | 设置为 `192.168.10.x` |
| 网线没接好 | 检查网口灯 |
| 控制器没启动完 | 等一会儿再刷新 |
| IP 被改过 | 问现场或看配置 |

## ROS2 服务找不到

先启动驱动：

```bash
ros2 launch lbot_driver lbot_start_driver.launch.py
```

再看服务：

```bash
ros2 service list | grep robot1
```

注意默认有 `/robot1` 命名空间。

## 机械臂不动

可能原因：

| 原因 | 处理 |
| --- | --- |
| 没使能 | 调 `set_enable` |
| 急停中 | 先恢复急停 |
| 目标点太远 | 换近一点的点 |
| 关节角单位错了 | 要用 rad，不是度 |
| 速度太大或太小 | 先用 `0.2~0.5` |

## MoveJ 报错

MoveJ 的 `joints` 必须是 7 个值：

```text
[J1, J2, J3, J4, J5, J6, J7]
```

例子：

```bash
ros2 service call /robot1/left_arm/move_joint lbot_arm_interfaces/srv/MoveJ \
"{joints: [0, 0, 0, 0, 0, 0, 0], speed: 0.3, acce: 0.3, block: true}"
```

## 灵巧手不动

检查：

- 话题是否带 `/robot1`。
- L6/O6 数据长度是否为 6。
- 是否先设置了速度和力矩。
- 线缆和供电是否正常。

## RViz 模型看不到

模型在：

```text
开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf
```

当前 SDK 没有写好的 RViz launch，需要自己启动 `robot_state_publisher` 和 `rviz2`。

## 逆解失败

常见原因：

- 目标点超出机械臂范围。
- 姿态角不合理。
- 初始关节角离目标太远。
- 工具坐标系没有设置对。

处理方式：先用当前末端附近的小位移测试，再慢慢扩大动作范围。
