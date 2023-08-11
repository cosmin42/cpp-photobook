#pragma once

#include <string>

#include <DataTree.h>

namespace PB {

class FilesMap final {
public:
  FilesMap() = delete;
  explicit FilesMap(const std::string &rootDirectory);
  explicit FilesMap(const std::string &&rootDirectory);

  ~FilesMap() = default;

  auto map() const -> TreeNodeDataWrapper;

private:
  const std::string mRootDirectory;
};

} // namespace PB