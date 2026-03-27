/**
 * @file sip_stack.cpp
 * @brief SIP 协议栈实现（框架）
 */

#include "sip_stack.h"
#include "../utils/utils.h"
#include <cstdio>

namespace gb28181 {

class SipStack::Impl {
 public:
  bool initialized_ = false;
  bool running_ = false;
  int local_port_ = 0;

  std::function<void(const std::string& call_id, int channel)> invite_callback_;
  std::function<void(const std::string& call_id)> bye_callback_;
};

SipStack::SipStack() : impl_(std::make_unique<Impl>()) {}

SipStack::~SipStack() {
  Stop();
}

int SipStack::Init(int local_port) {
  if (impl_->initialized_) {
    return 0;
  }

  impl_->local_port_ = local_port;
  impl_->initialized_ = true;

  LOG_INFO("SIP stack initialized on port %d", local_port);
  return 0;
}

int SipStack::Start() {
  if (!impl_->initialized_) {
    return -1;
  }

  if (impl_->running_) {
    return 0;
  }

  impl_->running_ = true;
  LOG_INFO("SIP stack started");
  return 0;
}

void SipStack::Stop() {
  if (!impl_->running_) {
    return;
  }

  impl_->running_ = false;
  LOG_INFO("SIP stack stopped");
}

int SipStack::Register(const std::string& platform_ip, int platform_port,
                       const std::string& device_id,
                       const std::string& server_id,
                       const std::string& username,
                       const std::string& password, int expires) {
  if (!impl_->running_) {
    return -1;
  }

  // TODO: 实现 SIP REGISTER 消息发送
  LOG_INFO("Sending REGISTER to %s:%d for device %s", platform_ip.c_str(),
           platform_port, device_id.c_str());

  // 模拟注册成功
  return 0;
}

int SipStack::Unregister() {
  if (!impl_->running_) {
    return -1;
  }

  // TODO: 实现 SIP unregister 逻辑
  LOG_INFO("Unregistering from platform");
  return 0;
}

int SipStack::Keepalive(const std::string& device_id,
                        const std::string& server_id) {
  if (!impl_->running_) {
    return -1;
  }

  // TODO: 实现 SIP MESSAGE 心跳消息发送
  LOG_DEBUG("Sending heartbeat for device %s", device_id.c_str());
  return 0;
}

int SipStack::RespondInvite(const std::string& call_id, uint32_t ssrc,
                            int rtp_port) {
  if (!impl_->running_) {
    return -1;
  }

  // TODO: 实现 SIP 200 OK 响应 INVITE
  LOG_INFO("Responding to INVITE %s with SSRC %u, RTP port %d", call_id.c_str(),
           ssrc, rtp_port);
  return 0;
}

int SipStack::Bye(const std::string& call_id) {
  if (!impl_->running_) {
    return -1;
  }

  // TODO: 实现 SIP BYE 消息发送
  LOG_INFO("Sending BYE for call %s", call_id.c_str());
  return 0;
}

void SipStack::SetInviteCallback(
    std::function<void(const std::string& call_id, int channel)> callback) {
  impl_->invite_callback_ = callback;
}

void SipStack::SetByeCallback(
    std::function<void(const std::string& call_id)> callback) {
  impl_->bye_callback_ = callback;
}

}  // namespace gb28181
