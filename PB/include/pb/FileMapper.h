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

template <typename TaskManageableType, typename T>
class MediaMapper final : public Thread {
public:
  explicit MediaMapper(
      std::filesystem::path const                             &root,
      std::shared_ptr<MediaMapListener<TaskManageableType, T>> listener)
      : Thread(Context::inst().sStopSource.get_token()), mListener(listener),
        mRoot(root)
  {
    printDebug("MediaMapper constructor.\n");
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        root, std::filesystem::directory_options::skip_permission_denied);
  }

  MediaMapper(MediaMapper const &other)
      : Thread(other), mMap(other.mMap), mListener(other.mListener),
        mRecursiveIterator(other.mRecursiveIterator), mRoot(other.mRoot)
  {
    printDebug("Copy MediaMapper\n");
  }

  MediaMapper(MediaMapper &&other) noexcept
      : Thread(std::move(other)), mMap(other.mMap), mListener(other.mListener),
        mRecursiveIterator(other.mRecursiveIterator), mRoot(other.mRoot)
  {
    printDebug("Move MediaMapper\n");
  }

  MediaMapper &operator=(MediaMapper const &other) = delete;

  ~MediaMapper() { printDebug("Destroying MediaMapper\n"); }

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
    mListener->onProgressUpdate();
  }

  void finish() override { mListener->onFinished(mMap, mRoot); }

  auto map() const -> MediaMap { return mMap; }

private:
  MediaMap                                              mMap;
  std::shared_ptr<MediaMapListener<TaskManageableType, T>> mListener;

  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  Path                                          mRoot;
};

} // namespace PB