/**
 * @file device_impl.cpp
 * @brief 设备核心实现类实现
 */

#include "device_impl.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "../media/media_processor.h"
#include "../rtp/rtp_session.h"
#include "../sip/sip_stack.h"
#include "../utils/utils.h"

namespace gb28181 {

DeviceImpl::DeviceImpl(const gb28181_dev_config_t& config)
    : platform_ip_(config.platform_ip),
      platform_port_(config.platform_port),
      device_id_(config.device_id),
      server_id_(config.server_id),
      username_(config.username),
      password_(config.password),
      local_sip_port_(config.local_sip_port),
      rtp_port_base_(config.rtp_port_base),
      running_(false),
      registered_(false) {
    // 初始化流上下文
    for (int i = 0; i < config.stream_count; ++i) {
        StreamContext ctx;
        ctx.channel_id = config.streams[i].channel_id;
        ctx.ssrc = config.streams[i].ssrc;
        if (ctx.ssrc == 0) {
            ctx.ssrc = utils::GenerateSsrc();
        }
        ctx.seq_num = 0;
        ctx.codec = config.streams[i].codec;
        ctx.mode = config.streams[i].mode;
        streams_.push_back(std::move(ctx));
    }

    LOG_INFO("Device created: %s", device_id_.c_str());
}

DeviceImpl::~DeviceImpl() {
    Stop();
    LOG_INFO("Device destroyed: %s", device_id_.c_str());
}

int DeviceImpl::Start() {
    if (running_) {
        return 0;
    }

    try {
        // 初始化 SIP 栈
        int ret = InitSipStack();
        if (ret < 0) {
            LOG_ERROR("Failed to init SIP stack");
            return ret;
        }

        // 启动 SIP 栈
        ret = sip_stack_->Start();
        if (ret < 0) {
            LOG_ERROR("Failed to start SIP stack");
            return ret;
        }

        // 注册到平台
        ret = RegisterToPlatform();
        if (ret < 0) {
            LOG_ERROR("Failed to register to platform");
            return ret;
        }

        // 初始化 RTP 会话
        ret = InitRtpSessions();
        if (ret < 0) {
            LOG_ERROR("Failed to init RTP sessions");
            return ret;
        }

        // 启动心跳线程
        running_ = true;
        heartbeat_thread_ = std::thread(&DeviceImpl::HeartbeatThread, this);

        LOG_INFO("Device started successfully");
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in Start: %s", e.what());
        return -1;
    }
}

void DeviceImpl::Stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // 停止心跳线程
    if (heartbeat_thread_.joinable()) {
        heartbeat_thread_.join();
    }

    // 取消注册
    if (registered_ && sip_stack_) {
        sip_stack_->Unregister();
    }

    // 停止 RTP 会话
    for (auto& stream : streams_) {
        if (stream.rtp_session) {
            stream.rtp_session->Stop();
        }
    }

    // 停止 SIP 栈
    if (sip_stack_) {
        sip_stack_->Stop();
    }

    LOG_INFO("Device stopped");
}

int DeviceImpl::SendVideo(int channel_id, const uint8_t* data, int len,
                          uint64_t pts) {
    if (!running_) {
        return -1;
    }

    // 查找对应的流
    auto it = std::find_if(streams_.begin(), streams_.end(),
                           [channel_id](const StreamContext& ctx) {
                               return ctx.channel_id == channel_id;
                           });

    if (it == streams_.end()) {
        LOG_ERROR("Channel %d not found", channel_id);
        return -1;
    }

    try {
        MediaProcessor processor;
        processor.Init(it->codec);

        std::vector<MediaFrame> frames;
        processor.ProcessRawStream(data, len, frames);

        for (const auto& frame : frames) {
            if (it->mode == STREAM_MODE_RAW) {
                // 源码流模式，自动 PS 封装并发送
                std::vector<uint8_t> ps_packet;
                if (it->codec == VIDEO_CODEC_H264) {
                    processor.PsPackH264(frame.data.data(), frame.data.size(),
                                         frame.pts, ps_packet);
                } else {
                    processor.PsPackH265(frame.data.data(), frame.data.size(),
                                         frame.pts, ps_packet);
                }

                if (it->rtp_session) {
                    it->rtp_session->SendPsPacket(ps_packet.data(),
                                                  ps_packet.size(), frame.pts);
                }
            } else {
                // PS 流模式，直接发送
                if (it->rtp_session) {
                    it->rtp_session->SendPsPacket(data, len, pts);
                }
            }
        }

        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in SendVideo: %s", e.what());
        return -1;
    }
}

int DeviceImpl::SendAudio(int /*channel_id*/, const uint8_t* /*data*/,
                          int /*len*/, uint64_t /*pts*/) {
    // TODO: 实现音频发送
    LOG_WARN("Audio sending is not implemented yet");
    return -1;
}

int DeviceImpl::InitSipStack() {
    sip_stack_ = std::make_unique<SipStack>();
    return sip_stack_->Init(local_sip_port_);
}

int DeviceImpl::InitRtpSessions() {
    // TODO: 根据实际平台地址初始化 RTP 会话
    // 这里暂时不创建，等待收到 INVITE 后再创建
    return 0;
}

int DeviceImpl::RegisterToPlatform() {
    if (!sip_stack_) {
        return -1;
    }

    int ret = sip_stack_->Register(platform_ip_, platform_port_, device_id_,
                                   server_id_, username_, password_);
    if (ret == 0) {
        registered_ = true;
        LOG_INFO("Registered to platform %s:%d", platform_ip_.c_str(),
                 platform_port_);
    }
    return ret;
}

void DeviceImpl::HeartbeatThread() {
    // 心跳间隔（秒）
    const int heartbeat_interval = 60;

    while (running_) {
        // 等待心跳间隔
        for (int i = 0; i < heartbeat_interval && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (!running_) {
            break;
        }

        // 发送心跳
        if (registered_ && sip_stack_) {
            int ret = sip_stack_->Keepalive(device_id_, server_id_);
            if (ret < 0) {
                LOG_WARN("Heartbeat failed, trying to re-register...");
                registered_ = false;
                RegisterToPlatform();
            } else {
                LOG_DEBUG("Heartbeat sent");
            }
        } else if (!registered_) {
            // 尝试重新注册
            RegisterToPlatform();
        }
    }
}

}  // namespace gb28181
