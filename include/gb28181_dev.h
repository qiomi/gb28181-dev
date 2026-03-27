#ifndef GB28181_DEV_H_
#define GB28181_DEV_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file gb28181_dev.h
 * @brief Tiny GB28181 Device SDK 对外接口定义
 * 
 * 轻量级 GB28181 设备端（客户端）实现
 * - SIP 注册、心跳保活、INVITE/ACK/BYE 流程
 * - 支持输入媒体流：裸 H.264、裸 H.265、PS 封装码流
 * - 国标 RTP 打包发送
 * - 极简 C++ 接口，易于集成
 * - 跨平台：Linux / x86 / ARM（海思、瑞芯微等）
 * - 无商业依赖，开源可商用
 */

// ============================================================================
// 基础类型定义
// ============================================================================

/**
 * @enum gb28181_dev_video_codec_t
 * @brief 视频编码格式
 */
typedef enum {
  VIDEO_CODEC_H264 = 0, ///< H.264/AVC 编码
  VIDEO_CODEC_H265 = 1  ///< H.265/HEVC 编码
} gb28181_dev_video_codec_t;

/**
 * @enum gb28181_dev_stream_mode_t
 * @brief 流模式类型
 */
typedef enum {
  STREAM_MODE_RAW = 0, ///< 源码流模式（自动 PS 封装）
  STREAM_MODE_PS = 1   ///< PS 封装流模式（直接发送已封装数据）
} gb28181_dev_stream_mode_t;

/**
 * @struct gb28181_dev_stream_config_t
 * @brief 单路流配置
 */
typedef struct {
  gb28181_dev_video_codec_t codec;  ///< 编码格式：H.264 或 H.265
  gb28181_dev_stream_mode_t mode;   ///< 流模式：源码流或 PS 流
  int channel_id;                   ///< 通道 ID
  uint32_t ssrc;                    ///< RTP SSRC（0 表示自动生成）
} gb28181_dev_stream_config_t;

/**
 * @struct gb28181_dev_config_t
 * @brief 设备配置参数
 */
typedef struct {
  char platform_ip[64];                 ///< 平台 SIP 服务器 IP
  int platform_port;                    ///< 平台 SIP 端口
  char device_id[64];                   ///< 设备 ID
  char server_id[64];                   ///< 平台 ID
  char username[64];                    ///< SIP 用户名
  char password[64];                    ///< SIP 密码
  int local_sip_port;                   ///< 本地 SIP 端口
  int rtp_port_base;                    ///< RTP 起始端口
  int stream_count;                     ///< 流数量
  gb28181_dev_stream_config_t* streams; ///< 流配置数组
} gb28181_dev_config_t;

// ============================================================================
// 设备句柄与核心接口
// ============================================================================

/**
 * @typedef gb28181_dev_device_handle_t
 * @brief 设备实例句柄
 */
typedef void* gb28181_dev_device_handle_t;

/**
 * @fn gb28181_dev_create
 * @brief 创建并初始化设备实例
 *
 * @param cfg 设备配置参数
 * @return 成功返回设备句柄，失败返回 NULL
 */
gb28181_dev_device_handle_t gb28181_dev_create(
    const gb28181_dev_config_t* cfg);

/**
 * @fn gb28181_dev_start
 * @brief 启动设备运行
 *
 * @param handle 设备句柄
 * @return 0 表示成功，负数表示失败
 */
int gb28181_dev_start(gb28181_dev_device_handle_t handle);

/**
 * @fn gb28181_dev_send_video
 * @brief 向指定通道发送视频数据
 *
 * @param handle 设备句柄
 * @param channel_id 通道 ID
 * @param data 视频数据指针
 * @param len 数据长度
 * @param pts 时间戳（微秒）
 * @return 0 表示成功，负数表示失败
 */
int gb28181_dev_send_video(gb28181_dev_device_handle_t handle, int channel_id,
                           const uint8_t* data, int len, uint64_t pts);

/**
 * @fn gb28181_dev_send_audio
 * @brief 向指定通道发送音频数据（预留接口）
 *
 * @param handle 设备句柄
 * @param channel_id 通道 ID
 * @param data 音频数据指针
 * @param len 数据长度
 * @param pts 时间栈（微秒）
 * @return 0 表示成功，负数表示失败
 */
int gb28181_dev_send_audio(gb28181_dev_device_handle_t handle, int channel_id,
                           const uint8_t* data, int len, uint64_t pts);

/**
 * @fn gb28181_dev_stop
 * @brief 停止设备运行
 *
 * @param handle 设备句柄
 */
void gb28181_dev_stop(gb28181_dev_device_handle_t handle);

/**
 * @fn gb28181_dev_destroy
 * @brief 销毁设备实例
 *
 * @param handle 设备句柄
 */
void gb28181_dev_destroy(gb28181_dev_device_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif  // GB28181_DEV_H_
