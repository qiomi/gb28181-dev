# GB28181-Dev

轻量级 GB28181 设备端（客户端）SDK，使用 C++ 实现，提供 C 语言接口。

## 特性

- ✅ **GB28181 设备端（客户端）实现**
- ✅ **SIP 注册、心跳保活、INVITE/ACK/BYE 流程**
- ✅ **支持输入媒体流**：
  - 裸 H.264
  - 裸 H.265
  - PS 封装码流
- ✅ **国标 RTP 打包发送**
- ✅ **极简 C++ 接口，易于集成**
- ✅ **跨平台**：Linux / x86 / ARM（海思、瑞芯微等）
- ✅ **无商业依赖，开源可商用**

## 项目结构

```
gb28181-dev/
├── include/                  # 头文件
│   └── gb28181_dev.h        # 对外接口定义
├── src/                      # 源代码
│   ├── core/                # 核心实现
│   ├── sip/                 # SIP 协议栈
│   ├── rtp/                 # RTP 打包
│   ├── media/               # 媒体处理
│   └── utils/               # 工具函数
├── examples/                 # 示例程序
├── tests/                    # 单元测试
├── docs/                     # 文档
├── scripts/                  # 构建脚本
├── CMakeLists.txt           # CMake 配置
├── .clang-format            # Clang 格式化配置
└── README.md                # 项目说明
```

## 快速开始

### 依赖

- CMake >= 3.14
- C++17 编译器
- pthread

### 编译

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 生成 compile_commands.json

CMake 会自动生成 `compile_commands.json` 文件用于 clangd 支持：

```bash
mkdir build && cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### 安装

```bash
sudo make install
```

### 使用示例

```cpp
#include "gb28181_dev.h"

int main() {
  // 配置设备参数
  gb28181_dev_config_t config = {0};
  snprintf(config.platform_ip, sizeof(config.platform_ip), "192.168.1.100");
  config.platform_port = 5060;
  snprintf(config.device_id, sizeof(config.device_id), "34020000001320000001");
  snprintf(config.server_id, sizeof(config.server_id), "3402000000");
  snprintf(config.username, sizeof(config.username), "34020000001320000001");
  snprintf(config.password, sizeof(config.password), "12345678");
  config.local_sip_port = 5060;
  config.rtp_port_base = 10000;
  
  // 配置视频流
  gb28181_dev_stream_config_t stream_config;
  stream_config.codec = VIDEO_CODEC_H264;
  stream_config.mode = STREAM_MODE_RAW;
  stream_config.channel_id = 1;
  stream_config.ssrc = 0;  // 自动生成
  
  config.stream_count = 1;
  config.streams = &stream_config;
  
  // 创建设备
  gb28181_dev_device_handle_t handle = gb28181_dev_create(&config);
  if (!handle) {
    return -1;
  }
  
  // 启动设备
  gb28181_dev_start(handle);
  
  // 发送视频数据
  uint8_t* h264_data = ...;  // H.264 数据
  int data_len = ...;
  uint64_t pts = get_current_timestamp_us();
  gb28181_dev_send_video(handle, 1, h264_data, data_len, pts);
  
  // 停止并销毁
  gb28181_dev_stop(handle);
  gb28181_dev_destroy(handle);
  
  return 0;
}
```

## API 文档

详细 API 文档请参考 [`include/gb28181_dev.h`](include/gb28181_dev.h)

### 核心接口

- `gb28181_dev_create()` - 创建并初始化设备实例
- `gb28181_dev_start()` - 启动设备运行
- `gb28181_dev_send_video()` - 发送视频数据
- `gb28181_dev_send_audio()` - 发送音频数据
- `gb28181_dev_stop()` - 停止设备运行
- `gb28181_dev_destroy()` - 销毁设备实例

## 代码规范

本项目采用 Google C++ Style 代码规范：

- 缩进：4 空格
- 大括号换行
- 使用 `clang-format` 自动格式化代码

格式化代码：

```bash
find src include -name '*.h' -o -name '*.cpp' | xargs clang-format -i
```

## 构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| GB28181_BUILD_EXAMPLES | ON | 是否编译示例程序 |
| GB28181_BUILD_TESTS | ON | 是否编译测试程序 |
| GB28181_BUILD_SHARED_LIBS | ON | 是否编译动态库 |

## 跨平台支持

- **Linux**: x86/x86_64
- **ARM**: 海思、瑞芯微等嵌入式平台
- **其他类 Unix 系统**

## 许可证

MIT License - 开源可商用

## 常见问题

### Q: 支持音频吗？

A: 当前版本主要支持视频，音频接口已预留，后续会添加支持。

### Q: 如何调试？

A: 使用 Debug 模式编译，查看详细日志输出。

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## 贡献

欢迎提交 Issue 和 Pull Request！

## 联系方式

- GitHub: https://github.com/qiomi/gb28181-dev
- Email: [你的邮箱]
