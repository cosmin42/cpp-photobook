#include <pb/Error.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

auto FileInfo::fileExists(std::variant<std::filesystem::path, Error> monoidPath)
    -> std::variant<std::filesystem::path, Error>
{
  if (std::holds_alternative<Error>(monoidPath)) {
    return monoidPath;
  }
  auto& path = std::get<std::filesystem::path>(monoidPath);
  if (!std::filesystem::exists(path)) {
    return Error() << ErrorKind::FileDoesNotExist;
  }
  return monoidPath;
}

auto FileInfo::isDirectory(
    std::variant<std::filesystem::path, Error> monoidPath)
    -> std::variant<std::filesystem::path, Error>
{
  if (std::holds_alternative<Error>(monoidPath)) {
    return monoidPath;
  }
  auto &path = std::get<std::filesystem::path>(monoidPath);
  if (std::filesystem::is_directory(path)) {
    return monoidPath;
  }
  return Error() << ErrorKind::NotADirectory;
}

auto FileInfo::validInputRootPath(std::filesystem::path const &path)
    -> std::variant<std::filesystem::path, Error>
{
  return compose(fileExists, isDirectory)(path);
}
auto FileInfo::validOutputRootPath(std::filesystem::path const &path)
    -> std::variant<std::filesystem::path, Error>
{
  return compose(fileExists, isDirectory)(path);
}
} // namespace PB