# Triton C++ 客户端构建指南

这个项目包含了用于调用 Triton 推理服务器的 C++ 客户端实现。

## 文件说明

- `client.cpp` - 功能完整的 Triton C++ 客户端（需要 Triton 客户端库）
- `simple_client.cpp` - 简化版 C++ 客户端（需要 Triton 客户端库）
- `minimal_client.cpp` - 最小 HTTP 客户端（仅需要 curl 和 jsoncpp）
- `CMakeLists.txt` - 主要的 CMake 配置文件
- `CMakeLists_simple.txt` - 简化版 CMake 配置文件
- `scripts/build_cpp_client.sh` - 自动化构建脚本

## 快速开始

### 1. 使用自动构建脚本（推荐）

```bash
# 基本构建
./scripts/build_cpp_client.sh

# 使用代理构建
./scripts/build_cpp_client.sh --proxy http://192.168.1.110:7890

# 或者分别设置HTTP和HTTPS代理
./scripts/build_cpp_client.sh --http-proxy http://192.168.1.110:7890 --https-proxy http://192.168.1.110:7890
```

### 2. 手动构建

#### 安装依赖
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    pkg-config \
    git
```

#### 配置代理（如果需要）
```bash
# 设置环境变量
export HTTP_PROXY=http://192.168.1.110:7890
export HTTPS_PROXY=http://192.168.1.110:7890

# 或者配置Git代理
git config --global http.proxy http://192.168.1.110:7890
git config --global https.proxy http://192.168.1.110:7890
```

#### 构建项目
```bash
mkdir build && cd build

# 基本构建
cmake -DCMAKE_BUILD_TYPE=Release ..

# 带代理的构建
cmake -DCMAKE_BUILD_TYPE=Release \
      -DHTTP_PROXY=http://192.168.1.110:7890 \
      -DHTTPS_PROXY=http://192.168.1.110:7890 \
      ..

make -j$(nproc)
```

## 运行客户端

### 1. 启动 Triton 服务器
```bash
./scripts/start_triton_server.sh
```

### 2. 运行客户端

#### 最小客户端（推荐开始使用）
```bash
# 直接运行
./build/minimal_client

# 或使用运行脚本
./build/run_minimal_client.sh
```

#### 简单客户端（如果构建成功）
```bash
./build/run_simple_client.sh
```

#### 完整客户端（如果构建成功）
```bash
# 基本用法
./build/run_client.sh

# 指定服务器地址
./build/run_client.sh --url localhost:8001

# 指定模型名称
./build/run_client.sh --model Times_Classify

# 启用详细日志
./build/run_client.sh --verbose

# 查看帮助
./build/run_client.sh --help
```

## 代理配置

### 环境变量方式
```bash
export HTTP_PROXY=http://192.168.1.110:7890
export HTTPS_PROXY=http://192.168.1.110:7890
./scripts/build_cpp_client.sh
```

### 命令行参数方式
```bash
./scripts/build_cpp_client.sh --proxy http://192.168.1.110:7890
```

### CMake 参数方式
```bash
cmake -DHTTP_PROXY=http://192.168.1.110:7890 \
      -DHTTPS_PROXY=http://192.168.1.110:7890 \
      ..
```

## 故障排除

### 构建问题

#### 1. Triton 客户端库未找到
如果系统中没有预安装的 Triton 客户端库，CMake 会尝试从源码构建。这可能需要较长时间和网络连接。

**解决方案:**
- 使用 `minimal_client.cpp`，它只依赖 curl 和 jsoncpp
- 或者手动安装 Triton 客户端库

#### 2. 网络连接问题
如果在下载 Triton 客户端源码时遇到网络问题：

**解决方案:**
- 配置代理（见上文代理配置）
- 使用最小客户端版本
- 手动下载源码到本地

#### 3. 依赖库缺失
```bash
# 安装缺失的依赖
sudo apt install -y libcurl4-openssl-dev libjsoncpp-dev
```

#### 4. CMake 版本过低
```bash
# 升级CMake
sudo apt install -y cmake
```

### 运行时问题

#### 1. 服务器连接失败
- 确保 Triton 服务器正在运行
- 检查服务器地址和端口
- 检查防火墙设置

#### 2. 模型加载失败
- 检查模型文件是否存在
- 验证模型配置文件
- 查看服务器日志

#### 3. 库文件找不到
```bash
# 设置库路径
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
```

## 性能优化

### 编译优化
```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native" \
      ..
```

### 并行构建
```bash
make -j$(nproc)  # 使用所有CPU核心
```

## 开发说明

### 添加新功能
1. 修改相应的 `.cpp` 文件
2. 更新 `CMakeLists.txt`（如果需要新依赖）
3. 重新构建项目

### 调试
```bash
# 构建调试版本
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# 使用GDB调试
gdb ./minimal_client
```

### 代码格式化
```bash
# 使用clang-format格式化代码
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

## 许可证

本项目遵循相关开源许可证。Triton 客户端库遵循其自己的许可证条款。
