#pragma once

#include <pb/util/Concepts.h>

namespace PB {
template <typename T, typename Content>
  requires RandomAccessibleConcept<T, Content>
class CircularIterator final {
public:
  explicit CircularIterator(T &container) : mContainer(container) {}

  CircularIterator(CircularIterator const &other)
      : mContainer(other.mContainer), mIndex(other.mIndex)
  {
  }

  CircularIterator(CircularIterator &&other)
      : mContainer(other.mContainer), mIndex(other.mIndex)
  {
  }

  CircularIterator &operator=(CircularIterator const &other)
  {
    mIndex = other.mIndex;
    mContainer = other.mContainer;
    return *this;
  }

  ~CircularIterator() = default;

  auto current() const -> Content { return mContainer.access(mIndex); }

  CircularIterator &next()
  {
    if (mContainer.size() == 0) {
      return *this;
    }
    mIndex++;
    mIndex %= mContainer.size();
    return *this;
  }

  CircularIterator &previous()
  {
    if (mContainer.size() == 0) {
      return *this;
    }
    if (mIndex == 0) {
      mIndex = mContainer.size();
    }
    mIndex--;
    return *this;
  }

private:
  T       &mContainer;
  unsigned mIndex = 0;
};
} // namespace PB