#pragma once

#include <string>

#include <Error.h>
#include <FileMapper.h>
#include <GradualControllable.h>
#include <ImageReader.h>

namespace PB {

class PhotoBook final : public GradualControllable {
public:
  explicit PhotoBook(GradualControllableListener &listener);
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

  void doStart() override;
  void doStop() override {}
  void doPause() override {}
  void doResume() override {}
  void doFinish() override {}

  std::optional<FilesMap>      fileMapper = std::nullopt;

  std::optional<Path> mInputPath = std::nullopt;
  std::optional<Path> mOutputPath = std::nullopt;

  std::vector<std::filesystem::path> mImagesMapCache;
};
} // namespace PB