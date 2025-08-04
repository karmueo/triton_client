#!/bin/bash

# Triton C++ 客户端构建脚本

set -e  # 遇到错误时退出

echo "🔧 构建 Triton C++ 客户端..."

# 代理设置
HTTP_PROXY=${HTTP_PROXY:-""}
HTTPS_PROXY=${HTTPS_PROXY:-""}

# 检查命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --proxy)
            HTTP_PROXY="$2"
            HTTPS_PROXY="$2"
            shift 2
            ;;
        --http-proxy)
            HTTP_PROXY="$2"
            shift 2
            ;;
        --https-proxy)
            HTTPS_PROXY="$2"
            shift 2
            ;;
        --help)
            echo "用法: $0 [选项]"
            echo "选项:"
            echo "  --proxy URL          设置HTTP和HTTPS代理"
            echo "  --http-proxy URL     设置HTTP代理"
            echo "  --https-proxy URL    设置HTTPS代理"
            echo "  --help               显示此帮助信息"
            exit 0
            ;;
        *)
            echo "未知选项: $1"
            echo "使用 --help 查看帮助信息"
            exit 1
            ;;
    esac
done

# 设置代理环境变量
if [ -n "$HTTP_PROXY" ]; then
    export HTTP_PROXY="$HTTP_PROXY"
    export http_proxy="$HTTP_PROXY"
    echo "🌐 设置HTTP代理: $HTTP_PROXY"
fi

if [ -n "$HTTPS_PROXY" ]; then
    export HTTPS_PROXY="$HTTPS_PROXY"
    export https_proxy="$HTTPS_PROXY"
    echo "🌐 设置HTTPS代理: $HTTPS_PROXY"
elif [ -n "$HTTP_PROXY" ]; then
    export HTTPS_PROXY="$HTTP_PROXY"
    export https_proxy="$HTTP_PROXY"
    echo "🌐 设置HTTPS代理: $HTTP_PROXY"
fi

# 设置Git代理
if [ -n "$HTTP_PROXY" ]; then
    git config --global http.proxy "$HTTP_PROXY" 2>/dev/null || true
    git config --global https.proxy "$HTTPS_PROXY" 2>/dev/null || true
    echo "🔧 已配置Git代理"
fi

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 创建构建目录
BUILD_DIR="$PROJECT_DIR/build"
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

echo "📁 工作目录: $(pwd)"
echo "📁 项目目录: $PROJECT_DIR"

# 检查是否安装了必要的依赖
echo "🔍 检查依赖..."

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake 未安装，正在安装..."
    apt update && apt install -y cmake
fi

# 检查必要的开发库
echo "📦 安装必要的开发库..."
apt update
apt install -y \
    build-essential \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    pkg-config \
    wget \
    git \
    clang \
    cmake

# 检查是否存在 Triton 客户端库
TRITON_CLIENT_EXISTS=false
if [ -f "/usr/include/triton/client/http_client.h" ] || \
   [ -f "/usr/local/include/triton/client/http_client.h" ]; then
    echo "✅ 发现系统中已安装 Triton 客户端库"
    TRITON_CLIENT_EXISTS=true
fi

# 如果没有 Triton 客户端库，尝试下载预编译版本或使用最小客户端
if [ "$TRITON_CLIENT_EXISTS" = false ]; then
    echo "⚠️  系统中未找到 Triton 客户端库"
    echo "📥 将构建最小客户端（使用 HTTP API）"
    
    # 使用简化的 CMake 配置
    echo "🔨 配置构建..."
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    if [ -n "$HTTP_PROXY" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DHTTP_PROXY=$HTTP_PROXY -DHTTPS_PROXY=$HTTPS_PROXY"
    fi
    cmake $CMAKE_ARGS -f "$PROJECT_DIR/CMakeLists_simple.txt" "$PROJECT_DIR"
else
    # 使用完整的 CMake 配置
    echo "🔨 配置构建（使用完整 Triton 客户端库）..."
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    if [ -n "$HTTP_PROXY" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DHTTP_PROXY=$HTTP_PROXY -DHTTPS_PROXY=$HTTPS_PROXY"
        echo "🌐 传递代理设置到CMake: $HTTP_PROXY"
    fi
    cmake $CMAKE_ARGS "$PROJECT_DIR"
fi

# 构建
echo "🔨 开始构建..."
make -j$(nproc)

echo "✅ 构建完成!"

# 检查构建结果
echo "📋 构建结果:"
ls -la "$BUILD_DIR" | grep -E "(client|triton)"

# 创建运行脚本
cat > "$BUILD_DIR/run_minimal_client.sh" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
./minimal_client "$@"
EOF

if [ -f "$BUILD_DIR/simple_triton_client" ]; then
    cat > "$BUILD_DIR/run_simple_client.sh" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
./simple_triton_client "$@"
EOF
fi

if [ -f "$BUILD_DIR/triton_client" ]; then
    cat > "$BUILD_DIR/run_client.sh" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
./triton_client "$@"
EOF
fi

# 添加执行权限
chmod +x "$BUILD_DIR"/*.sh 2>/dev/null || true

echo ""
echo "🎉 构建脚本执行完成!"
echo ""
echo "📍 可执行文件位置: $BUILD_DIR"
echo ""
echo "🚀 运行方式："
if [ -f "$BUILD_DIR/minimal_client" ]; then
    echo "  最小客户端: $BUILD_DIR/run_minimal_client.sh"
fi
if [ -f "$BUILD_DIR/simple_triton_client" ]; then
    echo "  简单客户端: $BUILD_DIR/run_simple_client.sh"
fi
if [ -f "$BUILD_DIR/triton_client" ]; then
    echo "  完整客户端: $BUILD_DIR/run_client.sh [选项]"
fi
echo ""
echo "💡 确保 Triton 服务器在 localhost:8000 上运行"
