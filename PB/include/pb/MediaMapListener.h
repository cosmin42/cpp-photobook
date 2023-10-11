#pragma once

#include <pb/Error.h>
#include <pb/MediaMap.h>
#include <pb/util/Concepts.h>

namespace PB {

template <typename PhotoBookType>
  requires PhotoBookListenerConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType>
  requires PhotoBookListenerConcept<PhotoBookType>
class MediaMapListener final {
public:
  explicit MediaMapListener(PhotoBook<PhotoBookType> &parent)
      : mParent(parent)
  {
  }
  ~MediaMapListener() = default;
  void onFinished(MediaMap &newMediaMap, Path &path)
  {
    mParent.onImportFolderMapped(path, newMediaMap);
  }
  void onStopped() {}
  void onStarted() {}
  void onPaused() {}
  void onResumed() {}

  void onProgressUpdate() {}
  void onError(PB::Error error) { mParent.onError(error); }

private:
  PhotoBook<PhotoBookType> &mParent;
};
} // namespace PB