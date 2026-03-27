# GB28181-Dev 快速开始指南

## 1. 环境准备

### 系统要求

- **操作系统**: Linux (x86/x86_64, ARM)
- **编译器**: g++ >= 7 或 clang++ >= 5 (支持 C++17)
- **构建工具**: CMake >= 3.14
- **依赖库**: pthread

### 安装依赖

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
```

**CentOS/RHEL:**
```bash
sudo yum install -y gcc-c++ cmake git
```

**ARM 平台（海思/瑞芯微）:**
使用交叉编译工具链，参考后文交叉编译章节。

## 2. 获取源码

```bash
git clone https://github.com/qiomi/gb28181-dev.git
cd gb28181-dev
```

## 3. 编译项目

### 简单编译

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 使用构建脚本（推荐）

```bash
# Release 模式构建
./scripts/build.sh

# Debug 模式构建
./scripts/build.sh -t Debug

# 清理并重新构建
./scripts/build.sh -c

# 构建并安装到系统目录
./scripts/build.sh -i
```

### 编译选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `-DCMAKE_BUILD_TYPE` | Release | 构建类型 (Debug/Release/RelWithDebInfo) |
| `-DCMAKE_EXPORT_COMPILE_COMMANDS` | ON | 生成 compile_commands.json |
| `-DGB28181_BUILD_EXAMPLES` | ON | 编译示例程序 |
| `-DGB28181_BUILD_TESTS` | ON | 编译测试程序 |
| `-DGB28181_BUILD_SHARED_LIBS` | ON | 编译动态库 |

### 自定义编译配置

```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DGB28181_BUILD_EXAMPLES=ON \
  -DGB28181_BUILD_TESTS=ON \
  -DGB28181_BUILD_SHARED_LIBS=OFF
make -j$(nproc)
```

## 4. 运行测试

```bash
cd build
ctest --output-on-failure
```

如果测试通过，会看到类似输出：
```
Test project /path/to/gb28181-dev/build
    Start 1: test_basic
1/1 Test #1: test_basic .......................   Passed    0.01 sec

100% tests passed!
```

## 5. 安装（可选）

### 安装到系统目录

```bash
cd build
sudo make install
```

安装后的文件结构：
```
/usr/local/include/gb28181_dev.h
/usr/local/lib/libgb28181-dev.so
/usr/local/lib/cmake/gb28181-dev/gb28181-dev-config.cmake
```

### 不安装直接使用

在构建目录中直接使用：
```bash
# 头文件路径：/path/to/gb28181-dev/include
# 库文件路径：/path/to/gb28181-dev/build/libgb28181-dev.so
```

## 6. 使用示例

### 最简单的例子

创建一个 `test_push.cpp` 文件：

```cpp
#include <cstdio>
#include <cstring>
#include "gb28181_dev.h"

int main() {
  // 1. 配置设备参数
  gb28181_dev_config_t config = {0};
  snprintf(config.platform_ip, sizeof(config.platform_ip), "192.168.1.100");
  config.platform_port = 5060;
  snprintf(config.device_id, sizeof(config.device_id), "34020000001320000001");
  snprintf(config.server_id, sizeof(config.server_id), "3402000000");
  snprintf(config.username, sizeof(config.username), "34020000001320000001");
  snprintf(config.password, sizeof(config.password), "12345678");
  config.local_sip_port = 5060;
  config.rtp_port_base = 10000;
  
  // 2. 配置视频流
  gb28181_dev_stream_config_t stream_config;
  stream_config.codec = VIDEO_CODEC_H264;
  stream_config.mode = STREAM_MODE_RAW;
  stream_config.channel_id = 1;
  stream_config.ssrc = 0;  // 自动生成
  
  config.stream_count = 1;
  config.streams = &stream_config;
  
  // 3. 创建设备
  gb28181_dev_device_handle_t handle = gb28181_dev_create(&config);
  if (!handle) {
    fprintf(stderr, "Failed to create device\n");
    return -1;
  }
  
  // 4. 启动设备（会自动注册到平台）
  int ret = gb28181_dev_start(handle);
  if (ret < 0) {
    fprintf(stderr, "Failed to start device\n");
    gb28181_dev_destroy(handle);
    return -1;
  }
  
  printf("Device started successfully\n");
  
  // 5. 发送视频数据
  // 这里应该从摄像头或编码器获取 H.264 数据
  uint8_t h264_data[1024] = {0x00, 0x00, 0x00, 0x01, 0x65};  // 模拟 IDR 帧
  uint64_t pts = 0;  // 时间戳（微秒）
  
  ret = gb28181_dev_send_video(handle, 1, h264_data, sizeof(h264_data), pts);
  if (ret < 0) {
    fprintf(stderr, "Failed to send video\n");
  }
  
  // 6. 停止并销毁设备
  gb28181_dev_stop(handle);
  gb28181_dev_destroy(handle);
  
  printf("Done.\n");
  return 0;
}
```

### 编译你的程序

**方法 1: 手动编译**
```bash
g++ -o test_push test_push.cpp \
  -I/path/to/gb28181-dev/include \
  -L/path/to/gb28181-dev/build \
  -lgb28181-dev \
  -lpthread \
  -Wl,-rpath,/path/to/gb28181-dev/build
```

**方法 2: 使用 CMake**

创建 `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.14)
project(my_gb28181_app)

set(CMAKE_CXX_STANDARD 17)

find_package(gb28181-dev REQUIRED)

add_executable(test_push test_push.cpp)
target_link_libraries(test_push gb28181-dev::gb28181-dev)
```

然后编译：
```bash
mkdir build && cd build
cmake .. -Dgb28181-dev_DIR=/path/to/gb28181-dev/build
make
```

## 7. 运行示例程序

项目自带示例程序：

```bash
cd build
./simple_push
```

示例程序会模拟推送 H.264 视频流到平台。

## 8. 交叉编译（ARM 平台）

### 海思平台示例

1. **设置交叉编译工具链**

```bash
export CROSS_COMPILE=arm-hisiv500-linux-gnueabi-
export CC=${CROSS_COMPILE}gcc
export CXX=${CROSS_COMPILE}g++
```

2. **创建工具链文件 `toolchain-hisi.cmake`**

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CROSS_COMPILE arm-hisiv500-linux-gnueabi-)
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

3. **编译**

```bash
mkdir build-hisi && cd build-hisi
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../toolchain-hisi.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j$(nproc)
```

### 瑞芯微平台示例

工具链文件 `toolchain-rockchip.cmake`:
```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CROSS_COMPILE arm-rockchip-linux-gnueabihf-)
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

## 9. 常见问题

### Q1: 编译时报错 "undefined reference to ..."

**解决**: 确保链接时添加了所有必需的库：
```bash
-lgb28181-dev -lpthread
```

### Q2: 运行时提示找不到共享库

**解决**: 
- 方法 1: 使用 `-Wl,-rpath` 指定库路径
- 方法 2: 设置 `LD_LIBRARY_PATH` 环境变量
```bash
export LD_LIBRARY_PATH=/path/to/gb28181-dev/build:$LD_LIBRARY_PATH
```
- 方法 3: 安装到系统目录 `sudo make install`

### Q3: 如何启用调试日志？

**解决**: 使用 Debug 模式编译：
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Q4: compile_commands.json 在哪里？

**解决**: CMake 会自动生成在项目根目录或 build 目录：
```bash
ln -sf build/compile_commands.json .
```

### Q5: 代码格式不符合规范？

**解决**: 使用格式化脚本：
```bash
./scripts/format.sh
```

## 10. 下一步

- 阅读 [ARCHITECTURE.md](docs/ARCHITECTURE.md) 了解项目架构
- 查看 [`include/gb28181_dev.h`](include/gb28181_dev.h) 了解完整 API
- 参考示例程序 [`examples/simple_push.cpp`](examples/simple_push.cpp)
- 阅读源代码了解实现细节

## 11. 获取帮助

- GitHub Issues: https://github.com/qiomi/gb28181-dev/issues
- 贡献指南: [CONTRIBUTING.md](CONTRIBUTING.md)

祝你使用愉快！🎉
