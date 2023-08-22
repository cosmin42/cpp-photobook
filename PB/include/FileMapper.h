#pragma once

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

  auto map() const -> std::shared_ptr<DataNode>;

private:
  const std::string mRootDirectory;

  static std::pair<boost::uuids::uuid, std::optional<DataNode>>
  wrap(const std::filesystem::path &path);
};

} // namespace PB