# 任务一离线验证报告（2026-09-07）


## 结论

**离线构建、接口契约和样本上的重构一致性已验证；识别准确性尚未合格，ROS/真机未验证。**
不能把这份报告解读为已经能够连续抓取或比赛验收通过。

## 环境与基线

- 本次在 Windows 本地执行；GNU C++ 16.1.0（MSYS2 UCRT64）、CMake 4.4.3、Ninja、
  Python 3.12、PyYAML 6.0.3、从官方 4.12.0 源码构建的 OpenCV 静态库。
- 编译并发为 2；依赖只在被忽略的 `artifacts/logs/dev/` 中构建，没有安装到外部 SDK。
- 旧版对比基线：`2c7164a0dc0dbb2629e701df3eabe05fb9c8aee8`。
- 检测参数使用仓库原 `config/vision/nut_detector.yaml`，未为样本调整阈值。
- 未进行人工图像标注/视觉验收；本次结论基于 C++ 数值结果、合成场景已知生成数量和文件校验。

## 通过的验证

| 项目 | 结果 | 说明 |
|---|---|---|
| 独立 CMake 构建 | 通过 | 共享检测库、离线适配、可执行程序和两个 C++ 测试均成功编译链接 |
| 离线 Python 测试 | 13/13 通过，无跳过 | 包含真实二进制、损坏图像、中文输入路径、原始字节保存、无伪造三维位置 |
| 二维核心契约测试 | 通过 | 合成三螺母、无蓝筐、诊断输出、无输入改写、非法配置与图像拒绝、确定性 |
| 原相机几何测试 | 通过 | 不改变已有 CameraGeometry 行为 |
| 旧/新算法样本对比 | 8/8 一致 | 比较框/筐轮廓、格子像素估计和选中圆的数值；不是识别正确率 |
| 完整离线回放 | 8 帧完成，处理错误 0 | 生成原始输入副本、标注图、掩膜、候选 JSON、配置与版本摘要 |
| git diff --check | 通过 | 仅说明已跟踪改动没有该命令检测到的空白问题 |

两个 C++ 测试属于接口/行为契约测试，不是比赛准确性验收。
最初将“空框输出必须为零”混入重构测试导致失败；经旧版对比证实这是原算法已有缺陷。
现已将该项明确记录为下表的**准确性未通过**，契约测试也会打印 `KNOWN ACCURACY FAILURE`，
没有通过放宽阈值或标记预期失败把算法问题冒充修复。

## 准确性检查：已发现的旧问题

| 案例 | 已知实际数量 | 旧版选择数量 | 新版选择数量 | 来源/判断 |
|---|---:|---:|---:|---|
| 合成空框 | 0 | 3 | 3 | 全部来自 Hough，准确性失败 |
| 合成一颗 | 1 | 3 | 3 | 1 个 blackhat + 2 个 Hough，准确性失败 |
| 合成两颗 | 2 | 3 | 3 | 2 个 blackhat + 1 个 Hough，准确性失败 |
| 合成三颗 | 3 | 3 | 3 | 均来自 blackhat，本案例数量符合预期 |
| 合成四颗 | 4 | 3 | 3 | 保留旧版三目标上限，额外候选见拒绝原因 |
| 合成三颗但无蓝筐 | 3 | 3 | 3 | 共享核心仍提供二维结果；ROS 三维发布仍受蓝筐条件约束 |
| a556c88658b3d98d422bbe746be799ca.jpg | 未人工标注 | 1 | 1 | 来自 Hough，不能声称检测正确 |
| c0f93663e95849bd3293abf8d98d89c5.jpg | 未人工标注 | 3 | 3 | 2 个 blackhat + 1 个 adaptive，不能仅凭数量声称检测正确 |

这说明任务二不仅要去掉“恰好三颗”的发布门槛，还需要处理 Hough 补目标误检和三目标截断。
本次遵循任务一范围，**没有修改这两项选择策略，也没有增加身份关联或抓取状态**。
8 个样本一致只能说明这些样本未观察到重构数值回归，不能证明对所有图像等价。

## 本次验证中修复的问题

- Windows 中文仓库路径被展开到对象文件路径，MinGW assembler 无法创建对象文件。
  独立 CMake 在 Windows 使用对象路径长度限制，以生成哈希目录。
- MinGW/Ninja 链接命令经 cmd.exe 处理后中文 OpenCV 库路径乱码。
  独立 CMake 对该工具链启用响应文件，避免命令行重新编码。
- 修正空框测试的职责划分，并显式保留准确性失败报告；未悄悄改变识别阈值或算法。

## 不能通过本机完成的验证

仓库要求的原入口 `scripts/test.sh` 与 `scripts/build.sh` 已尝试，均因缺少
`/opt/ros/jazzy/setup.bash` 停止，**完整 ROS 构建没有通过**。

直接运行原有 Python 配置/入口测试时，另有两项本机环境差异：

1. 原测试用 POSIX 斜杠检查可执行路径，在 Windows 路径上断言失败；该测试和 runtime 未改动。
2. 查看器测试缺少 `ament_index_python` / `rqt_image_view`；不能以 mock 或跳过冒充真实 ROS 可用。

ROS 节点链接、实际 CameraInfo、深度编码/配准、TF、实时延迟、持续运行、机械臂均未验证。
原来的重复/过期图像、深度不同步、TF 混帧、历史 PoseArray 等风险仍未解决。
设备、固件、实际相机 profile、测试时长：本次无硬件测试，不填写虚构值。

## 复现入口和证据位置

在配置好编译器、OpenCV 开发库与 PyYAML 的环境运行：

```bash
bash scripts/build_offline.sh
bash scripts/test_offline.sh
```

Windows 可用同名 `.ps1`，或 `python apps/offline_detection.py build/test`。
不要把上面的 `build/test` 当作一个参数，实际应分别执行 `build` 和 `test`。

本地证据（被 Git 忽略，适合私下给队友，不应随源码提交）：

- `artifacts/logs/dev/detector-build.log`
- `artifacts/logs/dev/offline-tests.log`
- `build/offline_detection/Testing/Temporary/LastTest.log`（含空框准确性警告）
- `artifacts/logs/dev/parity-results.log`
- `artifacts/logs/dev/parity/`（旧版提取对比器和合成图片）
- `artifacts/datasets/validation_20260907/sequence.json`
- `artifacts/offline_detection/validation_20260907/`（8 帧全部诊断输出）

后续应在现场 Ubuntu/ROS 环境补完仓库要求的自检和完整构建，再做受控实时感知验证。
