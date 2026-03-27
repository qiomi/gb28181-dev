/**
 * @file sip_stack.h
 * @brief SIP 协议栈接口定义
 */

#ifndef GB28181_DEV_SIP_STACK_H_
#define GB28181_DEV_SIP_STACK_H_

#include <functional>
#include <memory>
#include <string>

namespace gb28181 {

/**
 * @class SipStack
 * @brief SIP 协议栈实现类
 *
 * 负责处理 SIP 注册、心跳、INVITE/ACK/BYE 等流程
 */
class SipStack {
public:
    SipStack();
    ~SipStack();

    /**
     * @brief 初始化 SIP 栈
     * @param local_port 本地 SIP 端口
     * @return 0 表示成功，负数表示失败
     */
    int Init(int local_port);

    /**
     * @brief 启动 SIP 栈
     * @return 0 表示成功，负数表示失败
     */
    int Start();

    /**
     * @brief 停止 SIP 栈
     */
    void Stop();

    /**
     * @brief 注册到平台
     * @param platform_ip 平台 IP
     * @param platform_port 平台端口
     * @param device_id 设备 ID
     * @param server_id 服务器 ID
     * @param username 用户名
     * @param password 密码
     * @param expires 过期时间（秒）
     * @return 0 表示成功，负数表示失败
     */
    int Register(const std::string& platform_ip, int platform_port,
                 const std::string& device_id, const std::string& server_id,
                 const std::string& username, const std::string& password,
                 int expires = 3600);

    /**
     * @brief 取消注册
     * @return 0 表示成功，负数表示失败
     */
    int Unregister();

    /**
     * @brief 发送心跳
     * @param device_id 设备 ID
     * @param server_id 服务器 ID
     * @return 0 表示成功，负数表示失败
     */
    int Keepalive(const std::string& device_id, const std::string& server_id);

    /**
     * @brief 响应 INVITE
     * @param call_id 呼叫 ID
     * @param ssrc SSRC
     * @param rtp_port RTP 端口
     * @return 0 表示成功，负数表示失败
     */
    int RespondInvite(const std::string& call_id, uint32_t ssrc, int rtp_port);

    /**
     * @brief 结束呼叫
     * @param call_id 呼叫 ID
     * @return 0 表示成功，负数表示失败
     */
    int Bye(const std::string& call_id);

    /**
     * @brief 设置收到 INVITE 的回调
     * @param callback 回调函数
     */
    void SetInviteCallback(
        std::function<void(const std::string& call_id, int channel)> callback);

    /**
     * @brief 设置收到 BYE 的回调
     * @param callback 回调函数
     */
    void SetByeCallback(
        std::function<void(const std::string& call_id)> callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace gb28181

#endif  // GB28181_DEV_SIP_STACK_H_
