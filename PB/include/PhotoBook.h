#pragma once

#include <string>

#include <Error.h>
#include <FileMapper.h>
#include <ImageReader.h>

namespace PB {
class PhotoBookListener {
  virtual void onFinished() = 0;
  virtual void onStarted() = 0;
  virtual void onProgressUpdate() = 0;
  virtual void onError(Error error) = 0;
};

class PhotoBook final {
public:
  explicit PhotoBook(PhotoBookListener &listener);
  PhotoBook() = delete;
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  auto mapImages([[maybe_unused]] std::string const &root)
      -> std::vector<std::filesystem::path>;

  auto loadImage(std::string const &path) -> std::optional<cv::Mat>;

  void exportImage([[maybe_unused]] std::string const &path);

private:
  PhotoBookListener      &mListener;
  std::optional<FilesMap> fileMapper;
};
} // namespace PB