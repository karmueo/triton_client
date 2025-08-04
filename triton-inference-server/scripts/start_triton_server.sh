#!/bin/bash

# 获取脚本所在目录的父目录作为工作目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$SCRIPT_DIR")"

tritonserver \
    --model-repository="$WORKSPACE_DIR/model_repository" \
    --disable-auto-complete-config \
    --log-verbose=1