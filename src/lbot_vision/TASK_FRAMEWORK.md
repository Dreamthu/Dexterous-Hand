# lbot_control 视觉接口

任务流程以根目录 [`docs/task_framework.md`](../../docs/task_framework.md) 为准。

视觉适配器只需要实现 `lbot_control::VisionSystem`：

1. `initial_scene()`：返回基座坐标系下、按大中小排列的三颗螺母位姿和三个目标格子位姿；
2. `check_nut_in_source(size)`：放置撤离后、机械臂回到黑框上方之前，判断指定尺寸螺母是否
   仍在黑框内；此时相机视野不会被 `above_table` 姿态遮挡。

任务层不要求灵巧手反馈，也不要求检查蓝框中的放置结果。视觉包不得直接发送运动命令。
