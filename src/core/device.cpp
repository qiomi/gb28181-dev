/**
 * @file device.cpp
 * @brief 设备核心接口实现（C 接口）
 */

#include "gb28181_dev.h"
#include "device_impl.h"
#include "../src/sip/sip_stack.h"
#include "../src/rtp/rtp_session.h"
#include <cstdio>
#include <cstring>

using namespace gb28181;

// C 接口实现
gb28181_dev_device_handle_t gb28181_dev_create(
    const gb28181_dev_config_t* cfg) {
  if (!cfg) {
    fprintf(stderr, "Invalid config\n");
    return nullptr;
  }

  try {
    DeviceImpl* device = new DeviceImpl(*cfg);
    return static_cast<gb28181_dev_device_handle_t>(device);
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to create device: %s\n", e.what());
    return nullptr;
  }
}

int gb28181_dev_start(gb28181_dev_device_handle_t handle) {
  if (!handle) {
    return -1;
  }

  try {
    DeviceImpl* device = static_cast<DeviceImpl*>(handle);
    return device->Start();
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to start device: %s\n", e.what());
    return -1;
  }
}

int gb28181_dev_send_video(gb28181_dev_device_handle_t handle, int channel_id,
                           const uint8_t* data, int len, uint64_t pts) {
  if (!handle || !data || len <= 0) {
    return -1;
  }

  try {
    DeviceImpl* device = static_cast<DeviceImpl*>(handle);
    return device->SendVideo(channel_id, data, len, pts);
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to send video: %s\n", e.what());
    return -1;
  }
}

int gb28181_dev_send_audio(gb28181_dev_device_handle_t handle, int channel_id,
                           const uint8_t* data, int len, uint64_t pts) {
  if (!handle || !data || len <= 0) {
    return -1;
  }

  try {
    DeviceImpl* device = static_cast<DeviceImpl*>(handle);
    return device->SendAudio(channel_id, data, len, pts);
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to send audio: %s\n", e.what());
    return -1;
  }
}

void gb28181_dev_stop(gb28181_dev_device_handle_t handle) {
  if (!handle) {
    return;
  }

  try {
    DeviceImpl* device = static_cast<DeviceImpl*>(handle);
    device->Stop();
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to stop device: %s\n", e.what());
  }
}

void gb28181_dev_destroy(gb28181_dev_device_handle_t handle) {
  if (!handle) {
    return;
  }

  try {
    // 先停止设备
    DeviceImpl* device = static_cast<DeviceImpl*>(handle);
    device->Stop();
    delete device;
  } catch (const std::exception& e) {
    fprintf(stderr, "Failed to destroy device: %s\n", e.what());
  }
}
