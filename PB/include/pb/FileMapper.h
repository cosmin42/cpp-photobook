#pragma once

#include <filesystem>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Thread.h>

namespace PB {

template <TaskManageableConcept TaskManageableType>
class MediaMapper final : public Thread {
public:
  explicit MediaMapper(std::filesystem::path const &root,
                       TaskManageableType          &listener)
      : Thread(Context::inst().sStopSource.get_token()), mListener(listener)
  {
    printDebug("MediaMapper constructor.\n");
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        root, std::filesystem::directory_options::skip_permission_denied);
  }

  MediaMapper(MediaMapper const &other)
      : Thread(other), mRecursiveIterator(other.mRecursiveIterator),
        mListener(other.mListener)
  {
  }

  MediaMapper(MediaMapper &&other) noexcept
      : Thread(other), mRecursiveIterator(other.mRecursiveIterator),
        mListener(other.mListener)
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
      if (std::filesystem::is_directory(path) ||
          std::filesystem::is_regular_file(path)) {
        mPaths.push_back(path);
      }
      mRecursiveIterator++;
    }
    mListener.onProgressUpdate();
  }

  auto paths() -> std::vector<std::filesystem::path> & { return mPaths; }

  void finish() override { mListener.onFinished(); }

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::vector<std::filesystem::path>            mPaths;

  TaskManageableType &mListener;
};

} // namespace PB