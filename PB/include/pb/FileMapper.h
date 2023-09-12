#pragma once

#include <filesystem>
#include <vector>

#include <pb/GradualControllable.h>
#include <pb/util/Thread.h>

namespace PB {

class MediaMapper final : public Thread {
public:
  explicit MediaMapper(std::filesystem::path const &root,
                       GradualControllableListener &listener);
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

  void executeSingleTask() override;

  auto paths() -> std::vector<std::filesystem::path> &;

  void finish() override;

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::vector<std::filesystem::path>            mPaths;

  GradualControllableListener &mListener;
};

} // namespace PB