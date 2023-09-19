#pragma once

#include <pb/Error.h>
#include <pb/MediaMap.h>
#include <pb/util/Concepts.h>

namespace PB {

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class MediaMapListener final {
public:
  explicit MediaMapListener(PhotoBook<TaskManageableType, T> &parent)
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
  PhotoBook<TaskManageableType, T> &mParent;
};
} // namespace PB