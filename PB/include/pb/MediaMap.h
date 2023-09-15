#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/CircularIterator.h>
#include <pb/util/Traits.h>
namespace PB {
class MediaMap final {
public:
  MediaMap() = default;

  explicit MediaMap(std::vector<std::filesystem::path> &paths)
  {
    mPaths.reserve(paths.size());
    std::copy(paths.begin(), paths.end(), mPaths.begin());
  }

  MediaMap(MediaMap const &other)
  {
    mPaths.reserve(other.mPaths.size());
    std::copy(other.mPaths.begin(), other.mPaths.end(), mPaths.begin());
  }

  MediaMap(MediaMap &&other) noexcept
  {
    mPaths.reserve(other.mPaths.size());
    std::copy(other.mPaths.begin(), other.mPaths.end(), mPaths.begin());
  }

  MediaMap &operator=(MediaMap const &other)
  {
    mPaths.reserve(other.mPaths.size());
    std::copy(other.mPaths.begin(), other.mPaths.end(), mPaths.begin());
    return *this;
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

  [[nodiscard]] auto iterator() -> std::optional<CircularIterator>
  {
    return CircularIterator(mPaths);
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
} // namespace PB