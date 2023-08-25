#pragma once

#include <chrono>
#include <compare>
#include <optional>
#include <stack>
#include <string>

namespace PB {

struct CustomComparator {
  static constexpr int DAY_MONTH_YEAR_COUNT = 3;

  template <typename T> auto operator()(T const &, T const &) -> bool;

  static auto extractPrefix(std::string const &) -> std::optional<std::string>;

  static auto tokenizeDate(std::string const &blob) -> std::stack<std::string>;

  template <typename T> static auto interpretToken(std::string const &s) -> T
  {
    if (!s.empty() && T(std::stoi(s)).ok()) {
      return T(std::stoi(s));
    }
    return T{0};
  }

  static auto interpretTokens(std::stack<std::string> tokens)
      -> std::optional<std::chrono::year_month_day>;

private:
  static const std::string prefixRegex;
};
} // namespace PB