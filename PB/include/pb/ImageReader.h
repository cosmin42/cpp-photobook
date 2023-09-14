#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/FileMapper.h>

namespace PB {

template <typename T>
  requires TaskManageableConcept<T>
class ImageReader final {
public:
  // How many images are loaded beside the main one
  static constexpr unsigned sBufferSize = 3 * 2;
  ImageReader() = default;
  ImageReader(ImageReader const &) = delete;
  ImageReader(ImageReader &&) = delete;
  ImageReader &operator=(ImageReader const &other) = delete;
  ~ImageReader() = default;

  static auto defaultRead() -> auto
  {
    return [](std::filesystem::path const &path) -> std::shared_ptr<cv::Mat> {
      return read(path);
    };
  }

  void
  loadBuffer([[maybe_unused]] CircularIterator<MediaMapper<T>, Path> iterator)
  {
  }

private:
  static auto read(std::filesystem::path const &path)
      -> std::shared_ptr<cv::Mat>
  {
    cv::Mat image = cv::imread(path.string(), cv::IMREAD_COLOR);

    if (image.empty()) {
      printDebug("Image %s could not be read", path.string().c_str());
      return nullptr;
    }

    return std::make_shared<cv::Mat>(image);
  }

  std::map<Path, std::shared_ptr<cv::Mat>> mBuffer;
  std::vector<Path>                        mBufferIndex;
};
} // namespace PB