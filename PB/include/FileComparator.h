#pragma once

#include <compare>
#include <string>

namespace PB {
struct CustomComparator {
  template <typename T> std::strong_ordering operator()(T const &, T const &);

  static std::optional<std::string> extractPrefix(std::string const&);

private:
  static const std::string prefixRegex;
};
} // namespace PB