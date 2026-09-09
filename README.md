# 右腕 ArUco 外参标定工具

这是一个与比赛业务仓库隔离的固定相机 eye-to-hand 标定目录。它使用顶置 Gemini 2 的
彩色图和出厂 `CameraInfo`，同步读取右臂末端位姿，通过右腕 3×3 ArUco GridBoard 求解：

```text
base_torso_root <- camera_color_optical_frame
```

工具不导入 `linkerbot_ws`、Orbbec SDK 或机器人 SDK 的源码，不要求这些工作区与本目录处于
固定相对位置。运行时只订阅标准 ROS 2 消息，并在发布结果时调用系统中的
`static_transform_publisher`。相机和机器人硬件驱动仍需由各自 SDK 启动，这是无法打包进
标定算法目录的运行时边界。

## 目录

```text
aruco_eye_to_hand_calibration/
├── calibrate.sh                 # 唯一用户入口，自定位项目根目录
├── config/
│   ├── eye_to_hand_aruco.yaml   # 板、话题、坐标系和质量门
│   └── environment.example      # 可选 ROS overlay 路径模板
├── src/extrinsic_calibration/   # ArUco、手眼求解、采集和 TF 发布代码
├── scripts/                     # Python 入口与测试入口
├── tests/                       # 不依赖硬件的合成真值测试
├── docs/                        # 完整现场流程
├── artifacts/                   # 采集图、数据、结果（不进版本控制）
├── package.xml                  # rosdep/ROS 依赖清单
└── requirements.txt             # 非 ROS Python 环境依赖清单
```

所有数据路径默认相对本目录解析。源码和提交配置中没有开发机用户名或工作区绝对路径。

## 快速开始

如果当前终端已经 source 了 ROS、相机 overlay 和机器人 overlay，可以直接执行：

```bash
./calibrate.sh doctor
```

否则复制环境模板并填写本机安装位置。`environment.local` 已被忽略，不会把机器路径提交到
源码：

```bash
cp config/environment.example config/environment.local
```

ROS 机器推荐使用系统 OpenCV，避免 pip OpenCV 与 `cv_bridge` ABI 冲突。可通过 rosdep
按照 `package.xml` 安装声明的依赖：

```bash
rosdep install --from-paths . --ignore-src -r -y
```

完整执行顺序：

```bash
./calibrate.sh doctor
./calibrate.sh collect --set calibration --board-measured
./calibrate.sh solve
./calibrate.sh collect --set validation --board-measured
./calibrate.sh verify
./calibrate.sh publish-tf --dry-run
./calibrate.sh publish-tf
```

开始采集前，必须实测黑色 marker 边长和白色间距并更新配置。详细的姿态覆盖、质量指标、
故障排查和抓取前验收见 [完整标定流程](docs/extrinsic_calibration.md)。

运行无硬件测试：

```bash
./scripts/test.sh
```
