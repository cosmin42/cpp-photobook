#pragma once

#include <string>

#include <pb/Error.h>
#include <pb/FileMapper.h>
#include <pb/GradualControllable.h>
#include <pb/ImageReader.h>

namespace PB {

class PhotoBook final : public GradualControllable {
public:
  explicit PhotoBook(GradualControllableListener &listener);
  PhotoBook() = delete;
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void addMedia(std::string const &path);
  void setOutputPath(std::string const &path);

private:

  auto loadImage(std::string const &path) -> std::optional<cv::Mat>;

  void exportImage([[maybe_unused]] std::string const &path);

  void doStart() override;
  void doStop() override {}
  void doPause() override {}
  void doResume() override {}
  void doFinish() override {}

  std::unordered_map<Path, MediaMapper>   mMediaFolders;
  std::optional<Path> mOutputPath = std::nullopt;

  std::vector<std::filesystem::path> mImagesMapCache;

};
} // namespace PB