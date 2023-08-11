#pragma once

#include <string>

#include <DataNode.h>

namespace PB {

class FilesMap final {
public:
  FilesMap() = delete;
  explicit FilesMap(const std::string &rootDirectory);
  explicit FilesMap(const std::string &&rootDirectory);

  ~FilesMap() = default;

  auto map() const -> EitherFolderOrFile;

private:
  const std::string mRootDirectory;
};

} // namespace PB