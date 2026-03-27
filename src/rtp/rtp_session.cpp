/**
 * @file rtp_session.cpp
 * @brief RTP 会话实现（框架）
 */

#include "rtp_session.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "../utils/utils.h"

namespace gb28181 {

class RtpSession::Impl {
public:
    bool initialized_ = false;
    bool running_ = false;
    uint16_t local_port_ = 0;
    std::string remote_ip_;
    uint16_t remote_port_ = 0;
    uint32_t ssrc_ = 0;
    uint16_t seq_num_ = 0;
    int socket_fd_ = -1;
};

RtpSession::RtpSession() : impl_(std::make_unique<Impl>()) {}

RtpSession::~RtpSession() { Stop(); }

int RtpSession::Init(uint16_t local_port, const std::string& remote_ip,
                     uint16_t remote_port, uint32_t ssrc) {
    if (impl_->initialized_) {
        return 0;
    }

    impl_->local_port_ = local_port;
    impl_->remote_ip_ = remote_ip;
    impl_->remote_port_ = remote_port;
    impl_->ssrc_ = ssrc;
    impl_->seq_num_ = 0;

    // TODO: 创建 UDP socket
    // impl_->socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);

    impl_->initialized_ = true;
    LOG_INFO("RTP session initialized: local=%u, remote=%s:%u, ssrc=%u",
             local_port, remote_ip.c_str(), remote_port, ssrc);
    return 0;
}

int RtpSession::Start() {
    if (!impl_->initialized_) {
        return -1;
    }

    if (impl_->running_) {
        return 0;
    }

    impl_->running_ = true;
    LOG_INFO("RTP session started");
    return 0;
}

void RtpSession::Stop() {
    if (!impl_->running_) {
        return;
    }

    impl_->running_ = false;

    // TODO: 关闭 socket
    if (impl_->socket_fd_ >= 0) {
        // close(impl_->socket_fd_);
        impl_->socket_fd_ = -1;
    }

    LOG_INFO("RTP session stopped");
}

int RtpSession::SendH264Nalu(const uint8_t* nalu_data, int nalu_len,
                             uint64_t pts, bool is_key_frame) {
    if (!impl_->running_ || !nalu_data || nalu_len <= 0) {
        return -1;
    }

    // TODO: 实现 H.264 NALU 的 RTP 打包和发送
    // 包括 FU-A 分片逻辑
    LOG_DEBUG("Sending H.264 NALU: len=%d, pts=%lu, key=%d", nalu_len, pts,
              is_key_frame);
    return 0;
}

int RtpSession::SendH265Nalu(const uint8_t* nalu_data, int nalu_len,
                             uint64_t pts, bool is_key_frame) {
    if (!impl_->running_ || !nalu_data || nalu_len <= 0) {
        return -1;
    }

    // TODO: 实现 H.265 NALU 的 RTP 打包和发送
    // 包括 FU 分片逻辑
    LOG_DEBUG("Sending H.265 NALU: len=%d, pts=%lu, key=%d", nalu_len, pts,
              is_key_frame);
    return 0;
}

int RtpSession::SendPsPacket(const uint8_t* ps_data, int ps_len, uint64_t pts) {
    if (!impl_->running_ || !ps_data || ps_len <= 0) {
        return -1;
    }

    // TODO: 实现 PS 包的 RTP 打包和发送
    // RTP + PS payload
    LOG_DEBUG("Sending PS packet: len=%d, pts=%lu", ps_len, pts);
    return 0;
}

uint16_t RtpSession::GetLocalPort() const { return impl_->local_port_; }

uint32_t RtpSession::GetSsrc() const { return impl_->ssrc_; }

}  // namespace gb28181
