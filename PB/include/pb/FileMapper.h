#pragma once

#include <filesystem>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Thread.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename T, typename Content>
  requires RandomAccessibleConcept<T, Content>
class CircularIterator final {
public:
  explicit CircularIterator(T const &container) : mContainer(container) {}

  CircularIterator(CircularIterator const &other)
      : mContainer(other.mContainer), mIndex(other.mIndex)
  {
  }

  CircularIterator(CircularIterator &&other)
      : mContainer(other.mContainer), mIndex(other.mIndex)
  {
  }

  CircularIterator &operator=(CircularIterator const &other)
  {
    mIndex = other.mIndex;
    mContainer = other.mContainer;
    return *this;
  }

  ~CircularIterator() = default;

  auto current() const -> Content { return mContainer.access(mIndex); }

  CircularIterator &operator++()
  {
    if (mContainer.size() == 0) {
      return *this;
    }
    mIndex++;
    mIndex %= mContainer.size();
  }

  CircularIterator &operator--()
  {
    if (mContainer.size() == 0) {
      return *this;
    }
    if (mIndex == 0) {
      mIndex = mContainer.size();
    }
    mIndex--;
    return *this;
  }

private:
  T const &mContainer;
  unsigned mIndex = 0;
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

  [[nodiscard]] auto size() const -> unsigned { return mPaths.size(); }

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
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::vector<std::filesystem::path>            mPaths;

  T &mListener;
};

} // namespace PB