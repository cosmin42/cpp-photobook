#pragma once

#include <pb/entities/ImageProcessingData.h>

namespace PB {
struct LutImageProcessingData final : public ImageProcessingData {
  std::vector<cv::Vec4f> lut;

  ImageProcessingType type() const override { return ImageProcessingType::LUT; }
};
} // namespace PB
