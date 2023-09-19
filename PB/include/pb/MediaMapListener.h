#pragma once

#include <pb/Error.h>
#include <pb/MediaMap.h>
#include <pb/util/Concepts.h>

namespace PB {

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookConcept<PhotoBookType>
class MediaMapListener final {
public:
  explicit MediaMapListener(PhotoBook<PhotoBookType, PersistenceType> &parent)
      : mParent(parent)
  {
  }
  ~MediaMapListener() = default;
  void onFinished(MediaMap &newMediaMap, Path &path)
  {
    mParent.onNewMediaMap(path, newMediaMap);
  }
  void onStopped() {}
  void onStarted() {}
  void onPaused() {}
  void onResumed() {}

  void onProgressUpdate() {}
  void onError(PB::Error error) { mParent.onError(error); }

private:
  PhotoBook<PhotoBookType, PersistenceType> &mParent;
};
} // namespace PB