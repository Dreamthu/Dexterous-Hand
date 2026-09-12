# 蓝筐 HSV 调参界面

在有图形桌面的终端中运行，需要 Python 3、带 GUI 的 OpenCV、NumPy 和 PyYAML。
ROS 实时画面另外需要 `rclpy`、`cv_bridge`、`sensor_msgs`，并提前启动相机。

```bash
cd .
source install/setup.bash
python3 scripts/tune_blue_hsv.py
```

默认订阅 `/camera/color/image_raw`，也可以指定 `--topic /其他彩色图话题`。
单张图片模式无需 ROS：

```bash
python3 scripts/tune_blue_hsv.py --image /path/to/color.png
```

六个滑块对应检测器的 `blue_h_min/max`、`blue_s_min/max`、`blue_v_min/max`。
初值读取 `config/vision/nut_detector.yaml`，可用 `--config` 指定其他同结构配置。
H 使用 OpenCV 的 0～180 范围，S/V 为 0～255。
任一通道下限超过上限时，界面将对应上限同步提高。
旧配置未填写 `blue_s_max`、`blue_v_max` 时，调参界面、ROS 节点和离线检测均默认使用 255。

四幅预览依次是原图、原始 HSV 掩膜、7×7 闭运算掩膜、提取结果。
掩膜在原始分辨率计算，显示时才缩放，闭运算与当前蓝筐检测器一致。
白色表示通过筛选。点击左上角原图可显示、打印该像素的 HSV 值。
这些预览只反映颜色筛选，没有应用黑框左右关系、面积或四边形筛选。

- 空格：冻结/恢复画面，冻结时仍可调阈值。
- `R`：恢复启动时的阈值。
- `S`：在 `artifacts/hsv_tuning/` 导出带时间戳的参数 YAML，并在终端打印六项参数。
- `Q` / Esc / 关闭窗口：退出。

可用 `--output-dir` 更改导出目录，用 `--width`、`--height` 限制单幅预览尺寸。
导出文件只包含六个蓝色阈值。将这六项数值更新到 `config/vision/nut_detector.yaml`
对应位置，再重启检测节点使其生效；不要用导出文件覆盖整份检测配置。
脚本不会自动修改检测器配置或运行中节点的参数。
首次使用新增的 S/V 上限，需要重新构建 `lbot_vision`，使检测节点支持这两个参数。
