#pragma once

#include <string>

namespace PB {

class FilesMap final {
public:
  FilesMap() = delete;
  explicit FilesMap(const std::string &rootDirectory);
  explicit FilesMap(const std::string &&rootDirectory);

  ~FilesMap() = default;

private:
  const std::string mRootDirectory;
};

} // namespace PB