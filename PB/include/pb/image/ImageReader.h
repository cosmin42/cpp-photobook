#pragma once

#include <algorithm>
#include <filesystem>
#include <map>
#include <optional>
#include <string>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/infra/FileSupport.h>
#include <pb/jobs/PicturesSearchJob.h>

namespace PB {

class ImageReader final {
public:
  // How many images are loaded beside the main one
  static constexpr unsigned sBufferSize = 0 * 2 + 1;
  ImageReader() = default;
  ImageReader(ImageReader const &) = delete;
  ImageReader(ImageReader &&) = delete;
  ImageReader &operator=(ImageReader const &other) = delete;
  ~ImageReader() = default;

  auto read(Path const path, bool keepAspectRatio,
            cv::Size size = {0, 0}) -> std::shared_ptr<cv::Mat>
  {
    if (isCached(path)) {
      return mBuffer.at(path);
    }

    auto image = infra::loadImageToCvMat(path);
    if (!size.empty()) {
      return Process::resize(image, size, keepAspectRatio);
    }
    return image;
  }

  bool isValid(Path const path) const
  {
    auto image = cv::imread(path.string(), cv::IMREAD_COLOR);
    return !image.empty();
  }

  auto isCached(Path const &path) const -> bool
  {
    return mBuffer.find(path) != mBuffer.end();
  }

private:
  std::map<Path, std::shared_ptr<cv::Mat>> mBuffer;
  std::vector<Path>                        mBufferIndex;
};
} // namespace PB