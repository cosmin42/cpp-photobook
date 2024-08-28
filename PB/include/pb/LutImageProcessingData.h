#pragma once

#include <pb/ImageProcessingData.h>

namespace PB {
struct LutImageProcessingData : public ImageProcessingData {
  std::vector<cv::Vec3f> lut;

  ImageProcessingType type() const override { return ImageProcessingType::LUT; }
};
} // namespace PB
