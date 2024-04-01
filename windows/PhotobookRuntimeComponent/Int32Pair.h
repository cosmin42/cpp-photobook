#pragma once

#include "Int32Pair.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

struct Int32Pair : Int32PairT<Int32Pair> {
  Int32Pair() {}
  explicit Int32Pair(int first, int second) : mPair(first, second) {}
  ~Int32Pair() = default;

  int First() { return mPair.first; }
  int Second() { return mPair.second; }

  void First(int value) { mPair.first = value; }
  void Second(int value) { mPair.second = value; }

  std::pair<int, int> unwrap() { return mPair; }

private:
  std::pair<int, int> mPair;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct Int32Pair : Int32PairT<Int32Pair, implementation::Int32Pair> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
