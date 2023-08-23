#pragma once

#include <map>
#include <optional>
#include <string>

#include <DataNode.h>

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