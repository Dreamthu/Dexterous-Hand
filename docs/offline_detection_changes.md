# 任务一改动清单与提交说明


本文主要记录任务一的视觉解耦和离线回放。随后已增加固定任务专用的阶段身份/抓取状态，
最新交接与提交清单以 [nut_sequence_changes.md](nut_sequence_changes.md) 为准。
先阅读 [使用说明](offline_detection.md) 和 [2026-09-07 验证报告](offline_detection_validation.md)。
当前代码未执行 git add、commit 或 push；不要将离线验证通过表述为 ROS/真机通过。

## 修改文件

| 文件 | 作用 |
|---|---|
| `.gitignore` | 忽略离线采集数据和实验输出 |
| `README.md` | 增加离线入口、配置和边界说明 |
| `docs/architecture.md` | 说明共享核心及离线依赖方向 |
| `docs/development.md` | 补充离线检查与提交要求，不替代完整 ROS 构建 |
| `src/lbot_vision/CMakeLists.txt` | ROS 节点链接/导出共享检测库 |
| `src/lbot_vision/src/ros/nut_detector_node.cpp` | 移除节点内二维算法副本；先二维后深度；保留旧三维与选择逻辑 |

## 新增文件

| 文件 | 作用 |
|---|---|
| `src/lbot_vision/include/lbot_vision/nut_detector.hpp` | 普通 C++ 配置、二维结果与诊断接口 |
| `src/lbot_vision/include/lbot_vision/detector_fields.inc` | 共享参数名称/类型表，无第二份阈值 |
| `src/lbot_vision/src/core/nut_detector.cpp` | 唯一的场景几何、轮廓、可选 Hough 与候选筛选实现 |
| `src/lbot_vision/detector_core.cmake` | ROS/独立 CMake 共用库目标定义 |
| `tools/offline_detection/CMakeLists.txt` | 不依赖 ROS 的构建、测试及 Windows 路径兼容 |
| `tools/offline_detection/offline_io.hpp` | 离线配置和结果读写接口 |
| `tools/offline_detection/offline_io.cpp` | JSON 传输配置、候选及结果序列化 |
| `tools/offline_detection/main.cpp` | C++ 图片输入与标注/掩膜输出 |
| `linkerbot/offline.py` | 路径/配置校验、构建、回放、子进程清理、版本记录 |
| `apps/offline_detection.py` | argparse 入口，不含识别算法 |
| `config/vision/offline.yaml` | 离线路径与低并发配置，不重复识别阈值 |
| `scripts/build_offline.sh`、`scripts/build_offline.ps1` | Linux/Windows 构建入口 |
| `scripts/run_detector_offline.sh`、`scripts/run_detector_offline.ps1` | Linux/Windows 回放入口 |
| `scripts/test_offline.sh`、`scripts/test_offline.ps1` | Linux/Windows 测试入口 |
| `src/lbot_vision/tests/nut_detector_core_test.cpp` | 二维核心契约测试；明确记录空框旧误检 |
| `tests/test_offline.py` | 13 个配置、入口与真实二进制集成测试 |
| `docs/offline_detection.md` | 配置、使用、现场数据交接、已知限制 |
| `docs/offline_detection_validation.md` | 构建、测试、准确性问题与未验证事项 |
| `docs/offline_detection_changes.md` | 本文件 |

`config/vision/nut_detector.yaml` 新增默认关闭的 `enable_hough_fallback`；黑框最大面积比例仍为 `0.30`，螺母最大像素面积仍为 `100000.0`。CameraGeometry、实验运动代码和外部 SDK 均未修改。
`artifacts/logs/dev/` 的脚本/库/旧版对比器只是本地验证辅助，不是待提交源码。

## 如何提交到名为 branch 的分支

以下命令由开发者核对后手动执行，本文不会自动执行。先进入本 Git 仓库根目录。
注意：当前本地目录虽名为 Dexterous-Hand，但它是主业务仓库的审阅副本，
不要进入主办方 SDK 或原始开发资料目录执行。

```bash
git status --short
git remote -v
git fetch origin
git branch -a
```

根据实际分支状态，**只选择一种**：

```bash
# 本地已经有 branch：
git switch branch

# 只有远端 origin/branch：
git switch --track -c branch origin/branch

# 本地和远端都没有 branch，确认要从当前 main 基线创建：
git switch -c branch
```

未提交改动在切换时可能被保留；若 Git 提示会覆盖改动，停止，不要使用 -f、reset --hard 或
强制切换。应先备份/暂存并处理目标分支差异。远端 branch 状态需以上述 fetch 实际结果为准。
若有他人新增的改动，应先审阅，不能直接覆盖。

检查与选择性暂存（命令可在仓库根目录执行）：

```bash
git diff --check
git add -- .gitignore README.md docs/architecture.md docs/development.md docs/offline_detection.md docs/offline_detection_changes.md docs/offline_detection_validation.md
git add -- apps/offline_detection.py linkerbot/offline.py config/vision/offline.yaml
git add -- src/lbot_vision/CMakeLists.txt src/lbot_vision/detector_core.cmake src/lbot_vision/src/ros/nut_detector_node.cpp src/lbot_vision/src/core/nut_detector.cpp src/lbot_vision/include/lbot_vision/nut_detector.hpp src/lbot_vision/include/lbot_vision/detector_fields.inc src/lbot_vision/tests/nut_detector_core_test.cpp
git add -- tools/offline_detection/CMakeLists.txt tools/offline_detection/main.cpp tools/offline_detection/offline_io.hpp tools/offline_detection/offline_io.cpp tests/test_offline.py
git add -- scripts/build_offline.sh scripts/run_detector_offline.sh scripts/test_offline.sh scripts/build_offline.ps1 scripts/run_detector_offline.ps1 scripts/test_offline.ps1
git update-index --chmod=+x scripts/build_offline.sh scripts/run_detector_offline.sh scripts/test_offline.sh
git diff --cached --check
git diff --cached --stat
git diff --cached
```

不要 `git add -f artifacts`。确认暂存区没有图片、个人路径、OpenCV 源码、临时依赖和构建产物。
在具备 ROS 的队友环境按 HANDOFF 要求补跑 `scripts/test.sh`、`scripts/build.sh`；
若先提交未完成现场验证的开发版本，提交说明必须明确未验证状态，不要标作生产/比赛完成。

提交信息示例：

```bash
git commit -m "refactor: extract shared 2D detector and add offline replay" -m "Tested on Windows with OpenCV 4.12.0: offline build, 13 Python tests and 2 C++ contract tests passed; 8 sample outputs match baseline. Hough fallback is disabled by default for honest 0--3 contour counts. ROS Jazzy build and hardware validation pending."
git push -u origin branch
```

没有权限推送时让仓库维护者授权或使用团队约定的 fork/PR 流程，不要 force push。
此处只提供步骤，本次没有替用户创建分支、提交或推送。
