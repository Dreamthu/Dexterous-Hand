#!/bin/bash
set -e

# 功能包 lib 目录（脚本所在目录就是 lib）
LIB_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Target lib directory: $LIB_DIR"

# 根据系统架构选择对应文件夹
ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
    SRC_DIR="$LIB_DIR/linux_x64"
elif [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then
    SRC_DIR="$LIB_DIR/linux_arm64"
else
    echo "Unsupported architecture: $ARCH"
    exit 1
fi

TARGET_SO="$LIB_DIR/liblbot_api_cpp.so.1.0.0"

echo "Using source directory: $SRC_DIR"
echo "Removing old files..."
rm -f "$LIB_DIR/liblbot_api_cpp.so" \
      "$LIB_DIR/liblbot_api_cpp.so.1" \
      "$TARGET_SO"

echo "Copying..."
cp "$SRC_DIR/liblbot_api_cpp.so" "$TARGET_SO"

echo "Creating symlinks..."
ln -s "liblbot_api_cpp.so.1.0.0" "$LIB_DIR/liblbot_api_cpp.so.1"
ln -s "liblbot_api_cpp.so.1.0.0" "$LIB_DIR/liblbot_api_cpp.so"

echo "[SUCCESS] Installed."

