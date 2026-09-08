# 2026-09-08 螺母顺序状态改动与交接

本次在已完成的二维检测解耦和“默认禁用 Hough 补目标”基础上，增加固定任务使用的阶段驱动身份/状态模块。交接准备阶段未发送任何机械臂动作；提交操作只记录源码和文档，不会运行机器人。

## 1. 最终行为

- 初始连续 3 帧稳定识别三颗后，按像素半径绑定：`1=nut_large`、`2=nut_medium`、`3=nut_small`。
- 只有外部明确发送 `complete` 才推进阶段；检测数量变化不会推进。
- 大完成后期望 2 颗，按当前像素半径映射为 `nut_medium/nut_small`。
- 中完成后期望 1 颗，固定为 `nut_small`。
- 状态支持 `pending / in_progress / completed`，事件支持 `start / complete / retry / reset`。
- 事件带 session、round 和递增序号，最后一次成功事件可幂等重试。
- Hough 代码保留但中央配置 `enable_hough_fallback: false`；黑框最大面积比例仍为 `0.30`，`max_nut_area_px` 仍为 `100000.0`。

详细接口和现场操作见 [`docs/nut_sequence.md`](nut_sequence.md)。

## 2. 新增文件

| 文件 | 作用 |
|---|---|
| `src/lbot_vision/include/lbot_vision/sequence_fields.inc` | 顺序模块参数名称/类型表 |
| `src/lbot_vision/include/lbot_vision/nut_sequence.hpp` | ROS-free 固定 ID、阶段和事件接口 |
| `src/lbot_vision/src/core/nut_sequence.cpp` | 3→2→1 稳定排序和显式状态推进实现 |
| `src/lbot_vision/tests/nut_sequence_test.cpp` | 顺序、漏检、retry、幂等、reset 等契约测试 |
| `src/lbot_vision/msg/NutTarget.msg` | 单个固定目标的结构化状态 |
| `src/lbot_vision/msg/NutSequenceState.msg` | 会话、轮次、期望数量和三目标数组 |
| `src/lbot_vision/srv/SetNutState.srv` | 外部反馈 start/complete/retry/reset |
| `docs/nut_sequence.md` | 设计、ROS 接口、使用边界和现场验收 |
| `docs/nut_sequence_changes.md` | 本交接文件 |

## 3. 修改文件

| 文件 | 修改内容 |
|---|---|
| `config/vision/nut_detector.yaml` | 保持 Hough 默认关闭，并增加 sequence 话题、service、稳定帧与时效参数 |
| `src/lbot_vision/detector_core.cmake` | ROS 与离线构建共同编译 `lbot_vision_sequence` |
| `src/lbot_vision/CMakeLists.txt` | 生成 msg/srv，链接/安装顺序核心，加入测试 |
| `src/lbot_vision/package.xml` | 声明 rosidl 生成与运行依赖 |
| `src/lbot_vision/src/ros/nut_detector_node.cpp` | 删除“必须恰好三颗”的硬门槛；接入阶段状态、结构化发布、显式事件、图像时效和 RGB/depth 同步保护 |
| `linkerbot/offline.py` | 从中央 YAML 校验顺序参数类型，离线 build 同时检查配置 |
| `tests/test_offline.py` | 增加顺序配置和构建接线契约 |
| `tools/offline_detection/CMakeLists.txt` | ROS-free 构建并运行 `nut_sequence_test` |
| `README.md`、`docs/architecture.md`、`docs/development.md`、`docs/task_framework.md` | 更新入口、边界和控制器交接说明 |
| `docs/offline_detection.md`、`docs/offline_detection_changes.md`、`docs/offline_detection_validation.md` | 保留静态离线回放边界并指向新顺序模块 |
| `src/lbot_vision/include/lbot_vision/detector_fields.inc`、`src/lbot_vision/include/lbot_vision/nut_detector.hpp`、`src/lbot_vision/src/core/nut_detector.cpp`、`src/lbot_vision/tests/nut_detector_core_test.cpp`、`tools/offline_detection/offline_io.cpp` | 此工作区中尚未提交的 Hough 可配置/默认禁用改动 |

## 4. 已执行验证

Windows / MSYS2 UCRT64 / OpenCV 4.12.0：

```powershell
$deps = (Resolve-Path 'artifacts\logs\dev\python-deps').Path
$env:PYTHONPATH = $deps
$env:PATH = (Join-Path $deps 'bin') + ';' + $env:PATH

C:\msys64\ucrt64\bin\python.exe apps/offline_detection.py build
C:\msys64\ucrt64\bin\python.exe apps/offline_detection.py test
```

截至本文写入前的实际结果：

- Python：14 项通过；
- CTest：3/3 通过（detector core、nut sequence、camera geometry）；
- 顺序测试覆盖：冷启动 0/2 颗拒绝、稳定三帧、固定 ID、漏检不推进、顺序保护、目标消失不完成、retry 后要求新观测、3→2→1 重排、幂等事件、reset、尺寸歧义和非法配置。

提交前已再次运行离线 build/test，并执行 `git diff --check`；结果仍为 Python 14/14、CTest 3/3，diff 检查无错误。

## 5. 本机未验证事项

已通过 MSYS2 Bash 实际调用仓库标准脚本，但本机没有 ROS Jazzy，因此两者均在加载 `/opt/ros/jazzy/setup.bash` 时按预期停止：

```bash
./scripts/test.sh
./scripts/build.sh
```

也没有 Gemini 2、深度流、TF、真实机械臂和抓放结果。特别需要现场检查：

- rosidl 生成的 `NutTarget`、`NutSequenceState`、`SetNutState` 能否完整构建；
- 节点话题/service 名称和控制器接线；
- 相机 5 Hz 下 3 稳定帧阈值是否合适；
- 倾斜视角下像素半径排序是否始终区分三种螺母；
- 深度时间差 100 ms、图像年龄 500 ms 是否适配现场帧率；
- 完成反馈必须来自控制器动作结果，不能由“目标消失”代替；
- 本次不验证机械臂运动安全，不得据此开启真实执行。

## 6. 提交前检查

当前已在本地 `branch` 分支，不需要再次创建分支。请先在仓库根目录打开 PowerShell，然后执行：

```powershell
git branch --show-current
git status --short
git diff --check
```

确认输出分支为 `branch`，且没有 `artifacts/`、`build/`、`install/`、`log/` 或图片素材进入待提交列表。

建议一次性暂存本次工作区全部源码/文档改动：

```powershell
git add -- config/vision/nut_detector.yaml
git add -- README.md docs/architecture.md docs/development.md docs/task_framework.md
git add -- docs/offline_detection.md docs/offline_detection_changes.md docs/offline_detection_validation.md
git add -- docs/nut_sequence.md docs/nut_sequence_changes.md
git add -- linkerbot/offline.py tests/test_offline.py tools/offline_detection/CMakeLists.txt tools/offline_detection/offline_io.cpp
git add -- src/lbot_vision/CMakeLists.txt src/lbot_vision/package.xml src/lbot_vision/detector_core.cmake
git add -- src/lbot_vision/include/lbot_vision/detector_fields.inc src/lbot_vision/include/lbot_vision/nut_detector.hpp
git add -- src/lbot_vision/include/lbot_vision/sequence_fields.inc src/lbot_vision/include/lbot_vision/nut_sequence.hpp
git add -- src/lbot_vision/src/core/nut_detector.cpp src/lbot_vision/src/core/nut_sequence.cpp src/lbot_vision/src/ros/nut_detector_node.cpp
git add -- src/lbot_vision/tests/nut_detector_core_test.cpp src/lbot_vision/tests/nut_sequence_test.cpp
git add -- src/lbot_vision/msg/NutTarget.msg src/lbot_vision/msg/NutSequenceState.msg src/lbot_vision/srv/SetNutState.srv
```

然后检查暂存内容：

```powershell
git status --short
git diff --cached --check
git diff --cached --stat
```

## 7. 推荐提交说明

```powershell
git commit -m "feat: add stage-aware nut identities and grasp states" -m "Assigns fixed large/medium/small IDs after stable three-nut detection, re-ranks the remaining 2 and 1 observations by pixel size, and advances only through explicit start/complete/retry/reset feedback. Hough fallback remains available but disabled by default. ROS-free tests passed on Windows with OpenCV 4.12; ROS Jazzy and hardware validation are pending. No robot motion."
```

提交后确认并推送：

```powershell
git log -1 --oneline
git status --short
git push -u origin branch
```

如果 `git push` 提示远端 `branch` 已有新提交，不要使用 `--force`；先 `git fetch origin`，查看差异并与队友确认后再 rebase/merge。
