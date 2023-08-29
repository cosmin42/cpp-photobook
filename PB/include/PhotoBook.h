#pragma once

#include <string>

#include <Error.h>
#include <FileMapper.h>
#include <GradualControllable.h>
#include <ImageReader.h>

namespace PB {

class PhotoBookListener {
public:
  virtual void onFinished() = 0;
  virtual void onStarted() = 0;
  virtual void onProgressUpdate() = 0;
  virtual void onError(Error error) = 0;
};

class PhotoBook final : public GradualControllable {
public:
  explicit PhotoBook(PhotoBookListener &listener);
  PhotoBook() = delete;
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void setInputPath(std::string const &path);
  void setOutputPath(std::string const &path);

private:
  auto mapImages([[maybe_unused]] std::string const &root)
      -> std::vector<std::filesystem::path>;

  auto loadImage(std::string const &path) -> std::optional<cv::Mat>;

  void exportImage([[maybe_unused]] std::string const &path);

  void doStart() override {}
  void doStop() override {}
  void doPause() override {}
  void doResume() override {}

  PhotoBookListener      &mListener;
  std::optional<FilesMap> fileMapper;

  std::filesystem::path mInputPath;
  std::filesystem::path mOutputPath;
};
} // namespace PB