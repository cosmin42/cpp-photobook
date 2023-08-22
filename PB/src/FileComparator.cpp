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

std::queue<std::string> CustomComparator::tokenizeDate(std::string const &blob)
{
  auto tokensRanges = blob | std::views::split('.');

  int                     count = 0;
  std::queue<std::string> tokensQueue;

  for (const auto &tokenRange : tokensRanges) {
    assert(count < DAY_MONTH_YEAR_COUNT);
    tokensQueue.push(std::string(tokenRange.begin(), tokenRange.end()));
    count++;
  }

  return tokensQueue;
}

std::optional<std::chrono::year_month_day>
CustomComparator::interpretTokens(std::queue<std::string> tokens)
{
  if (tokens.empty()) {
    return std::nullopt;
  }

  std::chrono::day   day{0};
  std::chrono::month month{0};
  std::chrono::year  year{0};

  auto &yearStr = tokens.front();
  year = interpretToken<std::chrono::year>(yearStr);
  tokens.pop();
  if (tokens.empty()) {
    std::chrono::year_month_day{year, month, day};
  }

  auto &monthStr = tokens.front();
  month = interpretToken<std::chrono::month>(monthStr);
  tokens.pop();
  if (tokens.empty()) {
    return std::chrono::year_month_day{year, month, day};
  }

  auto &dayStr = tokens.front();
  day = interpretToken<std::chrono::day>(dayStr);
  tokens.pop();

  return std::chrono::year_month_day{year, month, day};
}

} // namespace PB
