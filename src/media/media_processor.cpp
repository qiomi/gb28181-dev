/**
 * @file media_processor.cpp
 * @brief 媒体处理器实现（框架）
 */

#include "media_processor.h"
#include "../utils/utils.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace gb28181 {

class MediaProcessor::Impl {
 public:
  int codec_ = 0;  // VIDEO_CODEC_H264
  bool initialized_ = false;
};

MediaProcessor::MediaProcessor() : impl_(std::make_unique<Impl>()) {}

MediaProcessor::~MediaProcessor() {}

int MediaProcessor::Init(int codec) {
  if (impl_->initialized_) {
    return 0;
  }

  impl_->codec_ = codec;
  impl_->initialized_ = true;

  LOG_INFO("Media processor initialized, codec=%d", codec);
  return 0;
}

int MediaProcessor::ProcessRawStream(const uint8_t* data, int len,
                                     std::vector<MediaFrame>& frames) {
  if (!impl_->initialized_ || !data || len <= 0) {
    return 0;
  }

  // TODO: 实现 H.264/H.265 NALU 提取逻辑
  // 查找起始码 0x00000001 或 0x000001
  int nalu_count = 0;
  const uint8_t* p = data;
  const uint8_t* end = data + len;

  while (p < end) {
    // 查找起始码
    if (p + 4 <= end && p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x00 &&
        p[3] == 0x01) {
      // 找到 4 字节起始码
      const uint8_t* nalu_start = p + 4;

      // 查找下一个起始码
      const uint8_t* next = p + 4;
      while (next + 4 <= end) {
        if (next[0] == 0x00 && next[1] == 0x00 && next[2] == 0x00 &&
            next[3] == 0x01) {
          break;
        }
        next++;
      }

      // 创建一个 NALU 帧
      MediaFrame frame;
      frame.data.assign(nalu_start, next);
      frame.pts = utils::GetTimestampUs();
      frame.is_key_frame = (GetH264NaluType(nalu_start) == NaluType::NAL_IDR_SLICE);

      frames.push_back(frame);
      nalu_count++;

      p = next;
    } else {
      p++;
    }
  }

  LOG_DEBUG("Processed %d NALUs from %d bytes", nalu_count, len);
  return nalu_count;
}

int MediaProcessor::PsPackH264(const uint8_t* nalu_data, int nalu_len,
                               uint64_t pts, std::vector<uint8_t>& ps_packet) {
  if (!impl_->initialized_ || !nalu_data || nalu_len <= 0) {
    return -1;
  }

  // TODO: 实现 H.264 的 PS 封装
  // 参考 GB/T 28181-2016 附录 B

  // PS packet structure:
  // - PS header (14 bytes)
  // - PES header (variable)
  // - PES payload (NALU data)

  // 这里仅提供框架，具体实现需要参考国标
  uint64_t rtp_ts = utils::PtsToRtpTimestamp(pts, 90000);

  // 简化的 PS 封装示例
  ps_packet.clear();

  // PS 包头部（简化版）
  ps_packet.push_back(0x00);  // start_code
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x01);
  ps_packet.push_back(0xBA);  // pack_start_code

  // Pack header
  ps_packet.push_back(0x40 | ((rtp_ts >> 29) & 0x07));
  ps_packet.push_back(0x80 | ((rtp_ts >> 22) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts >> 15) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts >> 8) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts) & 0x7F));
  ps_packet.push_back(0x00);  // stuffing_length

  // System header (可选)
  // ...

  // PES header for video stream
  ps_packet.push_back(0x00);  // start_code
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x01);
  ps_packet.push_back(0xE0);  // stream_id (video stream 0)

  // PES length (will be updated)
  int pes_len_pos = ps_packet.size();
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x00);

  // PES optional header
  ps_packet.push_back(0x80);  // marker bits + priority
  ps_packet.push_back(0x80);  // PTS flag
  ps_packet.push_back(0x05);  // PES header data length

  // PTS
  ps_packet.push_back(0x20 | ((rtp_ts >> 29) & 0x07));
  ps_packet.push_back(((rtp_ts >> 22) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts >> 15) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts >> 8) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts) & 0x7F) | 0x80);

  // NALU data
  ps_packet.insert(ps_packet.end(), nalu_data, nalu_data + nalu_len);

  // Update PES length
  int pes_len = ps_packet.size() - pes_len_pos - 2;
  ps_packet[pes_len_pos] = (pes_len >> 8) & 0xFF;
  ps_packet[pes_len_pos + 1] = pes_len & 0xFF;

  LOG_DEBUG("PS packed H.264 NALU: %d bytes -> %lu bytes, pts=%lu", nalu_len,
            ps_packet.size(), pts);
  return 0;
}

int MediaProcessor::PsPackH265(const uint8_t* nalu_data, int nalu_len,
                               uint64_t pts, std::vector<uint8_t>& ps_packet) {
  if (!impl_->initialized_ || !nalu_data || nalu_len <= 0) {
    return -1;
  }

  // TODO: 实现 H.265 的 PS 封装
  // 类似 H.264，但使用不同的 stream_type
  uint64_t rtp_ts = utils::PtsToRtpTimestamp(pts, 90000);

  ps_packet.clear();

  // PS header
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x01);
  ps_packet.push_back(0xBA);

  ps_packet.push_back(0x40 | ((rtp_ts >> 29) & 0x07));
  ps_packet.push_back(0x80 | ((rtp_ts >> 22) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts >> 15) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts >> 8) & 0x7F));
  ps_packet.push_back(0x80 | ((rtp_ts) & 0x7F));
  ps_packet.push_back(0x00);

  // PES header
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x01);
  ps_packet.push_back(0xE0);

  int pes_len_pos = ps_packet.size();
  ps_packet.push_back(0x00);
  ps_packet.push_back(0x00);

  ps_packet.push_back(0x80);
  ps_packet.push_back(0x80);
  ps_packet.push_back(0x05);

  ps_packet.push_back(0x20 | ((rtp_ts >> 29) & 0x07));
  ps_packet.push_back(((rtp_ts >> 22) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts >> 15) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts >> 8) & 0x7F) | 0x80);
  ps_packet.push_back(((rtp_ts) & 0x7F) | 0x80);

  ps_packet.insert(ps_packet.end(), nalu_data, nalu_data + nalu_len);

  int pes_len = ps_packet.size() - pes_len_pos - 2;
  ps_packet[pes_len_pos] = (pes_len >> 8) & 0xFF;
  ps_packet[pes_len_pos + 1] = pes_len & 0xFF;

  LOG_DEBUG("PS packed H.265 NALU: %d bytes -> %lu bytes, pts=%lu", nalu_len,
            ps_packet.size(), pts);
  return 0;
}

NaluType MediaProcessor::GetH264NaluType(const uint8_t* nalu_data) {
  if (!nalu_data) {
    return NaluType::NAL_SLICE;
  }

  uint8_t type = nalu_data[0] & 0x1F;
  return static_cast<NaluType>(type);
}

NaluType MediaProcessor::GetH265NaluType(const uint8_t* nalu_data) {
  if (!nalu_data) {
    return NaluType::NAL_SLICE;
  }

  uint8_t type = (nalu_data[0] >> 1) & 0x3F;
  return static_cast<NaluType>(type);
}

}  // namespace gb28181
