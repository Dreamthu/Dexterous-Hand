# 运行与故障排查

本页记录已经在 Gemini 2、ROS 2 Jazzy 和 WSL 环境中复现并验证过的问题。正常使用仍然
只通过 `scripts/` 入口，不需要手工拼接 ROS 命令。

## 正确启动顺序

`run_perception.sh` 是无界面的感知服务，不会自动弹出图像窗口。

在第一个终端启动并保持运行：

```bash
./scripts/run_perception.sh
```

相机正常时，终端会同时出现类似信息：

```text
color Frame - Width: 640 Height: 360 fps: 15 Format: MJPG
depth Frame - Width: 640 Height: 360 fps: 15 Format: Y16
```

然后在第二个终端查看原始彩色图：

```bash
./scripts/show_camera.sh
```

查看识别节点生成的标注图：

```bash
./scripts/show_camera.sh --topic /nut_detection/debug_image
```

同一时刻只运行一个相机启动入口。不要同时运行 `run_camera.sh` 和
`run_perception.sh`，否则后启动的进程无法独占 USB 相机。

## 当前验证过的相机 profile

权威配置位于 `config/camera/gemini2.yaml`：

- 彩色输入：`640×360 @ 15 FPS`，`MJPG`；ROS 输出编码为 `rgb8`。
- 深度输入：`640×400 @ 15 FPS`，格式由固件自动选择；ROS 输出编码为 `16UC1`。
- 开启硬件深度对齐后，当前设备的深度输出为 `640×360`，与彩色画面一致。
- WSL USBIP 下关闭 `enable_frame_sync`，避免深度流偶发停留在 `STARTING`。
- IR、IMU 和点云默认关闭，降低 USB 带宽与 CPU 占用。

不要仅因为配置中写着 `depth_format: ANY` 就把深度消息当作未知格式。`ANY` 表示让设备
选择可用的原生 profile，驱动发布到 ROS 后仍是明确的 `16UC1`。

## 厂内参与畸变

仓库不加载自定义 `camera_info.yaml`。驱动从设备读取当前 profile 对应的厂内参，并在
`/camera/color/camera_info` 发布 `K`、`D` 和畸变模型。识别节点对原始图像里的检测像素
应用 `D` 去畸变后再做三维反投影；如果 `CameraInfo` 无效，或它的尺寸与彩色图不一致，
节点会拒绝输出错误坐标并在终端报告原因。

修改彩色分辨率后必须同时确认驱动仍在发布同尺寸的 `CameraInfo`。不要通过
`color_info_url` 或仓库内标定文件覆盖设备数据。相机到机器人基座的 TF 属于安装外参，
它缺失时节点只会发布带有相机 frame 的坐标，不会伪装成机器人基座坐标。

## 灰色窗口

灰色背景通常表示所选 topic 当前没有图像消息，而不是 rqt 把彩色图转换成了灰度图。

按以下顺序判断：

1. 先查看默认的 `/camera/color/image_raw`。
2. 如果原始彩色图正常、`/nut_detection/debug_image` 灰色，检查
   `run_perception.sh` 终端中是否启动了 `nut_detector_node`，以及是否同时出现了彩色和
   深度的 `Frame` 日志。
3. 如果只有 `color Frame`、没有 `depth Frame`，按 `Ctrl+C` 完整停止感知程序，等待
   相机释放后重新运行 `run_perception.sh`。
4. 如果原始彩色图也没有画面，检查 USB 设备是否已连接到当前 WSL 实例，并确认没有
   第二个相机进程占用设备。
5. 驱动和启动日志位于 `artifacts/logs/orbbec/`，提交问题时附上最新一次启动目录中的
   `launch.log`。

识别状态 `frame_not_found` 表示算法没有在当前画面中找到目标黑框。此时调试 topic 仍应
显示原始画面，只是不会出现检测轮廓；它与灰色空窗口不是同一个问题。

## `class_loader` 卸载警告

关闭 rqt 窗口时可能出现：

```text
class_loader.ClassLoader: SEVERE WARNING!!! Attempting to unload library while
objects created by this loader exist in the heap
```

这是 rqt/pluginlib 退出阶段的插件卸载警告。只要窗口此前能正常显示图像，并且警告发生在
关闭窗口时，就不代表相机或图像编码失败，无需重新安装相机驱动。

## 固件说明

实机验证时 Gemini 2 固件为 `1.4.92`，仓库所用 Orbbec 驱动文档推荐 `1.4.98`。当前
低带宽配置已经能够正常工作，不应只为消除 rqt 警告而升级固件。固件升级必须单独安排，
升级前确认设备型号、供电和恢复方式，并在升级后重新验证彩色、深度、对齐和
`CameraInfo` 发布。
