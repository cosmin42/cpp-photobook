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

  auto map() const -> std::shared_ptr<DataNode>;

private:
  const std::string mRootDirectory;

  static std::optional<EitherFolderOrFile> wrap(const std::filesystem::path& path);
};

} // namespace PB