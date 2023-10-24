#pragma once

#include <pb/Error.h>
#include <pb/util/Concepts.h>

namespace PB {

class PhotoBook;

class MediaMapListener final {
public:
  explicit MediaMapListener(PhotoBook &parent) : mParent(parent) {}
  ~MediaMapListener() = default;
  void onFinished(std::vector<std::filesystem::path> addedPaths, Path &path)
  {
    //mParent.onImportFolderMapped(path, addedPaths);
  }
  void onStopped() {}
  void onStarted() {}
  void onPaused() {}
  void onResumed() {}

  void onProgressUpdate() {}
  void onError(PB::Error error)
  { /*
    mParent.onError(error);*/
  }

private:
  PhotoBook &mParent;
};
} // namespace PB