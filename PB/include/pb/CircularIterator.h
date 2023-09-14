#pragma once

#include <pb/util/Concepts.h>

namespace PB {

class CircularIterator final {
public:
  CircularIterator() = default;

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

  CircularIterator(CircularIterator &&other)
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

  auto current() const -> std::optional<Path>
  {
    if (mSize == 0) {
      return std::nullopt;
    }
    return *(mBeginIterator + mIndex);
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

  auto size() -> unsigned { return mSize; }

  auto valid() const -> bool { return mSize > 0; }

private:
  std::vector<Path>::iterator mBeginIterator;
  std::vector<Path>::iterator mEndIterator;
  unsigned                    mIndex = 0;
  unsigned                    mSize = 0;
};
} // namespace PB