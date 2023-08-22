#include <FileComparator.h>
#include <array>
#include <cassert>
#include <filesystem>
#include <ranges>
#include <regex>

namespace PB {

const std::string CustomComparator::prefixRegex = "^([0-9]*.)?([0-9]*.)?[0-9]*";

template <>
bool CustomComparator::operator()(std::string const &a, std::string const &b)
{
  auto aPrefix = extractPrefix(a);
  auto bPrefix = extractPrefix(b);

  assert(aPrefix.has_value());
  assert(bPrefix.has_value());

  auto aTokens = tokenizeDate(aPrefix.value());
  auto bTokens = tokenizeDate(bPrefix.value());

  auto aDate = interpretTokens(aTokens);
  auto bDate = interpretTokens(aTokens);

  if (!aDate && !bDate) {
    return true;
  }
  if (!aDate) {
    return true;
  }
  if (!bDate) {
    return false;
  }

  return aDate < bDate;
}

  template <>
bool
CustomComparator::operator()(std::filesystem::path const &a,
                             std::filesystem::path const &b)
{
  assert(std::filesystem::exists(a) && "File doesn't exist.");
  assert(std::filesystem::exists(b) && "File doesn't exist.");

  auto filenameA = a.filename().string();
  auto filenameB = b.filename().string();

  return operator()(filenameA, filenameB);
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
    return std::chrono::year_month_day{year, month, day};
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
