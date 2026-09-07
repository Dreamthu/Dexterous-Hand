# 躯干固定 Gemini2 外参标定

## 1. 生成并打印标定板

```bash
./scripts/calibrate_extrinsics.sh generate --output artifacts/calibration/extrinsics/board
```

输出 `board_a4.pdf`、`board_a4.png` 和 `board_geometry.yaml`。打印 PDF 时选择：

- 纸张：A4 纵向；
- 缩放：Actual size / 100%；
- 关闭 Fit、Shrink、Borderless scaling；
- 打印后用图中的水平、垂直 100 mm 标尺复核，误差超过 0.5 mm 就不能使用。

将纸张粘在刚性薄板上。把右手拆下，让 `arm_right_R8_Link` 的朝手面（R8 的 `z=0` 面）
直接贴在纸面右下角的矩形定位区。四个圆孔是侧伸支架的 Ø3.5 mm 孔，孔号 1→4 沿
R8 的 `+Y8` 方向排列；圆盘方向和 `+X8/+Y8` 箭头必须一致。不要使用圆盘上的六个孔。

## 2. 填写同姿态关节角

拆手并贴合后，保持机械臂不动，读取七个右臂关节的真实值（URDF 弧度）并填写：

```yaml
# config/calibration/right_arm_joints.yaml
joint_positions_rad:
  arm_right_R1_Joint: ...
  arm_right_R2_Joint: ...
  arm_right_R3_Joint: ...
  arm_right_R4_Joint: ...
  arm_right_R5_Joint: ...
  arm_right_R6_Joint: ...
  arm_right_R7_Joint: ...
```

程序会通过 `workstation.urdf` 的 FK 计算 `base_torso_root → arm_right_R8_Link`，不使用
桌面高度，也不会假定 URDF 的 `world` 就是真实地面。

## 3. 离线单张图求解

先启动相机，保存一张能看到上方 ArUco 和右下定位区的彩色图，同时导出同一时刻的
`/camera/color/camera_info` 为 YAML。然后运行：

```bash
./scripts/calibrate_extrinsics.sh solve \
  --image /path/to/color.png \
  --camera-info /path/to/camera_info.yaml \
  --joints config/calibration/right_arm_joints.yaml \
  --board-geometry artifacts/calibration/extrinsics/board/board_geometry.yaml
```

如需用录制的同步 IMU 检查这张图像是否在静止窗口内，追加：

```bash
  --imu-file /path/to/imu_samples.yaml --image-stamp-ns <rgb_stamp>
```

`<rgb_stamp>` 必须是图像原始时间戳；程序会检查以该时间为结束点、按配置回看的 1 秒窗口。
如需单独检查任意 IMU 时间段，可使用 `check-imu` 子命令：

```bash
./scripts/calibrate_extrinsics.sh check-imu \
  --imu-file /path/to/imu_samples.yaml \
  --start-stamp-ns <start> --end-stamp-ns <end>
```

程序会检查标签尺寸、PnP 重投影误差、正深度、URDF 关节限位和 R8 网格 SHA-256，结果写入
`extrinsics.yaml`。核心变换为：

```text
T_base_board = T_base_R8(q) · T_R8_board
T_base_camera = T_base_board · inverse(T_camera_board)
```

## 4. 发布 ROS TF

确认 Orbbec 驱动已经发布 `camera_link → camera_color_optical_frame` 后：

```bash
./scripts/calibrate_extrinsics.sh publish \
  --result artifacts/calibration/extrinsics/<timestamp>/extrinsics.yaml
```

检查：

```bash
ros2 run tf2_ros tf2_echo base_torso_root camera_color_optical_frame
```

程序只发布静态 TF，不发送任何机械臂运动命令。当前离线模式需要操作者保证图像和关节角
属于同一静止姿态；硬件接通后可使用 `capture` 子命令连续采集 25 帧并做稳定性检查。

实时采样并加入 IMU 静止检查：

```bash
./scripts/calibrate_extrinsics.sh capture \
  --joints config/calibration/right_arm_joints.yaml \
  --board-geometry artifacts/calibration/extrinsics/board/board_geometry.yaml \
  --with-imu
```

IMU 只用于检查标定时相机和 R8 是否静止，不参与 PnP 外参计算；通过后原始窗口保存为
结果目录中的 `imu_samples.yaml`。

## 5. 多姿态重复标定

连续采集同一姿态的 25 帧只能降低图像角点的随机噪声，不能修正打印缩放、孔位误差、
R8 与纸面没有完全贴平、机器人关节零位误差或相机支架变形。更高精度的流程是：

1. 保持标定板与 R8 四孔刚性贴合；
2. 让右臂取 5～10 个不同姿态，每个姿态停稳后采集 15～30 张图像；
3. 每组记录同一时刻的七个关节角、图像时间戳和 ArUco 角点；
4. 对所有姿态联合优化一个公共的 `T_base_camera`，使用 Huber/RANSAC 剔除错误帧；
5. 如果不同姿态的结果出现有规律的偏差，再单独估计关节零位偏置，不能直接把偏差平均掉。

联合模型为：

```text
P_camera(i) = T_camera_base · T_base_R8(q_i) · T_R8_board · P_board
```

重复拆装标定板只能测量重复性；如果每次贴合位置不同，误差会变成系统误差。最终使用的
标定板应一次贴牢，并在检测到 R8 受力、松动或相机支架移动后重新标定。

## 6. Gemini2 IMU

当前 [gemini2.yaml](../config/camera/gemini2.yaml) 已按
`OrbbecSDK_ROS2/v2-main` 接口启用同步 IMU。`camera_name:=camera` 时：

```text
topic: /camera/gyro_accel/sample
type: sensor_msgs/msg/Imu
frame_id: camera_accel_gyro_optical_frame
```

驱动源码明确将 `orientation_covariance[0]` 设为 `-1`，因此消息没有可用姿态，只能使用
`linear_acceleration` 和 `angular_velocity`。现场仍应核对设备实际支持的 profile：

```bash
ros2 topic list | grep -E 'accel|gyro|imu'
ros2 topic echo --qos-reliability best_effort /camera/gyro_accel/sample
ros2 topic hz /camera/gyro_accel/sample
```

如果设备不支持同步输出，关闭 `enable_sync_output_accel_gyro` 后会发布 `/camera/accel/sample`
和 `/camera/gyro/sample`。SDK 对这些独立流使用设备时间戳，不能直接与当前全局时钟的图像
时间戳进行窗口匹配，因此本标定程序不会把它们当作同步 IMU 的替代输入。

静止时可使用：

- 加速度计的重力方向约束相机的 roll/pitch，并作为 ArUco PnP 的弱先验；
- 陀螺仪角速度判断相机/机械臂是否已经停稳，剔除振动帧；
- 加速度窗口均值和协方差作为噪声权重。

IMU不能提供可靠的绝对 yaw、平移或桌面位置。陀螺积分会漂移，重力也无法确定绕重力轴的
旋转，因此 IMU 不能替代 ArUco。只有确认 `camera_accel_gyro_optical_frame` 与颜色光学
坐标系之间存在正确 TF、图像和 IMU 时间同步且相机刚性固定时，才应把它加入联合优化；
否则只用于稳定性门控。
