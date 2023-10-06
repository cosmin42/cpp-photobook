#pragma once

#include <assert.h>
#include <functional>
#include <optional>
#include <vector>

#include <pb/common/Log.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename Container> class CircularIterator final {
public:
  explicit CircularIterator(
      Container                                          &container,
      std::function<bool(typename Container::value_type)> filter)
      : mBeginIterator(container.begin())
  {
    for (int i = 0; i < container.size(); ++i) {
      if (filter(container.at(i))) {
        mFilteredIndices.push_back(i);
      }
    }
  }

  auto current() -> std::optional<typename Container::value_type>
  {
    if (mFilteredIndices.size() == 0) {
      return std::nullopt;
    }
    auto tmpIterator = mBeginIterator;
    std::advance(tmpIterator, mIndex);
    return *tmpIterator;
  }

  CircularIterator &next()
  {
    if (mFilteredIndices.size() == 0) {
      return *this;
    }
    mIndex++;
    mIndex %= mFilteredIndices.size();
    return *this;
  }

  CircularIterator &previous()
  {
    if (mFilteredIndices.size() == 0) {
      return *this;
    }
    if (mIndex == 0) {
      mIndex = (unsigned)mFilteredIndices.size();
    }
    mIndex--;
    return *this;
  }

  CircularIterator &goToPosition(int index)
  {
    if (mFilteredIndices.size() == 0) {
      return *this;
    }
    assert(index < (int)mFilteredIndices.size());
    mIndex = index;
    return *this;
  }

  auto size() const -> unsigned { return (unsigned)mFilteredIndices.size(); }

  auto valid() const -> bool { return (unsigned)mFilteredIndices.size() > 0; }

private:
  Container::iterator mBeginIterator;
  unsigned            mIndex = 0;
  std::vector<int>    mFilteredIndices;
};
} // namespace PB