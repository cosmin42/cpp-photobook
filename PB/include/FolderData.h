#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <filesystem>
#include <string>

namespace PB {

class FolderData final {
public:
  FolderData() = default;

  explicit FolderData(std::string name, std::filesystem::path path);

  FolderData(const FolderData &folderData);
  FolderData(const FolderData &&folderData);

  FolderData& operator=(const FolderData &folderData);
  FolderData& operator=(const FolderData &&folderData);

  ~FolderData() = default;

  [[nodiscard]] auto name() const -> std::string;
  [[nodiscard]] auto path() const -> std::filesystem::path;
  
  [[nodiscard]] auto id() const -> boost::uuids::uuid;

  void setName(std::string &newName);
  void setName(std::string &&newName);

  void setPath(std::filesystem::path &newPath);
  void setPath(std::filesystem::path &&newPath);

private:
  boost::uuids::uuid    mUUID;
  std::string           mName;
  std::filesystem::path mPath;
};

} // namespace PB