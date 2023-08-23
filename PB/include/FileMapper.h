#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace PB {

class FilesMap final {
public:
  FilesMap() = delete;
  explicit FilesMap(const std::string &rootDirectory);
  explicit FilesMap(std::string &&rootDirectory);

  ~FilesMap() = default;

  auto map() const -> std::vector<std::filesystem::path>;

private:
  const std::string mRootDirectory;

};

} // namespace PB