#include <FileComparator.h>
#include <array>
#include <cassert>
#include <filesystem>
#include <ranges>
#include <regex>

#include <util/Traits.h>

namespace PB {

const std::string CustomComparator::prefixRegex =
    "^([0-9]+\\.)?([0-9]+\\.)?[0-9]+\\.";

template <>
auto CustomComparator::operator()(std::string const &a, std::string const &b)
    -> bool
{
  auto parsedA = PB::compose(PB::CustomComparator::interpretTokens,
                             PB::CustomComparator::tokenizeDate,
                             PB::CustomComparator::extractPrefix)(a);

  auto parsedB = PB::compose(PB::CustomComparator::interpretTokens,
                             PB::CustomComparator::tokenizeDate,
                             PB::CustomComparator::extractPrefix)(b);

  if (!parsedA && !parsedB) {
    return a < b;
  }
  else if (!parsedA) {
    return false;
  }
  else if (!parsedB) {
    return true;
  }
  return *parsedA < *parsedB;
}

template <>
auto CustomComparator::operator()(std::filesystem::path const &a,
                                  std::filesystem::path const &b) -> bool
{
  assert(std::filesystem::exists(a) && "File doesn't exist.");
  assert(std::filesystem::exists(b) && "File doesn't exist.");

  auto filenameA = a.filename().string();
  auto filenameB = b.filename().string();

  return operator()(filenameA, filenameB);
}

auto CustomComparator::extractPrefix(const std::string &input)
    -> std::optional<std::string>
{
  std::regex  regexPattern(prefixRegex);
  std::smatch match;

  if (std::regex_search(input, match, regexPattern)) {
    std::string result = match.str();
    assert(result.length() > 0);

    // Remove the last "."
    result.pop_back();
    return result;
  }
  else {
    return std::nullopt;
  }
}

auto CustomComparator::tokenizeDate(std::optional<std::string> blob)
    -> std::stack<std::string>
{
  if (!blob) {
    std::stack<std::string>();
  }
  auto extractStr = *blob;
  auto tokensRanges = blob.value() | std::views::split('.');

  int                     count = 0;
  std::stack<std::string> tokensQueue;

  for (const auto &tokenRange : tokensRanges) {
    assert(count < DAY_MONTH_YEAR_COUNT);
    tokensQueue.push(std::string(tokenRange.begin(), tokenRange.end()));
    count++;
  }

  return tokensQueue;
}

auto CustomComparator::interpretTokens(std::stack<std::string> tokens)
    -> std::optional<std::chrono::year_month_day>
{
  if (tokens.empty()) {
    return std::nullopt;
  }

  std::chrono::day   day{0};
  std::chrono::month month{0};
  std::chrono::year  year{0};

  auto &yearStr = tokens.top();
  year = interpretToken<std::chrono::year>(yearStr);
  tokens.pop();
  if (tokens.empty()) {
    return std::chrono::year_month_day{year, month, day};
  }

  auto &monthStr = tokens.top();
  month = interpretToken<std::chrono::month>(monthStr);
  tokens.pop();
  if (tokens.empty()) {
    return std::chrono::year_month_day{year, month, day};
  }

  auto &dayStr = tokens.top();
  day = interpretToken<std::chrono::day>(dayStr);
  tokens.pop();

  return std::chrono::year_month_day{year, month, day};
}

} // namespace PB
