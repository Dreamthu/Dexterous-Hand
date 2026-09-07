# 开发约定

本页描述日常流程；目录边界、接口封装、安全要求和完成定义以仓库根目录的
[`HANDOFF.md`](../HANDOFF.md) 为准。

## 日常流程

1. 只修改 `linkerbot_ws`；外部 SDK 目录视为静态依赖。
2. 参数变化提交到 `config/`，不要写死在入口脚本。
3. 使用 `./scripts/build.sh` 构建，避免 WSL 自动使用 32 路并发导致 OOM。
4. 使用 `./scripts/test.sh` 做配置和入口自检。
5. 识别阶段使用 `./scripts/run_perception.sh`，不得启动实验运动代码。

## 新增功能

- 纯算法放入独立库或 Python module，避免直接读写 ROS topic。
- ROS subscriber/publisher/service client 放入命名明确的 adapter/node 文件。
- 进程组合放入 `apps/`，再提供对应的 `scripts/*.sh` 稳定入口。
- 运行结果、图片和日志写入 `artifacts/`，不要写进 `src/` 或 `config/`。
- 相机像素转三维统一复用 `lbot_vision::CameraGeometry`；不要新增只读取 `K` 的投影实现，
  也不要用仓库内文件覆盖设备发布的 `CameraInfo`。

## 提交前检查

```bash
./scripts/test.sh
./scripts/build.sh
git status --short
```

真实相机/机械臂测试结果应在提交说明中注明设备、固件、分辨率和测试时长。
相机 profile 或图像 topic 发生变化时，还必须同步更新
[`troubleshooting.md`](troubleshooting.md) 中的已验证运行状态和排查步骤。

## 离线核心改动的额外检查

```bash
bash scripts/build_offline.sh
bash scripts/test_offline.sh
```

这组检查无需 ROS，但**不能代替**上面的完整 ROS 自检与低并发构建。
无 ROS 的开发环境应明确记录未执行项，交给现场队友补测，不能把离线通过写成整仓构建通过。
新 Bash 入口首次提交时请记录可执行位（示例见 [本次提交说明](offline_detection_changes.md)）。
不得提交 `artifacts/` 内的私有图片、运行日志、测试依赖或 `build/` 产物。
