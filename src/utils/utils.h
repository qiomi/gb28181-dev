/**
 * @file utils.h
 * @brief 工具函数集
 */

#ifndef GB28181_DEV_UTILS_H_
#define GB28181_DEV_UTILS_H_

#include <stdint.h>
#include <string>

namespace gb28181 {
namespace utils {

/**
 * @brief 生成随机 SSRC
 * @return 随机 SSRC 值
 */
uint32_t GenerateSsrc();

/**
 * @brief 获取当前时间戳（微秒）
 * @return 时间戳
 */
uint64_t GetTimestampUs();

/**
 * @brief 将时间戳转换为 RTP 时间戳
 * @param pts_us 微秒时间戳
 * @param clock_rate 时钟频率（如 90000）
 * @return RTP 时间戳
 */
uint32_t PtsToRtpTimestamp(uint64_t pts_us, uint32_t clock_rate);

/**
 * @brief 计算 CRC32 校验和
 * @param data 数据
 * @param len 数据长度
 * @return CRC32 值
 */
uint32_t CalcCrc32(const uint8_t* data, size_t len);

/**
 * @brief 网络字节序转换（主机字节序转网络字节序）
 * @param hostlong 主机字节序
 * @return 网络字节序
 */
uint32_t Htonl(uint32_t hostlong);

/**
 * @brief 网络字节序转换（网络字节序转主机字节序）
 * @param netlong 网络字节序
 * @return 主机字节序
 */
uint32_t Ntohl(uint32_t netlong);

/**
 * @brief 网络字节序转换（主机字节序转网络字节序）
 * @param hostshort 主机字节序
 * @return 网络字节序
 */
uint16_t Htons(uint16_t hostshort);

/**
 * @brief 网络字节序转换（网络字节序转主机字节序）
 * @param netshort 网络字节序
 * @return 主机字节序
 */
uint16_t Ntohs(uint16_t netshort);

/**
 * @brief 日志宏定义
 */
#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) \
  fprintf(stderr, "[DEBUG] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#define LOG_INFO(fmt, ...) \
  fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
  fprintf(stderr, "[WARN] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
  fprintf(stderr, "[ERROR] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

}  // namespace utils
}  // namespace gb28181

#endif  // GB28181_DEV_UTILS_H_
