# 顶置 Gemini 2 + 右腕 ArUco 外参标定

## 1. 本装置采用的方案

从现场照片和仓库资料可确定：Gemini 2 固定在机器人顶部向下观察桌面，ArUco 板固定在
右手手腕，螺母最终要在机器人基坐标中定位。因此这是 **eye-to-hand（眼在手外）**，不是
eye-in-hand。

使用的坐标系和观测量如下：

| 符号 | 程序中的含义 | 来源 |
| --- | --- | --- |
| `B` | `base_torso_root` | 机器人共同基座 |
| `G` | `arm_right_R8_Link`/驱动当前右臂末端 | 右臂 `pose_states` |
| `C` | `camera_color_optical_frame` | 彩色 `CameraInfo` |
| `M` | 3×3 ArUco 板坐标系 | OpenCV GridBoard |
| `T_B_C` | 将相机点变到机器人基座的外参 | 本工具的最终结果 |

每个静止姿态满足：

```text
T_B_G(i) · T_G_M = T_B_C · T_C_M(i)
```

其中 `T_B_G(i)` 来自右臂状态，`T_C_M(i)` 由 ArUco 角点和相机出厂内参做 PnP 得到；
`T_B_C` 与板相对手腕的固定安装变换 `T_G_M` 同时求出。结果方向明确为：

```text
p_base = R_base_camera · p_camera + t_base_camera
```

右腕只充当标定时的“移动靶架”。求出的相机—基座关系与以后用左手还是右手抓取无关。

ArUco 求姿态只用 RGB；螺母三维定位仍使用对齐到彩色视角的深度。彩色/深度内参和二者
内部外参全部继续使用 Gemini 2 固件和 Orbbec 驱动提供的数据，本流程不重新估计或覆盖。

## 2. 先处理标定板

照片中的板是 3×3 GridBoard，标签显示 `DICT_6X6_250`，图案看起来是 ID 0–8。配置暂按
黑色方块边长 40 mm、相邻黑块间白色间距 10 mm 填写。开始采集前必须现场确认：

1. 用钢尺或卡尺测量 3–5 个黑色方块的外边长并取平均，只量黑框，不含白边。
2. 测量相邻两个黑框之间的白色净间距。
3. 确认九个 ID 为 0–8，且是 OpenCV GridBoard 的逐行布局。采集窗口应能标出这些 ID。
4. 把实测米制值写入
   `config/eye_to_hand_aruco.yaml` 的 `marker_length_m` 和
   `marker_separation_m`。

板的尺寸误差会近似等比例变成平移尺度误差。例如实际边长 39 mm 却填写 40 mm，会直接
引入约 2.6% 的距离误差。

照片中板面有覆膜反光，而且纸张可能弯曲。应把整块板贴在平整的硬质背板（亚克力、铝板
或硬泡沫板）上，再用至少三点固定到右腕；采集全程不得滑动、翘曲或重新粘贴。不要只让
一角悬空，也不要用手扶板。顶灯在覆膜上形成强反光时，调整灯光或使用哑光保护面，不能
让反光盖住 marker。安装后先手动检查右臂全行程，避免 A4 板碰撞立柱、桌面、相机线和
左臂。

如果需要核对现有板，可生成一张同配置参考图（不要求重新打印）：

```bash
cd aruco_eye_to_hand_calibration
./calibrate.sh generate-board
```

若重新打印，必须选择“实际大小/100%”，禁止“适合页面”，打印后仍要重新测量。

## 3. 软件和坐标系预检

本工具不保存相机、机器人或 ROS 工作区的路径。若当前终端尚未加载它们，可复制本地环境
模板并填写部署机器自己的 setup 文件；该本地文件不会进入版本控制：

```bash
cd aruco_eye_to_hand_calibration
cp config/environment.example config/environment.local
```

若 ROS 环境已在终端 source，则不需要创建 `environment.local`。依赖可按本目录的
`package.xml` 检查和安装：

```bash
rosdep install --from-paths . --ignore-src -r -y
./calibrate.sh doctor
```

预期看到固定相机、右臂、`DICT_6X6_250`、3×3、ID 0–8，以及 Python/ROS 依赖均为
`OK`。相机和机器人硬件驱动仍由各自 SDK 启动；标定目录只通过 ROS 话题与它们连接。

然后使用三个终端。先在终端 A 加载实际相机 overlay 并启动相机：

```bash
source config/environment.local
source "$ARUCO_CALIBRATION_ROS_SETUP"
source "$ARUCO_CALIBRATION_CAMERA_SETUP"
ros2 launch orbbec_camera gemini2.launch.py
```

终端 B 加载实际机器人 overlay，启动只发布状态并提供服务的驱动：

```bash
source config/environment.local
source "$ARUCO_CALIBRATION_ROS_SETUP"
source "$ARUCO_CALIBRATION_ROBOT_SETUP"
ros2 launch lbot_driver lbot_start_driver.launch.py
```

终端 C，先检查一次消息：

```bash
ros2 topic echo /camera/color/camera_info --once
ros2 topic echo /robot1/right_arm/pose_states --once
```

必须确认：

- 彩色图是 `/camera/color/image_raw`，分辨率与 `CameraInfo` 一致；
- `CameraInfo.header.frame_id` 是 `camera_color_optical_frame`；
- 右臂位姿的 `header.frame_id` 是 `base_torso_root`；
- 四元数不是全零，位置单位是米；
- 相机支架、焦距/profile、右腕工具/工作坐标设置在整个标定期间不变化。

若右臂消息的父坐标系不是 `base_torso_root`，不要简单改字符串绕过检查。先在机器人控制器
中切回真实基坐标/默认工作坐标，或者建立经过测量的坐标变换；否则结果不是螺母检测节点
所需的基座外参。

## 4. 采集 20–30 个求解姿态

先保证工作区无人、急停可触及，并使用 Web 控制、示教器或已验证的低速遥操作移动右臂。
标定程序本身不会使能机械臂，也不会发送任何动作命令。

终端 C 运行：

```bash
cd aruco_eye_to_hand_calibration
./calibrate.sh collect --set calibration --board-measured
```

窗口中绿色框和坐标轴表示已正确识别。每到一个姿态：

1. 停止右臂并等待至少 1 秒；
2. 确认至少识别 5 个 marker，最好 9 个全见；
3. 确认重投影误差低于 1.5 px，画面不过曝、不模糊；
4. 确认状态显示 `stable=YES`；
5. 按空格只保存一帧，然后再移动到明显不同的姿态；
6. 完成 20–30 帧后按 `Q` 或 `Esc` 退出。

建议按下列覆盖方式采集，而不是在一个姿态连续按空格：

- 位置：画面中心、左、右、前、后以及四个角附近，重点覆盖螺母和蓝框所在区域；
- 高度：至少三档，板到相机距离变化约 10–20 cm；
- 姿态：面向相机的基础上，绕两个不同轴分别倾斜约 `+20°/-20°`，再加入几组组合倾斜和
  `+30°/-30°` 平面旋转；
- 每一帧仍需保证足够 marker 完整可见，且板不靠近图像边缘到角点被截断。

只改变 XYZ 而让板始终严格平行于图像，会造成旋转退化；只在原地转板又会造成平移跨度
不足。程序要求有效数据至少 15 帧、位置跨度至少 120 mm、姿态跨度至少 35°。

原始图和记录保存在：

```text
artifacts/right_wrist_calibration.json
artifacts/right_wrist_calibration_images/
```

要重做时，先把旧 JSON 和对应图片目录改名归档，再开始新一轮；不要把不同相机安装或不同
板尺寸的数据追加在一起。

## 5. 求解候选外参

```bash
./calibrate.sh solve
```

工具使用 OpenCV `PARK` 手眼算法，并针对 eye-to-hand 约定正确反转机器人位姿。它会剔除
超过 15 mm 或 2° 的明显离群帧，输出：

```text
artifacts/gemini2_to_base.yaml
```

重点检查输出摘要：

- `translation_span_m`、`rotation_span_deg`：采集运动是否充分；
- `training_translation_rms_m`：各姿态推回“右腕到板”固定关系后的平移一致性；
- `training_rotation_rms_deg`：同一关系的旋转一致性；
- `rejected_sample_indices`：被拒绝的帧，可对照保存的原图检查反光、遮挡或运动模糊；
- `quality.training_passed`：训练质量门是否通过。

训练通过仍不会允许发布，因为用同一批数据自检可能掩盖系统误差。

## 6. 独立验证

保持相机和腕板完全不动，重新选择 6–10 个**没有出现在训练集中的**位置与倾角：

```bash
./calibrate.sh collect --set validation --board-measured
./calibrate.sh verify
```

默认要求验证平移 RMS 不超过 10 mm、旋转 RMS 不超过 2°，且至少 6 帧。通过后结果中应为：

```yaml
quality:
  training_passed: true
  validation_passed: true
  accepted: true
```

如果失败，优先按顺序检查：板尺寸填写错误；板/相机有松动；覆膜反光；采集时右臂未停稳；
机器人当前工作坐标或工具设置中途变化；所有姿态过于相似。不要通过放宽阈值把明显有问题的
结果强行用于抓取。

## 7. 发布 TF 并接入螺母识别

先查看将要发布的完整命令：

```bash
./calibrate.sh publish-tf --dry-run
```

确认父子坐标系正确后，在独立终端持续运行：

```bash
./calibrate.sh publish-tf
```

再在比赛业务工作区中启动螺母识别：

```bash
./scripts/run_detector.sh
```

检查 TF 和检测输出：

```bash
ros2 run tf2_ros tf2_echo base_torso_root camera_color_optical_frame
ros2 topic echo /nut_detections --once
ros2 topic echo /nut_slots --once
```

`/nut_detections` 和 `/nut_slots` 的 `header.frame_id` 必须是 `base_torso_root`。静态发布终端
关闭后 TF 会消失。还应通过 `ros2 run tf2_tools view_frames` 确认系统中只有一个节点发布
这条 base—camera 关系，避免两份外参互相争用。正式比赛启动编排后续应把已验收的 YAML
接入统一静态 TF 启动入口。

## 8. 上真机抓取前的最后验收

手眼一致性通过不等同于抓取已经安全。至少完成以下独立检查：

1. 在桌面中心和四角各放一个清晰目标，记录视觉给出的基座坐标。
2. 用机器人示教的 TCP/尖端在不接触桌面的安全高度对准这些点，比较 XY 偏差；建议目标
   小于 5 mm，超过 10 mm 必须排查。
3. 检查深度给出的桌面 Z 在整个工作区是否近似为同一平面；若呈整体倾斜，优先怀疑外参
   旋转，若局部跳变则检查深度/反光。
4. 第一次动作只到目标上方 80–100 mm 的预抓点，低速、单点、人工确认，暂不下降和闭手。
5. 只有多点预抓验证通过，才按安全规范逐步降低高度并测试抓取。

相机、支架、腕板、机器人基坐标定义发生变化后必须重标。仅在标定完成后拆掉腕板不会改变
`T_B_C`；但若拆板时碰到相机或支架，结果立即失效。
