#include <MetadataReader.h>

namespace PB {

auto MetadataReader::metadata(
    [[maybe_unused]] const std::string &absoluteFilePath) const
    -> std::map<std::string, std::string> const
{
  return std::map<std::string, std::string>();
}

auto MetadataReader::metadata(
    [[maybe_unused]] const std::string &&absoluteFilePath) const
    -> std::map<std::string, std::string> const
{
  return std::map<std::string, std::string>();
}
} // namespace PB