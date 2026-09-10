# 外参配置预留

此目录只用于设备安装和赛场相关的外参，不保存相机内部标定参数。固定相机
eye-to-hand 方案已经采用；此目录不提交占位数值或虚构的变换。

当前已验证的固定相机外参位于：

```text
artifacts/calibration/extrinsics/20260910_eye_to_hand_v2/extrinsics.yaml
```

其父坐标系为控制器任务帧 `base_link`，子坐标系为
`camera_color_optical_frame`，TF 发布时挂接到 `camera_link`。

后续按实际方案分别建立有明确 frame 名、单位、测量方法和日期的配置，例如：

- 固定相机：相机光学坐标系到机器人基座（eye-to-hand）；
- 手眼相机：相机光学坐标系到末端执行器（eye-in-hand）；
- 末端工具：法兰/手腕到 TCP；
- 现场基准：机器人基座到桌面、料框或比赛场地坐标系。

Gemini 2 的彩色/深度内参、畸变参数和两传感器之间的内部外参不属于本目录。它们由
相机固件和 Orbbec 驱动提供，运行时从 `CameraInfo`/驱动注册结果直接使用。

新增外参实现时必须：

1. 明确父子 frame 和单位，并拒绝不完整或非有限数据；
2. 将求解工具放到 `tools/extrinsic_calibration/`，ROS 转换留在 adapter/node；
3. 把采集记录和求解产物写到 `artifacts/calibration/extrinsics/`；
4. 先以 TF 可视化和独立验证数据验收，再允许控制模块使用。
