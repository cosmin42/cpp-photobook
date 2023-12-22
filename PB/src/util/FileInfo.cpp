#include <pb/util/Error.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PBDev {

auto FileInfo::fileExists(
    std::variant<std::filesystem::path, PBDev::Error> monoidPath)
    -> std::variant<std::filesystem::path, PBDev::Error>
{
  if (std::holds_alternative<PBDev::Error>(monoidPath)) {
    return monoidPath;
  }
  auto& path = std::get<std::filesystem::path>(monoidPath);
  if (!std::filesystem::exists(path)) {
    return PBDev::Error() << PB::ErrorCode::FileDoesNotExist;
  }
  return monoidPath;
}

auto FileInfo::isDirectory(
    std::variant<std::filesystem::path, PBDev::Error> monoidPath)
    -> std::variant<std::filesystem::path, PBDev::Error>
{
  if (std::holds_alternative<PBDev::Error>(monoidPath)) {
    return monoidPath;
  }
  auto &path = std::get<std::filesystem::path>(monoidPath);
  if (std::filesystem::is_directory(path)) {
    return monoidPath;
  }
  return PBDev::Error() << PB::ErrorCode::NotADirectory;
}

auto FileInfo::validInputRootPath(std::filesystem::path const &path)
    -> std::variant<std::filesystem::path, PBDev::Error>
{
  return PB::compose(fileExists, isDirectory)(path);
}
auto FileInfo::validOutputRootPath(std::filesystem::path const &path)
    -> std::variant<std::filesystem::path, PBDev::Error>
{
  return PB::compose(fileExists, isDirectory)(path);
}
} // namespace PB