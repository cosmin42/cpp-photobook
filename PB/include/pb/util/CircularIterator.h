#pragma once

#include <assert.h>
#include <functional>
#include <optional>
#include <vector>

#include <pb/config/Log.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename Container> class IteratorWithState final {
public:
  explicit IteratorWithState(Container &container)
      : mBeginIterator(container.begin())
  {
    mSize = (unsigned)container.size();
  }

  IteratorWithState() = default;
  ~IteratorWithState() = default;

  auto current() const -> typename Container::value_type
  {
    PB::basicAssert(mSize > 0);
    auto tmpIterator = mBeginIterator;
    std::advance(tmpIterator, mIndex);
    return *tmpIterator;
  }

  IteratorWithState &operator[](int index)
  {
    if (mSize == 0) {
      return *this;
    }
    index %= mSize;
    mIndex = index;
    return *this;
  }

  auto size() const -> unsigned { return mSize; }

  auto valid() const -> bool { return mSize > 0; }

private:
  Container::iterator mBeginIterator;
  unsigned            mIndex = 0;
  unsigned            mSize = 0;
};
} // namespace PB