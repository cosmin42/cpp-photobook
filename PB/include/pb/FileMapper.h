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

class MediaMap final {
public:
  MediaMap() = default;

  explicit MediaMap(std::vector<std::filesystem::path> &paths)
  {
    mPaths.reserve(paths.size());
    std::copy(paths.begin(), paths.end(), paths.begin());
  }

  ~MediaMap() = default;

  void add(std::filesystem::path &path)
  {
    bool isDirectory = std::filesystem::is_directory(path);
    bool isRegularFile = std::filesystem::is_regular_file(path);

    if (isRegularFile && validEndtry(path)) {
      mPaths.push_back(path);
    }

    if (isDirectory) {
      mPaths.push_back(path);
    }
  }

  [[nodiscard]] auto size() const -> unsigned
  {
    return (unsigned)mPaths.size();
  }

  auto access(unsigned index) const -> std::optional<Path>
  {
    if (index >= mPaths.size()) {
      return std::nullopt;
    }
    return mPaths.at(index);
  }

  [[nodiscard]] auto iterator()
  {
    return CircularIterator<MediaMap, Path>(*this);
  }

private:
  bool validEndtry(Path path) const
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

  std::vector<std::filesystem::path> mPaths;
};

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
      mMap.add(path);
      mRecursiveIterator++;
    }
    mListener.onProgressUpdate();
  }

  auto paths() -> std::vector<std::filesystem::path> & { return mPaths; }

  void finish() override { mListener.onFinished(); }

  auto map() const -> MediaMap { return mMap; }

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  MediaMap                                      mMap;
  T                                            &mListener;
};

} // namespace PB