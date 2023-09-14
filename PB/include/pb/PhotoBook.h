#pragma once

#include <string>

#include <pb/Error.h>
#include <pb/FileMapper.h>
#include <pb/GradualControllable.h>
#include <pb/ImageReader.h>
#include <pb/util/Traits.h>

namespace PB {

class PhotoBook final {
public:
  PhotoBook(GradualControllableListener &listener) : mListener(listener){};
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void addMedia(std::string const &path);
  void setOutputPath(std::string const &path);

  auto rootPaths() const -> std::vector<Path>;

private:
  auto loadImage(std::string const &path) -> std::optional<cv::Mat>;

  void exportImage([[maybe_unused]] std::string const &path);

  GradualControllableListener &mListener;

  std::unordered_map<Path, MediaMapper> mMediaFolders;
  std::optional<Path>                   mOutputPath = std::nullopt;
  std::vector<std::filesystem::path>    mImagesMapCache;
};
} // namespace PB