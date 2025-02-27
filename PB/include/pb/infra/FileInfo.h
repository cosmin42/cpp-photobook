#pragma once
#include <filesystem>
#include <variant>

#include <pb/infra/Error.h>
#include <pb/infra/Traits.h>

namespace PBDev {
// TODO: be implemented using std::expected
class FileInfo {
public:
  FileInfo() = delete;
  ~FileInfo() = delete;

  static auto validInputRootPath(std::filesystem::path const &path)
      -> std::variant<std::filesystem::path, Error>;
  static auto validOutputRootPath(std::filesystem::path const &path)
      -> std::variant<std::filesystem::path, Error>;

  static auto fileExists(std::variant<std::filesystem::path, Error> monoidPath)
      -> std::variant<std::filesystem::path, Error>;
  static auto isDirectory(std::variant<std::filesystem::path, Error> monoidPath)
      -> std::variant<std::filesystem::path, Error>;

  static auto contains(Path a, Path b) -> bool;
};
} // namespace PBDev