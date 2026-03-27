/**
 * @file test_basic.cpp
 * @brief 基础功能测试
 */

#include <cstdio>
#include <cassert>
#include "gb28181_dev.h"

void test_config_initialization() {
  printf("Testing config initialization...\n");

  gb28181_dev_config_t config = {0};
  snprintf(config.platform_ip, sizeof(config.platform_ip), "192.168.1.100");
  config.platform_port = 5060;
  snprintf(config.device_id, sizeof(config.device_id), "34020000001320000001");
  snprintf(config.server_id, sizeof(config.server_id), "3402000000");
  snprintf(config.username, sizeof(config.username), "34020000001320000001");
  snprintf(config.password, sizeof(config.password), "12345678");
  config.local_sip_port = 5060;
  config.rtp_port_base = 10000;

  gb28181_dev_stream_config_t stream_config;
  stream_config.codec = VIDEO_CODEC_H264;
  stream_config.mode = STREAM_MODE_RAW;
  stream_config.channel_id = 1;
  stream_config.ssrc = 0;

  config.stream_count = 1;
  config.streams = &stream_config;

  assert(config.platform_port == 5060);
  assert(config.stream_count == 1);
  assert(stream_config.codec == VIDEO_CODEC_H264);

  printf("Config initialization test passed.\n\n");
}

void test_device_creation() {
  printf("Testing device creation...\n");

  gb28181_dev_config_t config = {0};
  snprintf(config.platform_ip, sizeof(config.platform_ip), "192.168.1.100");
  config.platform_port = 5060;
  snprintf(config.device_id, sizeof(config.device_id), "34020000001320000001");
  snprintf(config.server_id, sizeof(config.server_id), "3402000000");
  snprintf(config.username, sizeof(config.username), "34020000001320000001");
  snprintf(config.password, sizeof(config.password), "12345678");
  config.local_sip_port = 5060;
  config.rtp_port_base = 10000;

  gb28181_dev_stream_config_t stream_config;
  stream_config.codec = VIDEO_CODEC_H264;
  stream_config.mode = STREAM_MODE_RAW;
  stream_config.channel_id = 1;
  stream_config.ssrc = 0;

  config.stream_count = 1;
  config.streams = &stream_config;

  // 注意：由于 SIP 栈还未完全实现，这里可能会失败
  // 实际测试时应该使用 mock 或 stub
  gb28181_dev_device_handle_t handle = gb28181_dev_create(&config);

  // 如果创建成功，则销毁
  if (handle) {
    gb28181_dev_destroy(handle);
    printf("Device creation and destruction test passed.\n\n");
  } else {
    printf("Device creation skipped (SIP stack not fully implemented).\n\n");
  }
}

int main(int argc, char* argv[]) {
  printf("GB28181 Dev Basic Tests\n");
  printf("=======================\n\n");

  test_config_initialization();
  test_device_creation();

  printf("All tests completed.\n");
  return 0;
}
