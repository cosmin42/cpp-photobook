#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>

namespace PB {
class VirtualImage {
public:
  virtual cv::Mat          fullImage() = 0;
  virtual cv::Mat          mediumImage() = 0;
  virtual cv::Mat          smallImage() = 0;
  virtual VirtualImageType type() const = 0;
};
} // namespace PB