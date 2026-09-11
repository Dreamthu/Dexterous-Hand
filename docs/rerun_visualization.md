# Rerun Task Visualization

`lbot_rerun` is a read-only ROS 2 bridge. It never creates a motion service
client or a command publisher, so it can be started before, during, or after a
task run without changing the controller's behavior.

Install the supported SDK once in the same Python environment used by ROS 2:

```bash
python3 -m pip install --user --break-system-packages 'rerun-sdk>=0.19,<0.20'
```

Build and source the workspace, then start the visualizer:

```bash
ros2 launch lbot_rerun lbot_rerun.launch.py
```

The launch file does not start the robot driver, Gemini2, detector, or task
controller. Start those with their existing ROS 2 launch entries. It subscribes
to `/robot1/left_arm/joint_states`, `/robot1/left_arm/pose_states`, the O6
command topic, `/camera/color/camera_info`, `/camera/depth/color/points`,
`/nut_detections`, `/nut_detections/sequence`, and `/nut_slots`.

The bridge uses `base_link` as the Rerun scene root. In this task it is the
midpoint between the two shoulders. The workstation URDF names the corresponding
model link `base_base_link`; the bridge explicitly aliases that link to
`base_link` and does not include the model-only 1.2 m stand offset.

All point clouds, detected nuts, slots, and end-effector poses are transformed
to `base_link` at their message timestamps. If TF cannot provide that transform,
the bridge skips the affected frame and reports the reason instead of placing
camera-frame data into the base-frame scene.

Gemini2 point clouds require `enable_point_cloud:=true` in the camera launch.
The repository's camera configuration currently leaves that option disabled.
`max_cloud_points` and `cloud_stride` in
[`config/viewer/rerun.yaml`](../config/viewer/rerun.yaml) bound the data sent to
Rerun. Set `recording_path` there to an absolute `.rrd` path to save a replay
when the visualizer exits.

The O6 hand has no position feedback. Its display pose is inferred from the
last six-element command and the visual-only `hand_open`/`hand_closed` arrays in
the Rerun configuration. Update those arrays after hand calibration; they do
not affect the hardware command path.
