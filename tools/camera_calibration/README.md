# 相机内参标定工具

`calibrate_camera.py` 是可复用的 ROS 2 单目棋盘标定工具。仓库正常使用时不要在这里
维护另一份参数；棋盘、话题和输出路径统一配置在：

```text
config/calibration/chessboard.yaml
```

从仓库根目录执行一条命令即可自动启动相机、打开标定窗口并在退出后清理相机进程：

```bash
./scripts/calibrate_camera.sh
```

窗口按键：

- `Space`：保存当前有效样本；
- `D`：撤销最后一个样本；
- `C`：达到最低样本数后标定；
- `Esc`：退出，不生成内参。

结果写入 `artifacts/calibration/<时间戳>/`：

```text
images/sample_*.png
camera_info.yaml
calibration_report.yaml
```

当前默认棋盘为 11 × 8 个方格，即 OpenCV 检测 10 × 7 个内角点，单格边长
15 mm。棋盘自身必须粘贴在刚性平板上；不同样本应覆盖画面中央、四边和四角，改变
距离并分别绕两个方向倾斜。

该 Python 文件仍支持独立 CLI，具体参数可通过以下命令查看：

```bash
python3 tools/camera_calibration/calibrate_camera.py --help
```
