# lbot_control 视觉接口

任务流程以根目录 [`docs/task_framework.md`](../../docs/task_framework.md) 为准。

`lbot_control` 已提供 `RosVisionSystem`，它直接消费以下接口并适配为
`lbot_control::VisionSystem`：

1. `initial_scene()`：返回基座坐标系下、按大中小排列的三颗螺母位姿和三个目标格子位姿；
2. `check_nut_in_source(size)`：放置撤离后、机械臂回到黑框上方之前，判断指定尺寸螺母是否
   仍在黑框内；此时相机视野不会被 `above_table` 姿态遮挡。

控制器在每次抓取前通过 `/nut_detections/set_state` 发送 `start`，检查后发送 `complete` 或
`retry`；视觉节点本身仍不发送机械臂命令。

任务层不要求灵巧手反馈，也不要求检查蓝框中的放置结果。视觉包不得直接发送运动命令。

大螺母单次抓放入口 `task_mode:=pregrasp` 使用独立的观测捕捉：
`/nut_detections/large` 发布当前最大候选的位置；`/nut_slots` 在黑框、蓝筐、深度及相机几何可用时
发布同一帧的三坑位坐标，不依赖三颗螺母的数量或稳定身份。坐标系和时间戳保留在消息头中，
缺少外参时仍可能是相机坐标，控制层仅接受新鲜的 `base_link` 观测。
控制层在起臂前保存大螺母和到基座原点水平距离最大的坑位，后续遮挡不覆盖保存结果。
完整任务仍通过 `initial_scene()` 检查三颗螺母与三坑位的同帧定位结果。
