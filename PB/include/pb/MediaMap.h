#pragma once

#include <algorithm>
#include <filesystem>
#include <set>
#include <vector>

#include <pb/util/CircularIterator.h>
#include <pb/util/Traits.h>
namespace PB {
class MediaMap final {
public:
  MediaMap() = default;

  explicit MediaMap(std::vector<std::filesystem::path> &paths)
  {
    mPaths.clear();
    for (auto &val : paths) {
      mPaths.push_back(val);
    }
  }

  MediaMap(MediaMap const &other)
  {
    mPaths.clear();
    for (auto &val : other.mPaths) {
      mPaths.push_back(val);
    }
  }

  MediaMap(MediaMap &&other) noexcept
  {
    mPaths.clear();
    for (auto &val : other.mPaths) {
      mPaths.push_back(val);
    }
  }

  MediaMap &operator=(MediaMap const &other)
  {
    mPaths.clear();
    for (auto &val : other.mPaths) {
      mPaths.push_back(val);
    }
    return *this;
  }

  ~MediaMap() = default;

  void add(std::filesystem::path &path)
  {
    bool isDirectory = std::filesystem::is_directory(path);
    bool isRegularFile = std::filesystem::is_regular_file(path);

    if (isRegularFile && validImagePath(path)) {
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

  [[nodiscard]] auto iterator() -> std::optional<CircularIterator>
  {
    return CircularIterator(mPaths);
  }

  static bool validImagePath(Path const& path)
  {
    const std::set<std::string> sValidFileExtensions = {".jpg", ".jpeg",
                                                               ".png"};

    std::string extensionStr = path.extension().string();
    std::transform(extensionStr.begin(), extensionStr.end(),
                   extensionStr.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });

    return sValidFileExtensions.contains(extensionStr);
  }

private:

  std::vector<std::filesystem::path> mPaths;
};
} // namespace PB