#pragma once

#include <string>

#include <pb/Error.h>
#include <pb/FileMapper.h>
#include <pb/GradualControllable.h>
#include <pb/ImageReader.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

class PhotoBook final {
public:
  PhotoBook(GradualControllableListener &listener) : mListener(listener){};
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void addMedia(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validInputRootPath(fsPath);
    std::visit(overloaded{[this](PB::Path const &path) {
                            printDebug("Add media %s\n", path.string().c_str());
                            mMediaFolders.insert(
                                {path, MediaMapper<GradualControllableListener>(
                                           path, mListener)});
                          },
                          [this](Error error) { mListener.doError(error); }},
               result);

    mMediaFolders.at(fsPath).start();
  }

  void setOutputPath(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validOutputRootPath(fsPath);
    std::visit(
        overloaded{[this](PB::Path const &path) mutable { mOutputPath = path; },
                   [this](Error error) { mListener.doError(error); }},
        result);
  }

  auto rootPaths() const -> std::vector<Path>
  {
    std::vector<Path> result;
    for (auto &[key, value] : mMediaFolders) {
      result.push_back(key);
    }
    return result;
  }

private:
  auto loadImage(std::string const &path) -> std::optional<cv::Mat>
  {
    return ImageReader::defaultRead()(path);
  }

  void exportImage([[maybe_unused]] std::string const &path) {}

  GradualControllableListener &mListener;

  std::unordered_map<Path, MediaMapper<GradualControllableListener>>
                                     mMediaFolders;
  std::optional<Path>                mOutputPath = std::nullopt;
  std::vector<std::filesystem::path> mImagesMapCache;
};
} // namespace PB