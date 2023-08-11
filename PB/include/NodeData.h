#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <filesystem>
#include <string>

namespace PB {

class NodeData final {
public:
  NodeData() = default;

  explicit NodeData(std::string name, std::filesystem::path path);

  ~NodeData() = default;

  [[nodiscard]] auto name() const -> std::string;
  [[nodiscard]] auto path() const -> std::filesystem::path;

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