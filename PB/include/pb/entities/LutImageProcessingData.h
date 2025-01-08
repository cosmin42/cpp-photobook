#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/entities/ImageProcessingData.h>

namespace PB {
struct LutImageProcessingData final : public ImageProcessingData {
  std::vector<cv::Vec4f> lut;

  ImageProcessingType type() const override { return ImageProcessingType::LUT; }
};
} // namespace PB
