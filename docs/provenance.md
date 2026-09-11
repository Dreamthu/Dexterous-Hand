# 迁移来源

本仓库于 2026-09-06 从以下本地代码整理而来：

- `src/lbot_vision`：来自 `../Dexterous-Hand/src/lbot_vision`；迁移时
  `Dexterous-Hand` 的提交为 `678ab21065236f13c4fa2de5808f68fb887ae05c`。
- 最初曾迁入 `../camera_intrinsic_calibration` 的单目棋盘内参工具；确认 Gemini 2 使用
  设备厂内参后，该工具、配置、入口和历史本地产物已于 2026-09-06 删除。
- `src/lbot_arm_interfaces` 和 `开发资源/assets/` 源自 Dexterous-Hand
  SDK；它们仍是螺母抓取任务控制、标定、MoveIt、Rerun 和对象模型所必需，因此保留。
  `src/lbot_driver`、`src/lbot_demo`、`src/lbot_teleop` 及其 SDK 动态库已移除，机器人驱动
  改由外部环境提供。

迁移后，视觉节点继续保留原实现；旧任务节点因包含未校验的机械臂占位坐标，被移动到
`src/lbot_vision/experimental/` 且不参与构建。仓库现在只预留设备安装和现场相关外参的
目录结构，不再维护相机内参标定或覆盖路径。

外部目录 `../OrbbecSDK_ROS2` 与外部机器人驱动环境不属于本 Git 仓库。它们的升级应当作为
显式维护操作，并在本文件记录经过验证的新版本；仓库脚本不会自动修改这些目录。
当前已验证相机 SDK 工作区为 `../OrbbecSDK_ROS2`，分支 `v2-main`，提交
`8e7cad2b`；该版本的 Gemini2 同步 IMU 话题为 `/camera/gyro_accel/sample`。
