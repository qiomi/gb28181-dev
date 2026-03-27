/**
 * @file media_processor.h
 * @brief 媒体处理器接口定义
 */

#ifndef GB28181_DEV_MEDIA_PROCESSOR_H_
#define GB28181_DEV_MEDIA_PROCESSOR_H_

#include <stdint.h>
#include <memory>
#include <vector>

namespace gb28181 {

/**
 * @enum NaluType
 * @brief H.264 NALU 类型
 */
enum class NaluType {
  NAL_SLICE = 1,        // 非关键帧
  NAL_DPA = 2,
  NAL_DPB = 3,
  NAL_DPC = 4,
  NAL_IDR_SLICE = 5,    // 关键帧
  NAL_SEI = 6,
  NAL_SPS = 7,
  NAL_PPS = 8,
  NAL_AUD = 9,
  NAL_FILLER = 12,
};

/**
 * @struct MediaFrame
 * @brief 媒体帧结构
 */
struct MediaFrame {
  std::vector<uint8_t> data;  // 数据
  uint64_t pts;               // 时间戳（微秒）
  bool is_key_frame;          // 是否关键帧
  int width;                  // 宽度（可选）
  int height;                 // 高度（可选）
};

/**
 * @class MediaProcessor
 * @brief 媒体处理器类
 * 
 * 负责处理 H.264/H.265 码流，提取 NALU，PS 封装等
 */
class MediaProcessor {
 public:
  MediaProcessor();
  ~MediaProcessor();

  /**
   * @brief 初始化媒体处理器
   * @param codec 编码格式
   * @return 0 表示成功，负数表示失败
   */
  int Init(int codec);

  /**
   * @brief 处理原始码流，提取 NALU
   * @param data 输入数据
   * @param len 数据长度
   * @param frames 输出帧列表
   * @return 提取的 NALU 数量
   */
  int ProcessRawStream(const uint8_t* data, int len,
                       std::vector<MediaFrame>& frames);

  /**
   * @brief PS 封装 H.264 NALU
   * @param nalu_data NALU 数据
   * @param nalu_len NALU 长度
   * @param pts 时间戳
   * @param ps_packet 输出 PS 包
   * @return 0 表示成功，负数表示失败
   */
  int PsPackH264(const uint8_t* nalu_data, int nalu_len, uint64_t pts,
                 std::vector<uint8_t>& ps_packet);

  /**
   * @brief PS 封装 H.265 NALU
   * @param nalu_data NALU 数据
   * @param nalu_len NALU 长度
   * @param pts 时间戳
   * @param ps_packet 输出 PS 包
   * @return 0 表示成功，负数表示失败
   */
  int PsPackH265(const uint8_t* nalu_data, int nalu_len, uint64_t pts,
                 std::vector<uint8_t>& ps_packet);

  /**
   * @brief 判断 H.264 NALU 类型
   * @param nalu_data NALU 数据
   * @return NALU 类型
   */
  static NaluType GetH264NaluType(const uint8_t* nalu_data);

  /**
   * @brief 判断 H.265 NALU 类型
   * @param nalu_data NALU 数据
   * @return NALU 类型
   */
  static NaluType GetH265NaluType(const uint8_t* nalu_data);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace gb28181

#endif  // GB28181_DEV_MEDIA_PROCESSOR_H_
