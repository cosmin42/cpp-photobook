#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/CircularIterator.h>
#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Thread.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename T>
  requires TaskManageableConcept<T>
class MediaMapper final : public Thread {
public:
  explicit MediaMapper(std::filesystem::path const &root, T &listener)
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
      bool isDirectory = std::filesystem::is_directory(path);
      bool isRegularFile = std::filesystem::is_regular_file(path);

      if (isRegularFile && fileValidator(path)) {
        mPaths.push_back(path);
      }

      if (isDirectory) {
        mPaths.push_back(path);
      }
      mRecursiveIterator++;
    }
    mListener.onProgressUpdate();
  }

  auto paths() -> std::vector<std::filesystem::path> & { return mPaths; }

  void finish() override { mListener.onFinished(); }

  [[nodiscard]] auto size() const -> unsigned
  {
    return (unsigned)mPaths.size();
  }

  auto access(unsigned index) const -> Path
  {
    if (index >= mPaths.size()) {
      return std::filesystem::path();
    }
    return mPaths.at(index);
  }

  [[nodiscard]] auto iterator()
  {
    return CircularIterator<MediaMapper<T>, Path>(*this);
  }

private:

  bool fileValidator(Path path) const
  {
    static const std::set<std::string> sValidFileExtensions = {"jpg", "jpeg",
                                                               "png"};
    // TODO: C++23 to be replaced with ends with
    std::string pathStr = path.string();
    for (auto &extension : sValidFileExtensions) {
      bool endsWith =
          std::equal(extension.rbegin(), extension.rend(), pathStr.rbegin());
      if (endsWith) {
        return true;
      }
    }
    return false;
  }

  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::vector<std::filesystem::path>            mPaths;

  T &mListener;
};

} // namespace PB