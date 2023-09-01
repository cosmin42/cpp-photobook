#pragma once

#include <filesystem>
#include <vector>

#include <pb/util/Thread.h>

namespace PB {

class MediaMapper final : public Thread {
public:
  explicit MediaMapper(std::filesystem::path const &root,
                       std::function<void()>        onFinish);
  MediaMapper(MediaMapper const &other) : Thread(other) {}
  MediaMapper(MediaMapper &&other) noexcept : Thread(other) {}
  MediaMapper &operator=(MediaMapper const &) { return *this; }
  ~MediaMapper() = default;

  void executeSingleTask() override;

  auto paths() -> std::vector<std::filesystem::path> &;

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;

  std::vector<std::filesystem::path> mPaths;
};

} // namespace PB