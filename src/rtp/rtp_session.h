/**
 * @file rtp_session.h
 * @brief RTP 会话管理
 */

#ifndef GB28181_DEV_RTP_SESSION_H_
#define GB28181_DEV_RTP_SESSION_H_

#include <stdint.h>

#include <memory>
#include <string>

namespace gb28181 {

/**
 * @class RtpSession
 * @brief RTP 会话类
 *
 * 负责处理 RTP 包的打包和发送
 */
class RtpSession {
public:
    RtpSession();
    ~RtpSession();

    /**
     * @brief 初始化 RTP 会话
     * @param local_port 本地端口
     * @param remote_ip 远端 IP
     * @param remote_port 远端端口
     * @param ssrc SSRC
     * @return 0 表示成功，负数表示失败
     */
    int Init(uint16_t local_port, const std::string& remote_ip,
             uint16_t remote_port, uint32_t ssrc);

    /**
     * @brief 启动 RTP 会话
     * @return 0 表示成功，负数表示失败
     */
    int Start();

    /**
     * @brief 停止 RTP 会话
     */
    void Stop();

    /**
     * @brief 发送 H.264 NALU
     * @param nalu_data NALU 数据
     * @param nalu_len NALU 长度
     * @param pts 时间戳（微秒）
     * @param is_key_frame 是否关键帧
     * @return 0 表示成功，负数表示失败
     */
    int SendH264Nalu(const uint8_t* nalu_data, int nalu_len, uint64_t pts,
                     bool is_key_frame);

    /**
     * @brief 发送 H.265 NALU
     * @param nalu_data NALU 数据
     * @param nalu_len NALU 长度
     * @param pts 时间戳（微秒）
     * @param is_key_frame 是否关键帧
     * @return 0 表示成功，负数表示失败
     */
    int SendH265Nalu(const uint8_t* nalu_data, int nalu_len, uint64_t pts,
                     bool is_key_frame);

    /**
     * @brief 发送 PS 包
     * @param ps_data PS 数据
     * @param ps_len PS 长度
     * @param pts 时间戳（微秒）
     * @return 0 表示成功，负数表示失败
     */
    int SendPsPacket(const uint8_t* ps_data, int ps_len, uint64_t pts);

    /**
     * @brief 获取本地 RTP 端口
     * @return 本地 RTP 端口
     */
    uint16_t GetLocalPort() const;

    /**
     * @brief 获取 SSRC
     * @return SSRC
     */
    uint32_t GetSsrc() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace gb28181

#endif  // GB28181_DEV_RTP_SESSION_H_
