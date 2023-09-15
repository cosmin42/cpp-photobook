#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/CircularIterator.h>
#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/MediaMapListener.h>
#include <pb/util/Concepts.h>
#include <pb/util/Thread.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename TaskManageableType> class MediaMapper final : public Thread {
public:
  explicit MediaMapper(std::filesystem::path const          &root,
                       MediaMapListener<TaskManageableType> &listener)
      : Thread(Context::inst().sStopSource.get_token()), mListener(listener),
        mRoot(root)
  {
    printDebug("MediaMapper constructor.\n");
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        root, std::filesystem::directory_options::skip_permission_denied);
  }

  MediaMapper(MediaMapper const &other)
      : Thread(other), mRecursiveIterator(other.mRecursiveIterator),
        mListener(other.mListener), mRoot(other.mRoot)
  {
  }

  MediaMapper(MediaMapper &&other) noexcept
      : Thread(other), mRecursiveIterator(other.mRecursiveIterator),
        mListener(other.mListener), mRoot(other.mRoot)
  {
  }

  MediaMapper &operator=(MediaMapper const &) { return *this; }
  ~MediaMapper() = default;

  void executeSingleTask() override
  {
    if (mRecursiveIterator == std::filesystem::end(mRecursiveIterator)) {
      stop();
    }
    else {
      auto path = mRecursiveIterator->path();
      mMap.add(path);
      mRecursiveIterator++;
    }
    mListener.onProgressUpdate();
  }

  void finish() override { mListener.onFinished(mMap, mRoot); }

  auto map() const -> MediaMap { return mMap; }

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  MediaMap                                      mMap;
  MediaMapListener<TaskManageableType>         &mListener;
  Path                                          mRoot;
};

} // namespace PB