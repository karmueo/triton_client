#!/bin/bash

# 安装 Triton 客户端依赖

echo "🔧 安装 Triton 客户端依赖..."

# 更新包列表
apt update

# 安装 Python 和 pip
apt install -y python3 python3-pip

# 安装 Triton 客户端库
pip3 install tritonclient[all]

# 安装其他依赖
pip3 install numpy

echo "✅ 依赖安装完成!"

echo "📋 已安装的包:"
pip3 list | grep -E "(tritonclient|numpy)"
