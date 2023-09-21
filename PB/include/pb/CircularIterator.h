#pragma once

#include <optional>
#include <vector>

#include <pb/common/Log.h>
#include <pb/util/Traits.h>

namespace PB {

class CircularIterator final {
public:
  CircularIterator()
  {
  }

  explicit CircularIterator(std::vector<Path> &container)
      : mBeginIterator(container.begin()), mEndIterator(container.end()),
        mSize((unsigned)container.size())
  {
  }

  CircularIterator(CircularIterator const &other)
      : mBeginIterator(other.mBeginIterator), mEndIterator(other.mEndIterator),
        mIndex(other.mIndex), mSize(other.mSize)
  {
  }

  CircularIterator(CircularIterator &&other) noexcept
      : mBeginIterator(other.mBeginIterator), mEndIterator(other.mEndIterator),
        mIndex(other.mIndex), mSize(other.mSize)
  {
  }

  CircularIterator &operator=(CircularIterator const &other)
  {
    mBeginIterator = other.mBeginIterator;
    mEndIterator = other.mEndIterator;
    mIndex = other.mIndex;
    mSize = other.mSize;
    return *this;
  }

  ~CircularIterator() = default;

  auto current() -> std::optional<Path>
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

  auto size() const -> unsigned { return mSize; }

  auto valid() const -> bool { return mSize > 0; }

private:
  std::vector<Path>::iterator mBeginIterator;
  std::vector<Path>::iterator mEndIterator;
  unsigned                    mIndex = 0;
  unsigned                    mSize = 0;
};
} // namespace PB