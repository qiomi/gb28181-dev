/**
 * @file utils.cpp
 * @brief 工具函数实现
 */

#include "utils.h"

#include <chrono>
#include <cstring>
#include <random>

namespace gb28181 {
namespace utils {

uint32_t GenerateSsrc() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis;
    return dis(gen);
}

uint64_t GetTimestampUs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration)
        .count();
}

uint32_t PtsToRtpTimestamp(uint64_t pts_us, uint32_t clock_rate) {
    // RTP 时间戳 = (PTS / 1000000) * clock_rate
    return static_cast<uint32_t>((pts_us * clock_rate) / 1000000);
}

uint32_t CalcCrc32(const uint8_t* data, size_t len) {
    // 简化的 CRC32 计算（实际应用中应使用标准 CRC32 算法）
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

uint32_t Htonl(uint32_t hostlong) {
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        return ((hostlong & 0x000000FFU) << 24) |
               ((hostlong & 0x0000FF00U) << 8) |
               ((hostlong & 0x00FF0000U) >> 8) |
               ((hostlong & 0xFF000000U) >> 24);
    }
    return hostlong;
}

uint32_t Ntohl(uint32_t netlong) {
    return Htonl(netlong);  // 操作相同
}

uint16_t Htons(uint16_t hostshort) {
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        return ((hostshort & 0x00FF) << 8) | ((hostshort & 0xFF00) >> 8);
    }
    return hostshort;
}

uint16_t Ntohs(uint16_t netshort) {
    return Htons(netshort);  // 操作相同
}

}  // namespace utils
}  // namespace gb28181
