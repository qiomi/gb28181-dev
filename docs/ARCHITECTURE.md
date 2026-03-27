# GB28181-Dev 项目架构文档

## 目录结构

```
gb28181-dev/
├── include/                  # 公共头文件（对外接口）
│   └── gb28181_dev.h        # C 语言接口定义
├── src/                      # 源代码
│   ├── core/                # 核心实现
│   │   ├── device_impl.h    # 设备实现类
│   │   ├── device_impl.cpp  # 设备实现类
│   │   └── device.cpp       # C 接口封装
│   ├── sip/                 # SIP 协议栈
│   │   ├── sip_stack.h      # SIP 栈接口
│   │   └── sip_stack.cpp    # SIP 栈实现
│   ├── rtp/                 # RTP 处理
│   │   ├── rtp_session.h    # RTP 会话接口
│   │   └── rtp_session.cpp  # RTP 会话实现
│   ├── media/               # 媒体处理
│   │   ├── media_processor.h # 媒体处理器接口
│   │   └── media_processor.cpp # 媒体处理器实现
│   └── utils/               # 工具函数
│       ├── utils.h          # 工具函数声明
│       └── utils.cpp        # 工具函数实现
├── examples/                 # 示例程序
│   └── simple_push.cpp      # 简单推送示例
├── tests/                    # 单元测试
│   └── test_basic.cpp       # 基础测试
├── docs/                     # 文档
│   └── ARCHITECTURE.md      # 架构文档（本文件）
├── scripts/                  # 构建脚本
│   ├── build.sh             # 构建脚本
│   └── format.sh            # 格式化脚本
├── .github/                  # GitHub 配置
│   ├── ISSUE_TEMPLATE/      # Issue 模板
│   └── workflows/           # GitHub Actions
├── CMakeLists.txt           # CMake 配置
├── .clang-format            # Clang 格式配置
├── .gitignore               # Git 忽略文件
├── LICENSE                  # 许可证
├── README.md                # 项目说明
└── CONTRIBUTING.md          # 贡献指南
```

## 模块划分

### 1. 核心模块 (core)

**职责**: 提供设备的核心管理和控制功能

**主要类**:
- `DeviceImpl`: 设备实现类，管理所有子模块
- C 接口层: 对外提供的 C 语言 API

**依赖关系**:
```
DeviceImpl → SipStack
DeviceImpl → RtpSession (多个)
DeviceImpl → MediaProcessor
```

### 2. SIP 协议栈模块 (sip)

**职责**: 实现 GB28181 所需的 SIP 信令流程

**主要功能**:
- SIP REGISTER 注册
- SIP MESSAGE 心跳保活
- INVITE/ACK/BYE 呼叫流程
- 支持 TCP/UDP 传输

**主要类**:
- `SipStack`: SIP 协议栈实现

**状态机**:
```
初始状态 → 已初始化 → 运行中 → 已注册 → 心跳中
              ↓           ↓         ↓
           停止 ←───────┴─────────┘
```

### 3. RTP 处理模块 (rtp)

**职责**: 实现国标 RTP 包的打包和发送

**主要功能**:
- H.264 NALU 的 RTP 打包（支持 FU-A 分片）
- H.265 NALU 的 RTP 打包（支持 FU 分片）
- PS payload 的 RTP 封装
- SSRC、序列号管理

**主要类**:
- `RtpSession`: RTP 会话管理

**RTP 包结构**:
```
+--------------+--------------+------------------+
|  RTP Header  | PS Payload   |  NALU Data       |
|  (12 bytes)  | (variable)   |  (variable)      |
+--------------+--------------+------------------+
```

### 4. 媒体处理模块 (media)

**职责**: 处理输入的视频码流，进行 PS 封装

**主要功能**:
- H.264/H.265 NALU 提取
- PS 封装（符合 GB/T 28181-2016 附录 B）
- 关键帧检测
- 时间戳转换

**主要类**:
- `MediaProcessor`: 媒体处理器

**PS 封装流程**:
```
原始码流 → NALU 提取 → PS 封装 → RTP 打包 → 网络发送
```

### 5. 工具模块 (utils)

**职责**: 提供通用工具函数

**主要功能**:
- SSRC 生成
- 时间戳转换
- CRC32 计算
- 字节序转换
- 日志宏

## 数据流

### 视频发送流程

```
应用层调用 gb28181_dev_send_video()
         ↓
DeviceImpl::SendVideo()
         ↓
查找对应的 StreamContext
         ↓
MediaProcessor::ProcessRawStream() - 提取 NALU
         ↓
对于每个 NALU:
  if STREAM_MODE_RAW:
    MediaProcessor::PsPackH264/H265() - PS 封装
    RtpSession::SendPsPacket() - RTP 打包发送
  else (STREAM_MODE_PS):
    RtpSession::SendPsPacket() - 直接 RTP 打包发送
```

### SIP 信令流程

#### 注册流程
```
DeviceImpl::Start()
    ↓
InitSipStack()
    ↓
Start() - 启动 SIP 接收线程
    ↓
RegisterToPlatform() - 发送 REGISTER
    ↓
等待 200 OK
    ↓
registered_ = true
    ↓
HeartbeatThread() - 启动心跳线程
```

#### 心跳流程
```
HeartbeatThread() (每 60 秒)
    ↓
SipStack::Keepalive() - 发送 MESSAGE
    ↓
等待 200 OK
    ↓
失败则尝试重新注册
```

#### INVITE 流程（平台主动拉流）
```
收到 INVITE
    ↓
解析 SDP
    ↓
创建/配置 RtpSession
    ↓
RespondInvite() - 发送 200 OK
    ↓
开始发送媒体流
    ↓
收到 BYE
    ↓
停止发送
```

## 内存管理

### 对象生命周期

1. **DeviceImpl**: 由 C 接口创建和销毁
   - `gb28181_dev_create()` 创建
   - `gb28181_dev_destroy()` 销毁

2. **SipStack**: 由 DeviceImpl 管理
   - DeviceImpl 构造函数中创建
   - DeviceImpl 析构函数中销毁

3. **RtpSession**: 每个 StreamContext 一个
   - 在收到 INVITE 或启动时创建
   - 在 Stop() 时销毁

4. **MediaProcessor**: 临时创建
   - 在 SendVideo() 中按需创建
   - 函数结束时自动销毁

### 智能指针使用

- `std::unique_ptr`: 用于独占所有权
  - `SipStack` in `DeviceImpl`
  - `RtpSession` in `StreamContext`
  - `Impl` in all Pimpl classes

- `std::vector`: 用于动态数组
  - `streams_` in `DeviceImpl`

## 线程模型

### 主线程
- 应用层调用接口
- `gb28181_dev_start()`
- `gb28181_dev_send_video()`
- `gb28181_dev_stop()`

### SIP 接收线程（TODO）
- 监听 SIP 端口
- 接收和处理 SIP 消息
- 回调通知

### 心跳线程
- 定期发送心跳
- 监测注册状态
- 自动重注册

## 错误处理

### 返回值约定

- **0**: 成功
- **负数**: 失败（具体错误码待定义）

### 异常安全

- 所有公共接口使用 try-catch 包裹
- 捕获 `std::exception` 并返回错误码
- 记录详细错误日志

## 扩展性设计

### 支持的扩展点

1. **音频支持**: 预留 `gb28181_dev_send_audio()` 接口
2. **多编码器支持**: `MediaProcessor` 可扩展支持其他编码格式
3. **传输协议**: `SipStack` 和 `RtpSession` 可添加 TCP 支持
4. **加密传输**: 可扩展 SRTP 支持

### 平台适配层

未来可以添加平台适配层，支持不同操作系统：

```
include/platform/
├── linux/
├── windows/
└── rtos/
```

## 性能优化建议

1. **零拷贝**: 避免不必要的内存拷贝
2. **对象池**: 对频繁创建的对象使用对象池
3. **异步 IO**: 使用 epoll/kqueue 提高网络性能
4. **锁优化**: 减少锁竞争，使用无锁数据结构

## 调试技巧

1. **启用 Debug 模式**: `-DCMAKE_BUILD_TYPE=Debug`
2. **查看详细日志**: LOG_DEBUG 宏输出
3. **网络抓包**: Wireshark 分析 SIP/RTP
4. **内存检查**: valgrind 检测内存泄漏

## 未来规划

- [ ] 完整的 SIP 协议栈实现
- [ ] H.264/H.265 RTP 打包完整实现
- [ ] PS 封装完整实现
- [ ] 音频编解码支持（G.711/AAC）
- [ ] TCP 被动模式支持
- [ ] 双向语音对讲
- [ ] 云台控制（PTZ）
- [ ] 设备目录查询响应
