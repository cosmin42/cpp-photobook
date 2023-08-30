#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <opencv2/core.hpp>

namespace PB {
class ImageReader final {
public:
  ImageReader() = delete;
  ImageReader(ImageReader const &) = delete;
  ImageReader(ImageReader &&) = delete;
  ImageReader &operator=(ImageReader const &other) = delete;
  ~ImageReader() = delete;

  static auto defaultRead() -> auto
  {
    return [](std::filesystem::path const &path) -> std::optional<cv::Mat> {
      return read(path);
    };
  }

private:
  static auto read(std::filesystem::path const &path) -> std::optional<cv::Mat>;
};
} // namespace PB