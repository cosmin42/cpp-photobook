#include <FileComparator.h>
#include <array>
#include <cassert>
#include <filesystem>
#include <ranges>
#include <regex>

namespace PB {

const std::string CustomComparator::prefixRegex =
    "^([0-9]+\\.)?([0-9]+\\.)?[0-9]+\\.";

template <>
auto CustomComparator::operator()(std::string const &a, std::string const &b)
    -> bool
{
  auto aPrefix = extractPrefix(a);
  auto bPrefix = extractPrefix(b);

  if (!aPrefix && !bPrefix) {
    return a < b;
  }
  else if (!aPrefix) {
    return false;
  }
  else if (!bPrefix) {
    return true;
  }

  auto aTokens = tokenizeDate(aPrefix.value());
  auto bTokens = tokenizeDate(bPrefix.value());

  auto aDate = interpretTokens(aTokens);
  auto bDate = interpretTokens(bTokens);

  // Alphabetical order
  if (!aDate && !bDate) {
    return a < b;
  }
  if (!aDate) {
    return false;
  }
  if (!bDate) {
    return true;
  }

  if (aDate.value().year() != bDate.value().year()) {
    return aDate.value().year() < bDate.value().year();
  }

  if (aDate.value().month() != bDate.value().month()) {
    return aDate.value().month() < bDate.value().month();
  }

  if (aDate.value().day() != bDate.value().day()) {
    return aDate.value().day() < bDate.value().day();
  }

  return aDate.value() < bDate.value();
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

auto CustomComparator::tokenizeDate(std::string const &blob)
    -> std::stack<std::string>
{
  auto tokensRanges = blob | std::views::split('.');

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
