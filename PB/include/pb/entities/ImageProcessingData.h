#pragma once

#include <memory>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

namespace PB {

enum ImageProcessingType { None, LUT };

struct ImageProcessingData {
  std::shared_ptr<cv::Mat> inImage = nullptr;
  std::shared_ptr<cv::Mat> outImage = nullptr;

  virtual ~ImageProcessingData() = default;

  virtual ImageProcessingType type() const { return ImageProcessingType::None; }

  static ImageProcessingData empty() { return ImageProcessingData(); }
};
} // namespace PB