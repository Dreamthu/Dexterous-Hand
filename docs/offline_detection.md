# 二维视觉解耦与离线回放（任务一）


## 1. 本次范围与安全边界

本次只实现**任务一的最小实用解耦**，不发送机械臂命令，不修改外部 SDK，不引入仿真。
离线程序和 ROS 节点链接同一个 `lbot_vision_detector` C++ 库，识别算法没有复制到 Python。

明确没有完成的内容：

- ROS 节点中的阶段状态/service 回放；静态工具只输出每张图片的 0..3 个二维观测。
- 相机移动补偿、抓取姿态、真实隔板识别。
- 原始深度离线定位、RGB/depth 时间同步、TF 混合坐标问题的修复。
- 自动现场录制器、ROS bag 解码器、视频解码器。本入口接收图片或有序图片清单。

因此，“按序回放”仍是**逐张独立检测**，不运行阶段状态机；离线输出不含有效抓取坐标或完成判断。
ROS 侧固定任务顺序模块见 [螺母顺序身份与状态](nut_sequence.md)。
当前支持 190×190 mm 黑框透视校正尺寸；蓝筐三格必须在机器人坐标系中计算，离线不估计格子。
详细算法与限制见 [尺寸校正与机器人 X 轴分格](perspective_sizing_and_robot_x_slots.md)。
不得据此版本直接接入自动抓取。

## 2. 结构与调用链

```text
scripts/run_detector_offline.sh 或 .ps1
  -> apps/offline_detection.py run
  -> linkerbot/offline.py（配置、文件、进程、运行记录）
  -> tools/offline_detection（C++ 文件输入/调试输出适配）
  -> lbot_vision_detector::detect_2d(BGR8, DetectorConfig)

scripts/run_detector.sh（现场原入口，不改用法）
  -> 原有 Python 适配层
  -> nut_detector_node（ROS 图像、深度、CameraInfo、TF、发布）
  -> 同一个 detect_2d
```

- `detect_2d` 不接收 ROS 消息，不订阅话题，不读写文件，不依赖深度或相机参数。
- 黑框与蓝筐的可见性分别返回；缺蓝筐时仍尝试黑框内螺母识别。
- 二维结果包括选中的圆、全部被评估的螺母候选、原始候选轮廓、来源、拒绝原因及掩膜。
- 黑框/蓝筐的候选几何拒绝过程尚未逐项输出原因；本次候选诊断指螺母候选。
- ROS 先产生二维结果，再检查三维输入。因此没有深度/CameraInfo时，仍可看二维标注。
- 离线程序不计算三维。`localization_valid=false`、`position=null`，不使用零坐标伪装成功。

## 3. 配置：不新增第二份阈值

识别阈值唯一来源仍是 `config/vision/nut_detector.yaml`。
`detector_fields.inc` 列出二维参数名称和类型；新增 `blue_s_max`、`blue_v_max`
带有兼容默认值 255，使旧配置保持原有筛选范围，其余参数仍由配置提供。
ROS 显式声明这些必需参数；离线 Python 从同一 YAML 提取字段并生成 JSON 传输快照。
C++ 的 `DetectorConfig::validate()` 统一检查数值范围、奇数核大小和枚举值。

注意：直接裸运行 ROS 可执行程序、不给完整参数文件，现在会在初始化时报缺失参数。
公开的 `scripts/run_detector.sh` 本来就传入中央 YAML，使用方式不变。
过去对偶数核大小的静默修正改为报错；中央配置本身为合法奇数，不需要改阈值。

新增的 `config/vision/offline.yaml` 只配置：

| 字段 | 默认值 | 校验 |
|---|---|---|
| detector_config | config/vision/nut_detector.yaml | 非空路径，运行时检查字段 |
| build_directory | build/offline_detection | 必须位于仓库 build/ 下 |
| output_directory | artifacts/offline_detection | 必须位于仓库 artifacts/ 下 |
| build_jobs | 2 | 1..8 的整数，不允许布尔值 |

仓库相对路径始终按仓库根目录解析，不依赖当前终端目录；manifest 内 image 按 manifest
所在目录解析。CLI 的输入和输出路径可以临时覆盖，不要把个人绝对路径写入提交的配置。

## 4. 环境和构建

### Ubuntu 24.04：离线开发不需要 ROS

安装编译器、CMake、OpenCV 开发包和 PyYAML，例如由开发人员自行运行：

```bash
sudo apt install build-essential cmake libopencv-dev python3-yaml
cd linkerbot_ws
bash scripts/build_offline.sh
bash scripts/test_offline.sh
```

这里使用 `bash scripts/...`，即使新文件还没有 Git executable 位也能运行。
构建入口只构建本仓库的独立 CMake 工程，不 source `_common.sh`，不读取/更新外部 SDK。
构建启用 CTest，默认两路并发；`test_offline.sh` 先跑离线 Python 测试，再跑 C++ 测试。
独立 CMake 使用 OpenCV 4 的 core/imgproc/imgcodecs/calib3d；calib3d 用于已有相机几何测试。

### Windows

需要 Python + PyYAML、CMake、C++17 编译器，以及**与编译器 ABI 匹配**的 OpenCV 4 开发库。
仅 `pip install opencv-python` 不提供本工程使用的 C++ 开发构建环境。
例如使用 Visual Studio 编译器就配套相应 OpenCV 库；不要混用 MSVC 库与 MinGW 编译器。
让 CMake 能通过 `OpenCV_DIR` CMake 缓存或 `CMAKE_PREFIX_PATH` 环境变量找到 OpenCV。
不要修改系统执行策略来运行脚本；如果本机策略阻止 `.ps1`，直接运行下面的 Python 入口。

```powershell
python -m pip install PyYAML
.\scripts\build_offline.ps1
.\scripts\test_offline.ps1

# 等价入口（不会启动 ROS）
python apps/offline_detection.py build
python apps/offline_detection.py test
```

当前开发机器的临时依赖及构建日志在 `artifacts/logs/dev/`，被 Git 忽略，不是正式交付依赖，
也不应提交到分支。不要修改原始机器人资料或外部 SDK 来适配离线编译。

## 5. 你如何调试照片

把图片放到被忽略的 `artifacts/datasets/`，或者直接指定仓库外绝对路径。

```bash
# 单张图片
bash scripts/run_detector_offline.sh artifacts/datasets/scene_001.png

# 一个目录：只读取该目录的图片，不递归；按文件名词典序
bash scripts/run_detector_offline.sh artifacts/datasets/round_01

# 多张图片：按命令行顺序
bash scripts/run_detector_offline.sh artifacts/datasets/a.png artifacts/datasets/b.png

# 固定输出位置，必须尚不存在，禁止覆盖旧实验
bash scripts/run_detector_offline.sh artifacts/datasets/scene_001.png \
  --output artifacts/offline_detection/experiment_01

# 无编译/无硬件检查：只检查输入和配置传输类型，不生成结果目录
bash scripts/run_detector_offline.sh artifacts/datasets/scene_001.png --dry-run
```

Windows 对应入口：

```powershell
.\scripts\run_detector_offline.ps1 artifacts/datasets/scene_001.png
# 或
python apps/offline_detection.py run artifacts/datasets/scene_001.png
```

建议图片命名为 `000001.png`、`000002.png`，不要用 `1.png`、`10.png`、`2.png` 期待数值排序。
`--dry-run` 不代表完整数值配置有效：数值边界由 C++ 在 build/run 时检查。
默认自动创建 UTC 时间 + 随机后缀的实验目录，避免两次调参结果互相覆盖。

## 6. 序列与时间戳

只有原始采集时间才填写 `capture_timestamp_ns`。不能用文件修改时间或离线运行时间冒充。
普通图片入口没有采集时间时会保留 `null`。

`artifacts/datasets/round_01/sequence.json` 示例（这里的 1000000000 仅示意，不是实际采集值）：

```json
{
  "frames": [
    {"image": "000001.png", "capture_timestamp_ns": 1000000000, "frame_id": "camera_color_optical_frame"},
    {"image": "000002.png", "capture_timestamp_ns": 1100000000, "note": "hand occlusion"},
    {"image": "000003.png", "capture_timestamp_ns": null}
  ]
}
```

```bash
bash scripts/run_detector_offline.sh --manifest artifacts/datasets/round_01/sequence.json
```

清单数组顺序就是回放顺序；它不假装是实时回放，不按时间差 sleep，也不检查物体身份。
其他逐帧字段原样写入运行记录的 metadata。这个版本每帧启动一次 C++ 子进程，优先可复现，
不适合测实时吞吐率。视频请先在采集端导出有序帧；不要把视频文件直接传入本工具。

## 7. 输出与排错

```text
artifacts/offline_detection/<run>/
  run.json                  运行版本、配置/二进制摘要、顺序、采集时间、逐帧退出码
  detector_config.yaml      当时完整中央配置副本
  effective_detector.json   传给 C++ 的二维配置快照，不要手改
  tracked_changes.patch     相对于 HEAD 的已跟踪文件差异
  000000/
    input.jpg / input.png   原输入编码字节原样保存；其他未知扩展名保存为 input.bin
    result.json             几何、候选、原因、有效性；没有机器人抓取坐标
    annotated.png           被选中的目标、校正尺寸、黑框和蓝筐四边形
    rejected.png            橙色候选标注及对应 candidates 数组索引/拒绝原因
    black_mask.png          黑色 HSV 掩膜
    blue_mask.png           蓝色 HSV 掩膜
    roi_mask.png            黑框内处理区域（找不到黑框时不生成）
    adaptive_mask.png       自适应阈值结果（找不到黑框时不生成）
    blackhat_mask.png       黑帽结果（找不到黑框时不生成）
    detector.log            C++ 输出、OpenCV 版本或处理错误
  000001/ ...
```

候选来源为 `blackhat`、`adaptive`，兼容开关启用时还可能出现 `hough`；Hough 候选没有真实外轮廓，contour 为空。
`enable_hough_fallback` 默认是 `false`，因此轮廓检测到 0、1、2、3 颗时会如实输出对应数量，不再用 Hough 补足到三颗。
拒绝原因包括面积、圆度、实心度、长宽比、靠近框线、ROI 外、重复，以及
`legacy_three_target_cap`（仍只接受前三个目标）。`nuts` 是静态回放当前帧选中的二维观测，
本工具不附加阶段身份或抓取状态；ROS 节点会把这些观测交给独立 `NutSequence` 核心。
候选半径对通过轮廓筛选的目标使用旧版旋转矩形长边的一半；早期被拒候选的诊断半径为
最小包围圆半径，因此不要拿被拒目标的半径做可靠尺寸分类。

`frame_not_found` 是无法观察黑框，不等于黑框为空；`observed_2d` 只代表在检测到的黑框内
执行了二维算法，不代表通过竞赛规则或通过三维安全检查。`basket_found` 单独检查。
离线不输出格子位置。ROS 节点用深度和 TF 将蓝框转换到 base_link 后沿 X 轴等分，编号按 X 递增；不识别真实隔板。

原图不旋转、不缩放、不重新压缩；忽略照片 EXIF 自动旋转，以免像素坐标与原始相机图不一致。
无法读取的图片产生 `processing_error` 和错误日志，继续处理后续帧，整个命令最终返回 1；
配置/环境错误返回 2；Ctrl+C 返回 130，SIGTERM 返回 143，并回收子进程。缺框或缺筐是检测结果，不是程序崩溃。

运行记录有 HEAD、dirty 状态、源码逐文件 SHA256、二进制 SHA256。dirty 源码与已编译二进制
可能不同：**改 C++ 后务必重新 build**。摘要帮助核对，不自动证明二进制来自当前源码。
`tracked_changes.patch` 不包含未跟踪文件正文，不能单独当代码备份；应发送分支提交或完整变更。
输出会记录输入绝对路径、图像和本地差异，发给队友前检查隐私；不要公开上传整包数据。

## 8. 现场队友工作流

1. 保留原来的 `scripts/run_perception.sh` / `scripts/show_camera.sh` 实时入口。
2. 漏检时通过已有采集手段保存**原始彩色图**，而不是截图或标注图；也保存完整检测 YAML。
3. 记录相机安装姿态、照明、固件、实际分辨率/profile、采集时间和当时 Git 提交号。
4. 对遮挡、连续取走问题保存一段帧序列和采集时间，先不要仅发一张图片。
5. 在无 ROS 的独立回放中确认是阈值/轮廓问题还是二维已经正确。
6. 将同一批样本发给远程开发者。修复后回放旧案例，再重新构建 ROS，在实时图像中复测。

如果二维正确但三维错误，还必须额外保存：原始深度数值、深度单位/编码、RGB 与深度时间戳、
设备发布的 CameraInfo K/D/distortion_model、profile、配准状态、TF/外参来源。
**本工具当前不会加载这些三维数据**；它们是下一阶段排错的数据要求，不是另一份内参标定。
彩色深度预览图不能替代原始深度；不要手填虚构外参。

## 9. ROS 行为变化与回归风险

- 原有检测/格子 PoseArray 话题名称保持不变；螺母按校正毫米尺寸排序，格子按机器人 X 递增排列。
- 二维计算移到深度检查前；缺深度/CameraInfo 新状态为 `localization_inputs_missing`。
- 显式报告 `invalid_color_image`、`detection_2d_failed`、`invalid_depth_image`、
  `unsupported_depth_encoding`；这些状态不会生成假的三维坐标。
- 修复候选浮点中心 cvRound 后可能落到 ROI 外的边界访问。
- 配置不再静默接受错误枚举或偶数核大小。
- 额外诊断轮廓和掩膜会增加内存/计算开销，需现场测延迟和持续运行稳定性。
- ROS 节点现已拒绝重复/过期彩色帧、RGB-depth 时间差超限，并对同一观测使用同一时刻 TF；
  但这些改动仍需 ROS Jazzy 和 Gemini 2 现场验证。消费者应优先使用结构化 sequence 消息，
  不得只把无状态 PoseArray 当成抓取指令。

完整文件清单、测试结果和提交步骤见 [本次修改与提交说明](offline_detection_changes.md)。

验证结果见 [2026-09-07 验证报告](offline_detection_validation.md)。离线契约测试通过不代表识别准确性通过；报告中记录的空框 Hough 误检已通过默认关闭兼容 fallback 避免。
