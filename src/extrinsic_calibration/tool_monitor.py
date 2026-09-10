"""Read-only ROS adapter for the installed robot driver's current-tool service."""

from __future__ import annotations

import time
from typing import Any, Mapping

from .core import CalibrationError, utc_now
from .safety import validate_tool_snapshot


def tool_service_class(config: Mapping[str, Any]):
    try:
        from rosidl_runtime_py.utilities import get_service
        return get_service(str(config["tool_service_type"]))
    except (ImportError, AttributeError, ValueError) as error:
        raise CalibrationError(
            "无法加载工具查询服务类型；请 source 已安装的机器人驱动 overlay "
            f"（需要 {config['tool_service_type']}），不需要复制 SDK 源码到标定目录"
        ) from error


class ToolMonitor:
    def __init__(self, node: Any, config: Mapping[str, Any]) -> None:
        self.config = config
        self.service_type = tool_service_class(config)
        self.client = node.create_client(self.service_type, str(config["tool_service"]))
        self.future: Any = None
        self.request_at = float("-inf")
        self.checked_at = float("-inf")
        self.snapshot: dict[str, Any] | None = None
        self.error = "waiting for current-tool service"
        self.latched_error = ""

    def poll(self) -> None:
        now = time.monotonic()
        if self.latched_error:
            return
        if self.future is not None:
            if self.future.done():
                try:
                    response = self.future.result()
                    if response is None or not response.success:
                        raise RuntimeError("current-tool service reported failure")
                    frame = response.frame
                    snapshot = {
                        "name": str(frame.name),
                        "translation_m": [float(frame.position.x), float(frame.position.y), float(frame.position.z)],
                        "euler_rad": [float(frame.euler.x), float(frame.euler.y), float(frame.euler.z)],
                    }
                    validate_tool_snapshot(snapshot, self.config)
                    snapshot["checked_at"] = utc_now()
                    self.snapshot = snapshot
                    # Age the observation from request time, not receipt time: a slow
                    # response must not make an old tool observation look fresh.
                    self.checked_at = self.request_at
                    self.error = ""
                except CalibrationError as error:
                    self.latched_error = str(error) + "; end this session and check the tool configuration"
                except Exception as error:
                    self.error = str(error)
                    self.snapshot = None
                finally:
                    self.future = None
            elif now - self.request_at > float(self.config["tool_service_timeout_s"]):
                self.client.remove_pending_request(self.future)
                self.future.cancel()
                self.future = None
                self.snapshot = None
                self.error = "current-tool service timeout"
        if (not self.latched_error and self.future is None
                and now - self.request_at >= float(self.config["tool_poll_interval_s"])):
            if self.client.service_is_ready():
                self.request_at = now
                self.future = self.client.call_async(self.service_type.Request())
            else:
                self.snapshot = None
                self.error = "current-tool service unavailable"

    def require_current(self) -> dict[str, Any]:
        if self.latched_error:
            raise CalibrationError(self.latched_error)
        if self.error or self.snapshot is None:
            raise CalibrationError(self.error or "tool has not been checked")
        if time.monotonic() - self.checked_at > float(self.config["maximum_tool_age_s"]):
            raise CalibrationError("current-tool observation is stale")
        return dict(self.snapshot)
