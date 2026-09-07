# 迁移来源

本仓库于 2026-09-06 从以下本地代码整理而来：

- `src/lbot_vision`：来自 `../Dexterous-Hand/src/lbot_vision`；迁移时
  `Dexterous-Hand` 的提交为 `678ab21065236f13c4fa2de5808f68fb887ae05c`。
- `tools/camera_calibration`：来自原独立目录
  `../camera_intrinsic_calibration`。

迁移后，视觉节点继续保留原实现；旧任务节点因包含未校验的机械臂占位坐标，被移动到
`src/lbot_vision/experimental/` 且不参与构建。标定工具在原功能基础上增加了棋盘画面占比
检查，用于防止“重投影误差很小但焦距明显错误”的退化标定。

外部目录 `../OrbbecSDK_ROS2` 与外部机器人 SDK 不属于本 Git 仓库。它们的升级应当作为
显式维护操作，并在本文件记录经过验证的新版本；仓库脚本不会自动修改这两个目录。
当前已验证相机 SDK 工作区为 `../OrbbecSDK_ROS2`，分支 `v2-main`，提交
`8e7cad2b`；该版本的 Gemini2 同步 IMU 话题为 `/camera/gyro_accel/sample`。
