#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#pragma warning(pop)

#include <filesystem>
#include <vector>

namespace PB {
class ImageSetWriter final {
public:
  ImageSetWriter() = default;
  ImageSetWriter(ImageSetWriter const &) = delete;
  ImageSetWriter(ImageSetWriter &&) = delete;
  ImageSetWriter &operator=(ImageSetWriter const &other) = delete;
  ~ImageSetWriter() = default;

  void writeImages(std::vector<cv::Mat> const  &images,
                   std::filesystem::path const &path) const;
};
} // namespace PB