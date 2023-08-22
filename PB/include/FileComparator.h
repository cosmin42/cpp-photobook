#pragma once

#include <compare>
#include <optional>
#include <string>

namespace PB {
struct CustomComparator {
  static constexpr int DAY_MONTH_YEAR_COUNT = 3;

  template <typename T> std::strong_ordering operator()(T const &, T const &);

  static std::optional<std::string> extractPrefix(std::string const &);

  static std::array<std::string, DAY_MONTH_YEAR_COUNT>
  tokenizeDate(std::string const &blob);

private:
  static const std::string prefixRegex;
};
} // namespace PB