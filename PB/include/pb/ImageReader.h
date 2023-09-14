#pragma once

#include <filesystem>
#include <optional>
#include <string>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#pragma warning(pop)

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