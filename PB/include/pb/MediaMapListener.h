#pragma once

#include <pb/Error.h>
#include <pb/MediaMap.h>
#include <pb/util/Concepts.h>

namespace PB {

template <typename TaskManageableType>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename T>
  requires TaskManageableConcept<T>
class MediaMapListener final {
public:
  explicit MediaMapListener(PhotoBook<T> &parent) : mParent(parent) {}
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
  PhotoBook<T> &mParent;
};
} // namespace PB