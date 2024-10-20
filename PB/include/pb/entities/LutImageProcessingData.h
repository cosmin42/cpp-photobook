#pragma once

#include <pb/entities/ImageProcessingData.h>

namespace PB {
struct LutImageProcessingData final : public ImageProcessingData {
  std::vector<cv::Vec3f> lut;

  ImageProcessingType type() const override { return ImageProcessingType::LUT; }
};
} // namespace PB
