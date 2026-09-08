# 2026-09-08 main/branch 整合与验证记录

## 整合基线

- 工作分支：`branch`，原提交 `0f87b62`（阶段身份/抓取状态）。
- 合入的主分支：`origin/main`，提交 `79bae47`（R8 外参标定、模型资产、相机运行修复）。
- 使用 merge 保留两边历史，不 rebase、不覆盖 main、不强制推送。
- PR 方向：**base = main，compare = branch**。

此记录说明本机可完成的验证，不代表 ROS 或实机验收完成。若主分支之后有新提交，需再次
同步并复测。本次没有连接相机、发布 TF 或发送机械臂运动命令。

## 冲突处理与修复

### 1. 检测节点

唯一文本冲突为 `src/lbot_vision/src/ros/nut_detector_node.cpp` 的 `process()`。
保留 branch 的共享二维核心、固定 ID、显式完成反馈、图像时效、重复时间戳、深度时间差和
单观测 TF 逻辑，不恢复 main 中的第二份旧检测算法。

融合 main 的缺输入诊断：

1. 检查 RGB 时效和重复时间戳；
2. 运行共享二维核心，更新并发布不含三维位置的 sequence；
3. 如缺少深度或 CameraInfo，在调试图中显示并发布 `waiting_for_depth`、
   `waiting_for_camera_info` 或 `waiting_for_depth_and_camera_info`；
4. 输入齐全后，继续执行原有阶段、篮筐、图像尺寸、深度同步和定位检查。

缺少定位数据时仍有 RGB 调试图和二维状态，但不发布新的三维抓取坐标。
该 ROS 控制流目前有源码结构回归检查，**没有用这种检查冒充 ROS 运行验证**。

### 2. Windows 离线兼容与相机锁

- `runtime.py` 不再在模块顶层导入 `fcntl`，配置、离线回放及帮助入口可在 Windows 导入。
- 仅实际申请相机锁时加载 POSIX 依赖；不支持的平台返回明确的 Linux/POSIX/ROS 提示，
  不能绕过锁直接启动真实相机。
- 保留 Linux 非阻塞独占锁和相机 exec 入口的可继承描述符。
- 锁争用/文件操作错误时关闭已打开的文件，并为目录或权限问题返回可操作错误。
- 感知多进程入口在清理子进程后显式关闭锁。
- 修正测试中写死 POSIX 斜杠的路径断言；没有删除真正依赖 ROS 的查看器测试。

### 3. 工作区路径

- 保留 main 的默认外部路径：`../OrbbecSDK_ROS2`、`../lbot_ws`。
- `_common.sh` 在默认配置后加载已被忽略的 `config/workspace.local.env`。
- 本地覆盖中的相对路径仍以本仓库根目录为准，不依赖调用者当前目录。
- 对齐 README、HANDOFF 和架构文档中的目录说明，不移动/修改外部 SDK。

### 4. 回归测试

新增 `tests/test_offline_integration.py`，由现有离线 test 入口自动发现：

- 无 fcntl 环境下的全新 Python 进程导入和配置读取；
- 从其他目录运行离线/相机/感知帮助，以及相机 dry-run；
- 原生路径与不支持平台的错误信息；
- 模拟 POSIX 后端的锁争用、I/O 失败和资源关闭；
- 相机进程提前退出时感知入口释放锁；
- 合并后的 ROS 源码接线顺序和本地覆盖配置加载顺序。

其中模拟 POSIX 的单元测试只证明错误处理/资源清理；真正的 Linux flock 互斥、exec 后
持锁和 ROS 生命周期仍需在 Linux 验证。原 `test_runtime.py` 在 POSIX 上保留真实互斥测试，
在 Windows 上验证明确拒绝启动。

## 实际验证结果

本机：Windows，MSYS2 UCRT64 C++ 工具链，C++ OpenCV 4.12.0。
离线入口使用 UCRT Python；标定测试另外使用 Python 3.12 和 OpenCV contrib headless 4.12.0。
所有临时依赖、构建和日志都在被忽略的目录，不随提交上传。

| 检查 | 实际结果 | 能证明的范围 |
|---|---|---|
| 离线 configure/build | 通过 | 配置校验与共享 C++ 构建接线 |
| clean-first、并行度 2 重编译 | 15 个构建步骤完成 | 不是仅复用旧二进制 |
| 离线 Python 回归 | **25/25 通过，无跳过** | 原 14 项 + 本次 11 项整合契约 |
| CTest | **3/3 通过** | detector core、nut sequence、camera geometry |
| 标定板测试 | **6/6 通过** | 标定板几何、生成结果和 ArUco 识别契约 |
| IMU 测试 | **11/11 通过** | 静止窗口、运动拒绝、时效及采样数据检查 |
| Python 全量 discover | **48 项中 47 通过、1 项环境错误，无跳过** | 查看器测试缺 `ament_index_python`/`rqt_image_view`；全量套件并非全绿 |
| 外参工具 `--help` | 通过 | 合并后 CLI 可导入和解析参数，不代表求解通过 |
| 外参配置与 R8 网格 SHA-256 | 通过 | 配置及版本化网格一致 |
| 外参 `robot_pose` 检查 | **未通过：缺 PyKDL** | 未证明正运动学、关节角拒绝或完整外参求解 |
| 本地环境覆盖的 Bash 实际执行 | 通过 | 临时夹具内验证覆盖顺序和跨目录路径解析；没有使用真实 ROS |
| `scripts/test.sh` | **退出 2：缺 ROS Jazzy setup** | `/opt/ros/jazzy/setup.bash` 不存在 |
| `scripts/build.sh` | **退出 2：缺 ROS Jazzy setup** | 没有完成 rosidl/ROS 节点构建 |
| `git diff --check` | 通过 | 无检测到的空白问题/冲突标记 |

离线契约通过不等于比赛识别准确率通过，也不等于真实抓取成功。

## 复现入口

配置好 C++17、CMake、OpenCV 开发库与 PyYAML 后：

```powershell
./scripts/build_offline.ps1
./scripts/test_offline.ps1
```

Linux 可用同名 `.sh`。离线命令只需要离线依赖；标定板测试额外需要 numpy、带 ArUco 的
OpenCV 和 Pillow。使用对应 Python 环境运行：

```bash
python -m unittest discover -s tests -p 'test_calibration_board.py' -v
python -m unittest discover -s tests -p 'test_imu_sampling.py' -v
python -m unittest discover -s tests -v
```

最后一个命令还要求真实可用的 ROS 查看器；在无 ROS 环境报错不能写成全通过。

本地证据（被 Git 忽略）：

- `artifacts/logs/dev/integration-clean-build.log`
- `artifacts/logs/dev/integration-offline-tests.log`
- `artifacts/logs/dev/integration-python-tests.log`
- `artifacts/logs/dev/integration-calibration-preflight.log`
- `artifacts/logs/dev/integration-workspace-env.log`
- `artifacts/logs/dev/integration-test.sh.log`
- `artifacts/logs/dev/integration-build.sh.log`

## PR / 现场验收清单（待完成）

- [ ] 在 Ubuntu / ROS Jazzy 上配置真实的外部工作区与 `lbot_arm_interfaces`，运行
      `./scripts/test.sh` 和 `./scripts/build.sh`。
- [ ] 运行包含 C++ 测试的 ROS 构建/测试。标准 build 脚本使用 `BUILD_TESTING=OFF`，
      不能把它当作 C++ 测试已执行。开发终端可在标准构建后运行：

  ```bash
  source /opt/ros/jazzy/setup.bash
  source install/setup.bash
  colcon build --base-paths src --packages-select lbot_vision --executor sequential \
    --cmake-args -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
  source install/setup.bash
  colcon test --packages-select lbot_vision --executor sequential --return-code-on-test-failure
  colcon test-result --verbose
  ```

  保持低并发（例如先 `export MAKEFLAGS='-j2 -l2'`），检查新 msg/srv 生成与节点链接。
- [ ] 验证 `NutTarget`、`NutSequenceState`、`SetNutState` 及控制器反馈接口。
- [ ] 不接深度/CameraInfo 时仍有 RGB 调试图和清晰等待原因；不得输出有效三维位置。
- [ ] 验证初始稳定三颗、漏检不推进、显式 complete 后 3→2→1、retry/reset 和幂等事件。
- [ ] 验证旧图像、重复时间戳、RGB/depth 不同步、TF 缺失及相机断开后的安全行为。
- [ ] 验证两个相机入口不能同时启动，退出/异常后锁能释放，exec 后仍持续持锁。
- [ ] 安装 PyKDL 并补完 FK、同步图像/真实关节角和 R8 外参求解验证。
- [ ] 记录设备、固件、profile、时长；本 PR 不以现有结果宣称机械臂运动安全或实机抓放通过。

在 ROS / 现场结果补齐前，建议以 Draft PR 交接，而不是将它标记为已完成实机验收。
