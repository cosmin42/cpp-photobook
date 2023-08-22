#include <FileComparator.h>

#include <cassert>
#include <filesystem>
#include <regex>

namespace PB {

const std::string CustomComparator::prefixRegex =
    "^[[0-9]+(\.)][[0-9]+(\.)][0-9]+";

template <>
std::strong_ordering
CustomComparator::operator()(std::filesystem::path const &a,
                             std::filesystem::path const &b)
{
  assert(std::filesystem::exists(a) && "File doesn't exist.");
  assert(std::filesystem::exists(b) && "File doesn't exist.");
  return std::strong_ordering::greater;
}

std::optional<std::string> CustomComparator::extractPrefix(const std::string &input)
{
  std::regex  regexPattern(prefixRegex);
  std::smatch match;

  if (std::regex_search(input, match, regexPattern)) {
    return match.str();
  }
  else {
    return std::nullopt;
  }
}

} // namespace PB
