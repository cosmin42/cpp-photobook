#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <opencv2/core.hpp>

namespace PB {
class ImageReader final {
public:
  ImageReader() = default;
  ImageReader(ImageReader const &) = delete;
  ImageReader(ImageReader &&) = delete;
  ImageReader &operator=(ImageReader const &other) = delete;
  ~ImageReader() = default;

  auto read(std::filesystem::path const &path) -> std::optional<cv::Mat>;
};
} // namespace PB