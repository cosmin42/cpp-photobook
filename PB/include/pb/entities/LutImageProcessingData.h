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
struct LutImageProcessingData final {
  std::vector<cv::Vec4f> lut;
  Path                   inImage;
  Path                   outImage;
};

struct LutInMemoryData final {
  std::vector<cv::Vec4f> lut;
  cv::Mat                inImage;
  cv::Mat                outImage;
};

struct LutImageProcessingAndEffectsData final {
  std::vector<cv::Vec4f> lut;
  Path                   inImage;
  Path                   outImage;
  double                 saturation;
  double                 contrast;
  double                 brightness;
};

typedef std::variant<LutImageProcessingData, LutInMemoryData,
                     LutImageProcessingAndEffectsData>
    ImageProcessingData;

} // namespace PB
