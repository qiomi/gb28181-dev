/**
 * @file device_impl.h
 * @brief 设备核心实现类
 */

#ifndef GB28181_DEV_DEVICE_IMPL_H_
#define GB28181_DEV_DEVICE_IMPL_H_

#include "gb28181_dev.h"
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

namespace gb28181 {

// 前向声明
class SipStack;
class RtpSession;
class MediaProcessor;

/**
 * @struct StreamContext
 * @brief 流上下文信息
 */
struct StreamContext {
  int channel_id;           // 通道 ID
  uint32_t ssrc;           // RTP SSRC
  uint16_t seq_num;        // RTP 序列号
  gb28181_dev_video_codec_t codec;  // 编码格式
  gb28181_dev_stream_mode_t mode;   // 流模式
  std::unique_ptr<RtpSession> rtp_session;  // RTP 会话

  // 默认构造函数
  StreamContext()
      : channel_id(0),
        ssrc(0),
        seq_num(0),
        codec(VIDEO_CODEC_H264),
        mode(STREAM_MODE_RAW) {}

  // 移动构造函数
  StreamContext(StreamContext&& other) noexcept = default;

  // 移动赋值运算符
  StreamContext& operator=(StreamContext&& other) noexcept = default;

  // 禁用拷贝
  StreamContext(const StreamContext&) = delete;
  StreamContext& operator=(const StreamContext&) = delete;
};

/**
 * @class DeviceImpl
 * @brief 设备实现类
 */
class DeviceImpl {
 public:
  explicit DeviceImpl(const gb28181_dev_config_t& config);
  ~DeviceImpl();

  /**
   * @brief 启动设备
   * @return 0 表示成功，负数表示失败
   */
  int Start();

  /**
   * @brief 停止设备
   */
  void Stop();

  /**
   * @brief 发送视频数据
   * @param channel_id 通道 ID
   * @param data 视频数据指针
   * @param len 数据长度
   * @param pts 时间戳（微秒）
   * @return 0 表示成功，负数表示失败
   */
  int SendVideo(int channel_id, const uint8_t* data, int len, uint64_t pts);

  /**
   * @brief 发送音频数据
   * @param channel_id 通道 ID
   * @param data 音频数据指针
   * @param len 数据长度
   * @param pts 时间戳（微秒）
   * @return 0 表示成功，负数表示失败
   */
  int SendAudio(int channel_id, const uint8_t* data, int len, uint64_t pts);

 private:
  /**
   * @brief 初始化 SIP 栈
   * @return 0 表示成功，负数表示失败
   */
  int InitSipStack();

  /**
   * @brief 初始化 RTP 会话
   * @return 0 表示成功，负数表示失败
   */
  int InitRtpSessions();

  /**
   * @brief 注册到平台
   * @return 0 表示成功，负数表示失败
   */
  int RegisterToPlatform();

  /**
   * @brief 心跳保活线程
   */
  void HeartbeatThread();

  // 配置信息
  std::string platform_ip_;
  int platform_port_;
  std::string device_id_;
  std::string server_id_;
  std::string username_;
  std::string password_;
  int local_sip_port_;
  int rtp_port_base_;

  // 流上下文
  std::vector<StreamContext> streams_;

  // SIP 栈
  std::unique_ptr<SipStack> sip_stack_;

  // 运行状态
  std::atomic<bool> running_;
  std::atomic<bool> registered_;

  // 心跳线程
  std::thread heartbeat_thread_;

  // 互斥锁
  std::mutex mutex_;
};

}  // namespace gb28181

#endif  // GB28181_DEV_DEVICE_IMPL_H_
