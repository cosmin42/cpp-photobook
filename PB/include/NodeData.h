#pragma once

#include <filesystem>
#include <string>

namespace PB {

class NodeData final {
public:
  NodeData() = default;

  explicit NodeData(std::string name, std::filesystem::path path);

  ~NodeData() = default;

  [[nodiscard]] auto name() const noexcept -> std::string;
  [[nodiscard]] auto path() const noexcept -> std::filesystem::path;

  void setName(std::string &newName) noexcept;
  void setName(std::string &&newName) noexcept;

  void setPath(std::filesystem::path &newPath) noexcept;
  void setPath(std::filesystem::path &&newPath) noexcept;

private:
  std::string mName;
  std::filesystem::path mPath;
};

} // namespace PB