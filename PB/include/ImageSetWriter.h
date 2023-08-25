#pragma once

#include <opencv2/core.hpp>

#include <vector>

namespace PB {
class ImageSetWriter final {
public:
  ImageSetWriter() = default;
  ImageSetWriter(ImageSetWriter const &) = delete;
  ImageSetWriter(ImageSetWriter &&) = delete;
  ImageSetWriter &operator=(ImageSetWriter const &other) = delete;
  ~ImageSetWriter() = default;

  void writeImages(std::vector<cv::Mat> const &images,
                   std::string const          &path) const;
};
} // namespace PB