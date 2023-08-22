#include <FileComparator.h>
#include <array>
#include <cassert>
#include <filesystem>
#include <ranges>
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

std::optional<std::string>
CustomComparator::extractPrefix(const std::string &input)
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

std::array<std::string, CustomComparator::DAY_MONTH_YEAR_COUNT>
CustomComparator::tokenizeDate(std::string const &blob)
{
  auto tokensRanges = blob | std::views::split('.');

  int                        count = 0;
  std::array<std::string, 3> tokens;

  for (const auto &tokenRange : tokensRanges) {
    assert(count < DAY_MONTH_YEAR_COUNT);
    tokens[count] = std::string(tokenRange.begin(), tokenRange.end());
    count++;
  }

  std::shift_right(tokens.begin(), tokens.end(), DAY_MONTH_YEAR_COUNT - count);
  std::fill(tokens.begin(), tokens.begin() + (DAY_MONTH_YEAR_COUNT - count),
            "");

  return tokens;
}

} // namespace PB
