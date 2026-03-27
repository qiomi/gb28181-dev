/**
 * @file simple_push.cpp
 * @brief 简单的视频推送示例
 * 
 * 演示如何使用 GB28181 SDK 推送 H.264 视频流到平台
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <chrono>
#include <thread>

#include "gb28181_dev.h"

// 全局设备句柄
static gb28181_dev_device_handle_t g_device = nullptr;
static volatile bool g_running = true;

// 信号处理函数
void signal_handler(int sig) {
  printf("\nReceived signal %d, stopping...\n", sig);
  g_running = false;
  
  if (g_device) {
    gb28181_dev_stop(g_device);
  }
}

// 模拟读取 H.264 帧（实际应用中应从编码器或文件读取）
bool read_h264_frame(uint8_t** data, int* len) {
  // 这里仅作为示例，实际应该从摄像头或文件读取
  static uint8_t fake_frame[1024];
  static int frame_count = 0;
  
  // 模拟一个 H.264 NALU（以 0x00000001 起始码开头）
  memset(fake_frame, 0, 4);  // 起始码
  fake_frame[4] = 0x65;      // IDR 帧标识
  for (int i = 5; i < 1024; i++) {
    fake_frame[i] = (frame_count + i) & 0xFF;
  }
  
  *data = fake_frame;
  *len = 1024;
  frame_count++;
  
  return true;
}

// 获取当前时间戳（微秒）
uint64_t get_timestamp_us() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

int main(int argc, char* argv[]) {
  // 设置信号处理
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  
  printf("GB28181 Simple Push Example\n");
  printf("=========================\n\n");
  
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
  stream_config.mode = STREAM_MODE_RAW;  // 源码流模式，自动 PS 封装
  stream_config.channel_id = 1;
  stream_config.ssrc = 0;  // 自动生成 SSRC
  
  config.stream_count = 1;
  config.streams = &stream_config;
  
  // 创建设备
  printf("Creating device...\n");
  g_device = gb28181_dev_create(&config);
  if (!g_device) {
    fprintf(stderr, "Failed to create device\n");
    return -1;
  }
  
  // 启动设备
  printf("Starting device...\n");
  int ret = gb28181_dev_start(g_device);
  if (ret < 0) {
    fprintf(stderr, "Failed to start device: %d\n", ret);
    gb28181_dev_destroy(g_device);
    return -1;
  }
  
  printf("Device started successfully\n");
  printf("Press Ctrl+C to stop\n\n");
  
  // 主循环：读取视频并发送
  int frame_count = 0;
  while (g_running) {
    uint8_t* frame_data = nullptr;
    int frame_len = 0;
    
    // 读取一帧 H.264 数据
    if (!read_h264_frame(&frame_data, &frame_len)) {
      fprintf(stderr, "Failed to read frame\n");
      break;
    }
    
    // 获取当前时间戳
    uint64_t pts = get_timestamp_us();
    
    // 发送视频数据
    ret = gb28181_dev_send_video(g_device, 1, frame_data, frame_len, pts);
    if (ret < 0) {
      fprintf(stderr, "Failed to send video: %d\n", ret);
      break;
    }
    
    frame_count++;
    if (frame_count % 30 == 0) {
      printf("Sent %d frames\n", frame_count);
    }
    
    // 模拟 25fps 的帧率
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
  }
  
  // 清理
  printf("\nStopping device...\n");
  gb28181_dev_stop(g_device);
  
  printf("Destroying device...\n");
  gb28181_dev_destroy(g_device);
  
  printf("Total frames sent: %d\n", frame_count);
  printf("Done.\n");
  
  return 0;
}
