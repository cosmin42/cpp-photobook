#pragma once

namespace PB {
struct CustomComparator {

  template <typename T> std::strong_ordering operator()(T const &, T const &);
};
} // namespace PB