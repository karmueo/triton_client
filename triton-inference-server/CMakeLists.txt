cmake_minimum_required(VERSION 3.18)
project(TritonClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找必要的包
find_package(PkgConfig REQUIRED)

# 设置Triton客户端库路径
set(TRITON_CLIENT_BACKEND_INSTALL_DIR "/workspace/install" CACHE PATH "Path to Triton client libraries")
set(TRITON_CLIENT_REPO_TAG "r24.10" CACHE STRING "Triton client repository tag")

message(STATUS "使用预安装的Triton客户端库: ${TRITON_CLIENT_BACKEND_INSTALL_DIR}")
set(TRITON_CLIENT_INSTALL_DIR ${TRITON_CLIENT_BACKEND_INSTALL_DIR})
set(TRITON_CLIENT_LIBRARIES 
    ${TRITON_CLIENT_INSTALL_DIR}/lib/libtritonclient.so
)
set(TRITON_CLIENT_INCLUDE_DIRS ${TRITON_CLIENT_INSTALL_DIR}/include)

# 查找依赖库
find_package(Threads REQUIRED)

# 查找或构建依赖项
find_package(PkgConfig REQUIRED)

# libcurl
find_package(CURL REQUIRED)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 包含目录
include_directories(${TRITON_CLIENT_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

# 构建完整版客户端
add_executable(triton_client client.cpp)

# 构建简单版客户端
add_executable(simple_triton_client simple_client.cpp)

# 如果是从源码构建Triton客户端，添加依赖
if(TARGET triton-client)
    add_dependencies(triton_client triton-client)
    add_dependencies(simple_triton_client triton-client)
endif()

# 链接库
target_link_libraries(triton_client 
    ${TRITON_CLIENT_LIBRARIES}
    ${CURL_LIBRARIES}
    Threads::Threads
)

target_link_libraries(simple_triton_client 
    ${TRITON_CLIENT_LIBRARIES}
    ${CURL_LIBRARIES}
    Threads::Threads
)

# 运行时库路径
set_target_properties(triton_client PROPERTIES
    INSTALL_RPATH "${TRITON_CLIENT_INSTALL_DIR}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

set_target_properties(simple_triton_client PROPERTIES
    INSTALL_RPATH "${TRITON_CLIENT_INSTALL_DIR}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装目标
install(TARGETS triton_client simple_triton_client
    RUNTIME DESTINATION bin
)

# 创建一个脚本来设置环境变量
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/setup_env.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup_env.sh
    @ONLY
)

# 打印构建信息
message(STATUS "=== Triton客户端构建配置 ===")
message(STATUS "Triton客户端安装目录: ${TRITON_CLIENT_INSTALL_DIR}")
message(STATUS "Triton客户端包含目录: ${TRITON_CLIENT_INCLUDE_DIRS}")
message(STATUS "Triton客户端库: ${TRITON_CLIENT_LIBRARIES}")
message(STATUS "CURL库: ${CURL_LIBRARIES}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
