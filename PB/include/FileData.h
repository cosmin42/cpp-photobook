#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <string>

namespace PB {

class FileData final {
public:
  FileData() = default;

  explicit FileData(std::string name, std::filesystem::path path);

  FileData(const FileData &fileData);
  FileData(const FileData &&fileData);

  FileData &operator=(const FileData &fileData);
  FileData &operator=(const FileData &&fileData);

  ~FileData() = default;

  [[nodiscard]] auto name() const -> std::string;
  [[nodiscard]] auto path() const -> std::filesystem::path;

  [[nodiscard]] auto id() const -> boost::uuids::uuid;

  void setName(std::string &newName);
  void setName(std::string &&newName);

  void setPath(std::filesystem::path &newPath);
  void setPath(std::filesystem::path &&newPath);

  operator std::string() const
  {
    std::string str = filePrefix + mPath.string() + "(" +
                      boost::uuids::to_string(mUUID) + ")";

    return str;
  }

private:
  static const std::string filePrefix;
  boost::uuids::uuid       mUUID;
  std::string              mName;
  std::filesystem::path    mPath;
};

} // namespace PB