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
  explicit CircularIterator(Container &container)
      : mBeginIterator(container.begin())
  {
    mSize = (unsigned)container.size();
  }

  CircularIterator() = default;
  ~CircularIterator() = default;

  auto current() -> std::optional<typename Container::value_type>
  {
    if (mSize == 0) {
      return std::nullopt;
    }
    auto tmpIterator = mBeginIterator;
    std::advance(tmpIterator, mIndex);
    return *tmpIterator;
  }

  CircularIterator &next()
  {
    if (mSize == 0) {
      return *this;
    }
    mIndex++;
    mIndex %= mSize;
    return *this;
  }

  CircularIterator &previous()
  {
    if (mSize == 0) {
      return *this;
    }
    if (mIndex == 0) {
      mIndex = mSize;
    }
    mIndex--;
    return *this;
  }

  CircularIterator &operator[](int index)
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