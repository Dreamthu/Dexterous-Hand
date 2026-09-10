# 顶置 Gemini 2 + 右腕 ArUco 外参标定

> 当前 v2 标定已通过并完成复核。日常使用请先阅读 [README 的后续开发说明](../README.md)，
> 不需要重复本页的采集/求解步骤。重新标定前必须使用新的训练、验证和结果路径，保留成功轮次。

## 1. 本装置采用的方案

从现场照片和仓库资料可确定：Gemini 2 固定在机器人顶部向下观察桌面，ArUco 板固定在
右手手腕，螺母最终要在机器人基坐标中定位。因此这是 **eye-to-hand（眼在手外）**，不是
eye-in-hand。

使用的坐标系和观测量如下：

| 符号 | 程序中的含义 | 来源 |
| --- | --- | --- |
| `B` | `base_link` | 控制器/网页模型的双臂共同基座，原点在两肩之间附近 |
| `G` | 当前工具 `Arm_Tip` | 右臂 `pose_states`；工具名称和偏移通过只读服务检查 |
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

网页 [模型配置](http://192.168.10.21:8000/api_http/robot_model.json) 和
[URDF](http://192.168.10.21:8000/urdf/lkls73_i1_dual_arm_description.urdf) 中，左右第一关节
相对 `base_link` 的位置约为 `[0,+0.164,0]` 与 `[0,-0.164,0]` 米。它不是右肩单独的原点，
也不是地面坐标。网页 `base_position: [0,0,1.2]` 是可视化摆放，不作为标定的实测变换。
上述网址只是核查来源，程序不依赖网页地址；机器人/ROS 地址均由部署环境决定。

网页模型末端为 `R8_Link`，控制器工具为 `Arm_Tip`。核查时工具位置、欧拉角均为零偏移，
但不能据此推断历史采集期间的设置。`PoseStamped` 没有 `child_frame_id`，因此
`robot_pose_child_frame: Arm_Tip` 是反馈末端的语义记录，不是在 TF 中创建该坐标系。
`robot.model_end_link: R8_Link` 单独记录模型名称；不能把旧 `arm_right_R8_Link` 标签直接
当作已验证的 TF 别名，也不要用另一版 URDF 的 FK 替换当前反馈而不核对模型。

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
rosdep install --from-paths . --ignore-src -r -y --skip-keys lbot_arm_interfaces
./calibrate.sh doctor
```

`lbot_arm_interfaces` 需由已构建/安装的厂商机器人驱动 overlay 提供。只需 source 它的
setup 文件，不需把 SDK 源码复制进标定目录。若 `doctor` 报服务类型缺失，先检查 overlay。

预期看到固定相机、右臂、`DICT_6X6_250`、3×3、ID 0–8，以及 Python/ROS 依赖均为
`OK`。相机和机器人硬件驱动仍由各自 SDK 启动；标定目录通过 ROS 话题、只读工具查询服务
和 TF 与它们连接，不连接网页控制接口，也不发送运动、使能或设置坐标系命令。

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
ros2 service type /robot1/right_arm/get_current_tool_frame
ros2 service call /robot1/right_arm/get_current_tool_frame lbot_arm_interfaces/srv/GetCurrentFrame '{}'
```

必须确认：

- 彩色图是 `/camera/color/image_raw`，分辨率与 `CameraInfo` 一致；
- `CameraInfo.header.frame_id` 是 `camera_color_optical_frame`；
- 右臂位姿的 `header.frame_id` 是 `base_link`；
- 工具查询 `success: true`，`frame.name: Arm_Tip`，`frame.position` 与 `frame.euler` 均为零；
- 四元数不是全零，位置单位是米；
- 相机支架、焦距/profile、右腕工具/工作坐标设置在整个标定期间不变化。

当前这台机器的 `base_link` 已与网页模型核对，应保留，不需要恢复成 `base_torso_root`。
若现场消息与配置仍不一致，应核实控制器工作坐标和模型定义，不能只改消息字符串。
标定工具沿用 `/robot1/right_arm/pose_states`；网页的 `/right/arm_state` 是另一种消息，
不能只改话题名就替换。

工具检查参数位于 `robot`：默认每 0.5 秒只读查询一次，观测最长有效期 1 秒，服务请求
超时 2 秒。缺少服务、查询失败、超时、工具名称或偏移不符都会阻止保存；名称/偏移不符
会锁定本次采集，必须退出、核查后重新启动。默认仅接受零偏移 `Arm_Tip`；若确实使用
非零工具，先确认反馈语义，将实测/控制器确认的值填入 `expected_tool_*`，开启新数据集。
程序不会帮你设置或切换工具。轮询无法证明两次查询之间没有短暂修改，因此仍禁止采集中
切换工具、工作坐标或关节零位。

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
4. 确认状态显示 `stable=YES`、`tool=OK`；
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
artifacts/right_wrist_calibration_v2.json
artifacts/right_wrist_calibration_v2_images/
```

要重做时，先把旧 JSON 和对应图片目录改名归档，再开始新一轮；不要把不同相机安装或不同
板尺寸的数据追加在一起。

当前 `_v2` 文件已是成功基线，历史轮次已移到目录外的恢复归档。不要追加到成功数据集；
以后再次改变安装关系时，在新的配置中选择新的训练、验证及结果路径，成功轮次保留原样。

静止检查现在要求历史覆盖图像前完整 0.4 秒窗口，允许端点采样误差但消息间隙不得超过
0.1 秒；重复/倒退时间戳、窗口中父坐标系变化、消息过期都会阻止采集。
`maximum_pose_age_s` 默认 0.3 秒、图像最长 1 秒，图像与位姿仍需同步到 0.12 秒以内。
若持续提示时间异常，先检查机器时钟、相机时间基准及驱动，不要只增大阈值。
某些驱动会给重复的底层状态重新打发布时间戳；程序无法仅从 `PoseStamped` 证明底层状态
新鲜，因此硬件反馈是否持续更新仍需现场确认。`stable=YES` 也检测不到板相对腕部滑动。

## 5. 求解候选外参

```bash
./calibrate.sh solve
```

工具使用 OpenCV `PARK` 手眼算法，并针对 eye-to-hand 约定正确反转机器人位姿。它会尝试剔除
超过 15 mm 或 2° 的明显离群帧，但到达最少 15 帧或迭代上限后不会继续删除，仍需检查最终
质量门和逐帧残差，输出：

```text
artifacts/gemini2_to_base_v2.yaml
```

重点检查输出摘要：

- `translation_span_m`、`rotation_span_deg`：采集运动是否充分；
- `training_translation_rms_m`：各姿态推回“右腕到板”固定关系后的平移一致性；
- `training_rotation_rms_deg`：同一关系的旋转一致性；
- `rejected_sample_indices`：被拒绝的帧，可对照保存的原图检查反光、遮挡或运动模糊；
- `sample_residuals`：每帧相对最终估计腕板安装关系的平移/旋转残差，包含被剔除帧和图片路径；
- `quality.training_passed`：训练质量门是否通过。

训练通过仍不会允许发布，因为用同一批数据自检可能掩盖系统误差。

## 6. 独立验证

保持相机—基座、板—腕部安装关系不变，移动右臂，重新选择 6–10 个**没有出现在训练集中的**位置与倾角：

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

验证数据默认保存为 `artifacts/right_wrist_validation_v2.json`。程序检查工具/板/相机/坐标系
契约一致，并拒绝重复训练时间戳或直接复制的训练观测；仍需人工保证姿态真正独立。

如果失败，优先按顺序检查：板尺寸填写错误；板/相机有松动；覆膜反光；采集时右臂未停稳；
机器人当前工作坐标或工具设置中途变化；所有姿态过于相似。不要通过放宽阈值把明显有问题的
结果强行用于抓取。

## 7. 发布 TF 并接入螺母识别

先保持相机驱动运行，开启 `publish_tf`（驱动默认开启），停止已有相机外参发布进程。
然后查看将要发布的完整命令；`--dry-run` 只读 ROS TF，不发布，但不能离线凭空生成内部外参：

```bash
./calibrate.sh publish-tf --dry-run
```

确认父子坐标系正确后，在独立终端持续运行：

```bash
./calibrate.sh publish-tf
```

程序读取 `camera_link <- camera_color_optical_frame`，计算：

```text
T_base_camera_link = T_base_color_optical × inverse(T_camera_link_color_optical)

base_link → camera_link → … → camera_color_optical_frame
            标定发布       相机驱动内部 TF
```

求解 YAML 中 `parent_frame: base_link`、`child_frame: camera_color_optical_frame` **不变**；
只有实际发布命令的 child 是 `camera_link`，变换数值也随之换算。
禁止只重命名 child 而不换算数值，禁止另外发布 `base_link → camera_color_optical_frame`。
发布前会检查内部静态链和多父节点；`camera_link` 已有父节点时拒绝启动，以避免重复外参。
检查属于启动预检，不能阻止别的程序在之后发布冲突 TF，运行期间仍须保持单一发布者。

再在比赛业务工作区中启动螺母识别：

```bash
./scripts/run_detector.sh
```

检查 TF 和检测输出：

```bash
ros2 run tf2_ros tf2_echo base_link camera_color_optical_frame
ros2 topic echo /nut_detections --once
ros2 topic echo /nut_slots --once
```

业务仓库 `config/vision/nut_detector.yaml` 的 `target_frame` 现为 `base_link`，`use_tf: true`。
`/nut_detections` 和 `/nut_slots` 的 `header.frame_id` 必须是 `base_link`。TF 转换失败时，
检测节点报告 `tf_unavailable` 并停止该帧坐标输出，不再回退为相机坐标。抓取端仍必须拒绝
过期结果、错误 frame 和非正常状态；不得重复执行上一次缓存目标。
只有显式设置 `use_tf: false` 才能用于相机坐标调试，调试输出不能接抓取。

关闭静态发布终端后，新订阅者不能再获得该发布者的持久消息，但已有 TF Buffer 可能仍缓存
旧变换；停止进程不等于所有消费者立即失效。更换外参时重启相关消费者并核对结果。
还应通过 `ros2 run tf2_tools view_frames` 确认系统中只有一个节点发布
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

## 9. 当前成功轮次与历史归档

当前 `gemini2_to_base_v2.yaml` 已通过训练和独立验证，离线重算与保存结果一致。
训练为 20 帧（使用 16 帧），独立验证 9 帧；RMS 分别为 7.26 mm / 1.19° 和 8.01 mm / 1.68°。
所有 29 张图片及数据均保留在本地但被 Git 忽略；结果和
`artifacts/calibration_config_v2.yaml` 参数快照纳入 Git，克隆后即可用于发布。
重新审计原始求解需要另外复制原始 JSON 和图片，日常发布不依赖这些原始数据。

旧数据、旧候选和 `legacy_diagnostic.yaml` 已移出运行目录，不再作为快速开始或测试的依赖。
本机历史归档位于仓库外，不是运行依赖，也不随 Git 交付。
标定流程、TF/离线读取代码及抓取限制见 [README](../README.md)。
