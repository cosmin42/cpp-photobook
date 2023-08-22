#pragma once

#include <chrono>
#include <compare>
#include <optional>
#include <queue>
#include <string>

namespace PB {
struct CustomComparator {
  static constexpr int DAY_MONTH_YEAR_COUNT = 3;

  template <typename T> std::strong_ordering operator()(T const &, T const &);

  static std::optional<std::string> extractPrefix(std::string const &);

  static std::queue<std::string>
  tokenizeDate(std::string const &blob);

  template <typename T>
  static T interpretToken(std::string const& s)
  {
    if (!s.empty() && T(std::stoi(s)).ok()) {
      return T(std::stoi(s));
    }
    return T{0};
  }

  static std::optional<std::chrono::year_month_day>
  interpretTokens(std::queue<std::string> tokens);

private:
  static const std::string prefixRegex;
};
} // namespace PB