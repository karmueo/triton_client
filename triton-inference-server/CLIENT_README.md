# Triton Times_Classify 客户端使用指南

这个文件夹包含了用于调用 Triton 推理服务器上的 `Times_Classify` 模型的客户端代码。

## 文件说明

- `client.py` - 功能完整的 Triton 客户端，支持多种选项
- `simple_client.py` - 简化版客户端，适合快速测试
- `scripts/install_client_deps.sh` - 依赖安装脚本

## 快速开始

### 1. 安装依赖

```bash
# 运行依赖安装脚本
./scripts/install_client_deps.sh

# 或者手动安装
pip3 install tritonclient[all] numpy
```

### 2. 启动 Triton 服务器

```bash
# 在另一个终端中启动服务器
./scripts/start_triton_server.sh
```

确保服务器启动成功，没有模型加载错误。

### 3. 运行客户端

#### 使用简单客户端
```bash
python3 simple_client.py
```

#### 使用完整客户端
```bash
# 基本用法
python3 client.py

# 指定不同的服务器地址
python3 client.py --url localhost:8001

# 使用 gRPC 协议
python3 client.py --protocol grpc --url localhost:8001

# 使用自定义数据文件
python3 client.py --data-file your_data.npy

# 指定批处理大小
python3 client.py --batch-size 4
```

## 模型信息

- **模型名称**: Times_Classify
- **输入**: 
  - 名称: `input`
  - 形状: `[batch_size, 20, 14]`
  - 数据类型: `FP32`
- **输出**: 
  - 名称: `output`
  - 形状: `[batch_size, 2]`
  - 数据类型: `FP32`
- **类别标签**: 
  - 0: bird (鸟类)
  - 1: uav (无人机)

## 输入数据格式

模型期望接收形状为 `(20, 14)` 的时间序列数据：
- 20 个时间步
- 每个时间步 14 个特征

示例数据准备：
```python
import numpy as np

# 创建示例数据
data = np.random.randn(20, 14).astype(np.float32)

# 保存为文件
np.save('my_data.npy', data)

# 使用文件进行推理
# python3 client.py --data-file my_data.npy
```

## 输出解释

模型输出两个数值，分别对应两个类别的 logits。客户端会自动：
1. 应用 softmax 转换为概率
2. 选择概率最高的类别作为预测结果
3. 显示置信度和概率分布

示例输出：
```
🎯 预测结果:
预测类别: bird
置信度: 0.7234
概率分布: bird=0.7234, uav=0.2766
```

## 故障排除

### 服务器连接问题
- 确保 Triton 服务器正在运行
- 检查端口是否正确 (默认 HTTP: 8000, gRPC: 8001)
- 确保防火墙没有阻止连接

### 模型加载问题
- 检查模型文件是否存在于 `model_repository/Times_Classify/1/`
- 确认配置文件 `config.pbtxt` 格式正确
- 查看服务器日志获取详细错误信息

### 输入数据问题
- 确保输入数据形状为 `(20, 14)` 或 `(batch_size, 20, 14)`
- 确保数据类型为 `float32`
- 检查数据范围是否合理

## 高级用法

### 批处理推理
```python
# 准备多个样本
batch_data = np.random.randn(8, 20, 14).astype(np.float32)

# 使用批处理
python3 client.py --batch-size 8
```

### 性能监控
客户端会显示推理时间，可用于性能分析：
```
⚡ 推理时间: 0.0234 秒
```

### 自定义标签
修改客户端代码中的标签列表：
```python
labels = ["your_class_1", "your_class_2"]
```
