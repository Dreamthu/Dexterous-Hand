# 右腕 ArUco 眼在手外标定

顶置 Gemini 2 固定在机器人上，3×3 ArUco 板固定在右腕。相机使用出厂内参，通过多姿态
观测求解 `base_link <- camera_color_optical_frame`。本工具只采集、求解和发布外参，
不发送机械臂运动命令，不包含物体识别或抓取控制器。

## 一、标定流程

### 1. 准备软件环境

保留本仓库的 `calibrate.sh`、`src/`、`scripts/`、`config/`、依赖清单和文档即可重新标定。
运行需要 ROS 2、相机驱动、机器人驱动及其 `lbot_arm_interfaces` 接口；这些由部署机器
安装，不要求 SDK 源码与本仓库处于固定相对位置，也无需对本工具执行 `colcon build`。

所有相对配置/产出路径均以本仓库根目录为基准。先进入本仓库并加载 ROS、相机及机器人
驱动 overlay。如果不在终端手动 source，可以创建本机环境配置：

```bash
cp -n config/environment.example config/environment.local
```

编辑 `environment.local`，填写本机实际安装的 setup 文件路径；`calibrate.sh` 会自动读取。
该文件被 Git 忽略，不能把开发机路径写入公共配置。ROS 机器建议使用系统 OpenCV，避免
pip OpenCV 与 `cv_bridge` ABI 冲突。

```bash
rosdep install --from-paths . --ignore-src -r -y --skip-keys lbot_arm_interfaces
./calibrate.sh doctor
```

`lbot_arm_interfaces` 由已经构建/安装的机器人驱动 overlay 提供，不由公共 rosdep 安装。
无硬件测试使用 `./scripts/test.sh`；加载 ROS/机器人接口后可用 `./scripts/test_ros.sh`
运行本机隔离测试域中的假服务/假 TF 测试，不连接真实机器人。

### 2. 准备标定板和新一轮配置

- 板字典为 `DICT_6X6_250`，3×3、ID 0–8。实测黑框外边长和相邻黑框白色净间距，按米填写；
  默认 0.040 m / 0.010 m 只有与实物一致时才可使用。
- 板必须平整、刚性固定在右腕；相机与机器人基座的安装关系必须固定。避免反光、弯曲及单角悬挂。
- 全程保持相机—基座、板—反馈末端的相对关系不变，不修改工具偏移、工作坐标或关节零位。
- 已提交的 v2 文件是成功基线，**不要继续向它们采集，也不要覆盖它们重新求解**。

先为新一轮复制配置：

```bash
cp -n config/eye_to_hand_aruco.yaml config/next_session.yaml
```

编辑 `config/next_session.yaml` 中的板尺寸，并把下面三个路径改成该轮独有的名字，例如：

```yaml
capture:
  calibration_dataset: artifacts/right_wrist_calibration_session03.json
  validation_dataset: artifacts/right_wrist_validation_session03.json
solve:
  result: artifacts/gemini2_to_base_session03.yaml
```

这里只展示需要修改的字段，不要用这段片段覆盖整个配置文件。临时轮次配置和新产出默认不进入 Git。

### 3. 启动驱动并检查坐标系

加载相应 overlay 后，在两个终端分别启动相机和机器人驱动：

```bash
# 相机终端：只启动一个相机驱动，保持 publish_tf 开启
ros2 launch orbbec_camera gemini2.launch.py
```

```bash
# 机器人终端：发布状态、提供只读查询服务
ros2 launch lbot_driver lbot_start_driver.launch.py
```

第三个终端检查：

```bash
ros2 topic echo /camera/color/camera_info --once
ros2 topic echo /robot1/right_arm/pose_states --once
ros2 service call /robot1/right_arm/get_current_tool_frame lbot_arm_interfaces/srv/GetCurrentFrame '{}'
./calibrate.sh --config config/next_session.yaml doctor
```

确认彩色帧为 `camera_color_optical_frame`，机器人位姿父坐标系为 `base_link`，位置单位是米。
当前配置要求工具查询 `success: true`、`frame.name: Arm_Tip`、位置/欧拉角均为零偏移。
`R8_Link` 是模型末端名称，`Arm_Tip` 是反馈工具，不要只改标签来绕过检查。
若现场设置不同，先核实控制器的坐标定义，再建立新的采集契约。

### 4. 采集训练数据

```bash
./calibrate.sh --config config/next_session.yaml collect --set calibration --board-measured
```

用已验证的控制方式低速移动右臂；本程序不会自动移动机器人。每到一个姿态停止并等待至少
1 秒，确认 `tool=OK`、`stable=YES`、至少 5 个 marker 可见且重投影误差低于 1.5 px，再按
空格保存一帧。按 `Q` / `Esc` 结束。

建议采集 20–30 个不同姿态，覆盖工作区域、至少三档高度，以及绕两个不同轴的倾斜和组合
旋转。不要只平移不旋转，也不要在同一姿态连续保存。注意急停、板与桌面/立柱的碰撞空间。

程序检查完整静止时间窗、时间戳/消息新鲜度、父坐标系及当前工具设置。不通过时先排查
反馈、时钟或安装问题，不要仅放宽门限。工具名称或偏移不符会锁定当前会话，需要退出核查。

### 5. 求解并独立验证

```bash
./calibrate.sh --config config/next_session.yaml solve
```

检查训练质量门和逐帧残差。默认至少保留 15 帧、平移跨度 120 mm、旋转跨度 35°；训练
RMS 门限为 8 mm / 1.5°。离群剔除受最少样本数和迭代次数限制，不能只看剔除数量。

保持安装关系不变，移动右臂，再采集 6–10 个不同于训练集的姿态：

```bash
./calibrate.sh --config config/next_session.yaml collect --set validation --board-measured
./calibrate.sh --config config/next_session.yaml verify
```

默认独立验证 RMS 门限为 10 mm / 2°，至少 6 帧。结果中必须同时满足：

```yaml
quality:
  training_passed: true
  validation_passed: true
  accepted: true
```

训练和验证都通过后才能发布。不要复制训练观测充当验证，也不要手动修改通过标志。
完整姿态建议、故障排查和现场验收见 [详细标定流程](docs/extrinsic_calibration.md)。

## 二、外参产出后续怎么使用

### 1. 本次已提交的成功产出

| 文件 | 用途 |
| --- | --- |
| [artifacts/gemini2_to_base_v2.yaml](artifacts/gemini2_to_base_v2.yaml) | 已验证的外参、坐标系、质量指标及来源记录 |
| [artifacts/calibration_config_v2.yaml](artifacts/calibration_config_v2.yaml) | 本次参数快照，现有发布工具使用它定位结果并检查配置 |

本次结果为 2026-09-10 的成功基线：20 帧训练中使用 16 帧，另有 9 帧独立验证；训练 RMS
为 **7.26 mm / 1.19°**，验证 RMS 为 **8.01 mm / 1.68°**，验证最大偏差为 **13.42 mm / 3.21°**。
门限评估的是 RMS 一致性，不代表每帧都低于门限，也不等于绝对定位精度或抓取成功率认证。

这两个 YAML **已纳入 Git**，克隆仓库后可直接用于部署；原始采集 JSON、图片、新实验配置、
缓存、日志、本机环境和其他产出均被忽略，但仍保留在本地。需要重新审计或复现原始求解时，
应另行备份/复制原始训练与验证数据及图片；结果中的源文件路径仅作来源记录，发布 TF 不读取它们。

本次结果文件 SHA-256：

```text
e9347da9c7005ec3a3e0bbce84083e0fd86ed0d68a676fdc883303621aeb9a69
```

### 2. 日常启动：发布 TF，不要重新标定

相机相对基座的安装未变时，无需贴板或重跑 `collect / solve / verify`。启动相机驱动、
加载 ROS 环境后，在本目录运行：

```bash
./calibrate.sh --config artifacts/calibration_config_v2.yaml publish-tf --dry-run
./calibrate.sh --config artifacts/calibration_config_v2.yaml publish-tf
```

`--dry-run` 只读查询相机内部 TF，因此也需要相机驱动在线。保持相机和发布进程持续运行；
如果已有正确的外参发布进程，只查询，不要重复启动。日常发布不查询机器人关节或工具服务。

```text
base_link → camera_link → … → camera_color_optical_frame
            本工具           相机驱动的内部 TF
```

YAML 保存的是 `base_link <- camera_color_optical_frame`；程序读取驱动内部变换后，计算
`T_base_camera_link = T_base_color_optical × inverse(T_camera_link_color_optical)` 再发布。
不能只改 child 名称而保留原数值，也不能额外把光学坐标系直接挂到基座上，与驱动争用父节点。

在另一已加载 ROS 的终端核对：

```bash
ros2 run tf2_ros tf2_echo base_link camera_color_optical_frame
```

查询值应与结果 YAML 一致。没有 TF 时检查相机/发布进程、ROS_DOMAIN_ID、网络发现及环境；
“文件 accepted”不代表发布者当前在线。停掉静态发布者后，旧 TF Buffer 可能仍缓存旧值；
更换外参时统一管理发布者并重启/刷新相关消费者。

### 3. 从识别像素得到机器人坐标

```text
RGB 识别像素 + 对齐到 RGB 的深度 + 当前 CameraInfo(K、D、畸变模型)
  → camera_color_optical_frame 中的三维点（米）
  → TF 或外参矩阵转换一次
  → base_link 中的物体位置
  → 另行计算 TCP 目标、规划和执行抓取
```

像素不能直接乘 4×4 外参。先用当前相机内参和畸变模型去畸变/反投影，且深度必须与彩色
视角正确注册；仅按宽高比例缩放原始深度像素不等于完成注册。检查深度单位、有效性和时间同步。

本次彩色 profile 为 **1280×720**。切换分辨率不必然改变刚体外参，但必须读取新 profile
的 CameraInfo，确认同一相机/光学坐标系及深度注册，并在实际运行 profile 下复验多点定位。
结果中的内参只是采集快照，不能覆盖相机驱动当前的出厂参数。

**ROS 节点优先查询 TF。** 以下函数接收已经反投影的三维点；调用方需创建并持续 spin
`tf2_ros.Buffer`/`TransformListener`，检查点坐标有限、时间戳非零且未过期：

```python
from geometry_msgs.msg import PointStamped
from rclpy.duration import Duration
from rclpy.time import Time
from tf2_geometry_msgs import do_transform_point

def camera_point_to_base(tf_buffer, point: PointStamped) -> PointStamped:
    if point.header.frame_id != "camera_color_optical_frame":
        raise ValueError("需要彩色光学坐标系下、单位为米的三维点")
    transform = tf_buffer.lookup_transform(
        "base_link", point.header.frame_id,
        Time.from_msg(point.header.stamp), timeout=Duration(seconds=0.2),
    )
    output = do_transform_point(point, transform)
    output.header.stamp = point.header.stamp  # 保留观测时间
    return output
```

查询异常时停止本次目标处理，不回退为“原坐标就是基座坐标”。同步等待时必须保证 TF
监听回调仍可执行，不能阻塞唯一执行器线程后又依赖它接收 TF。已经位于 `base_link` 的
检测点无需再转换。

**非 ROS 程序可以直接读取外参矩阵。** 结果路径由调用方传入，不硬编码开发机路径：

```python
from pathlib import Path
import numpy as np
import yaml

def transform_camera_point(result_file: Path, point_camera_m):
    with result_file.open(encoding="utf-8") as stream:
        result = yaml.safe_load(stream)
    if result.get("quality", {}).get("accepted") is not True:
        raise ValueError("外参未通过验证")
    if (result["parent_frame"], result["child_frame"]) != (
        "base_link", "camera_color_optical_frame"
    ):
        raise ValueError("外参方向不符合接口契约")
    matrix = np.asarray(result["matrix_parent_from_child"], dtype=float)
    point = np.asarray(point_camera_m, dtype=float)
    if matrix.shape != (4, 4) or point.shape != (3,):
        raise ValueError("需要 4×4 外参及三维点")
    if not np.isfinite(matrix).all() or not np.isfinite(point).all() or point[2] <= 0:
        raise ValueError("非法外参或相机点")
    return matrix[:3, :3] @ point + matrix[:3, 3]
```

公式为 `p_base = R × p_camera + t`，位置单位是米。输入原始深度光学坐标时还需先变换到
彩色光学坐标系；输入已经是 `base_link` 时不能重复转换。

### 4. 抓取接入与重标条件

- 视觉输出必须携带 `base_link` 和观测时间；抓取端拒绝错误 frame、过期目标和失败状态，不能继续使用缓存目标。
- 外参不提供抓取方向、手型、TCP/指尖偏移、逆运动学或碰撞规划，这些需另外实现。
- `estimated_gripper_from_board` 是腕部到标定板的安装变换，**不是抓取 TCP**。不能把物体中心直接当成手腕目标。
- 反馈末端为 `G`、目标抓取 TCP 为 `P` 时，使用 `T_base_G = T_base_P × inverse(T_G_P)`；
  `T_G_P` 必须经过确认，左右臂控制接口也必须使用一致的共同基座定义。
- 相机—基座关系变化、支架松动、换相机或基座定义变化后需重标/重建可靠变换。
- 标定后拆掉腕板、移动手臂或换手抓取不会自动改变相机外参，但不能碰松相机。改变工具/TCP 后须更新 TCP 映射并复验。
- 在训练和验证期间禁止改变工具或腕板安装。上真机前先做桌面多点比对，再人工确认、低速到安全预抓点，最后才逐步接近并闭手。
