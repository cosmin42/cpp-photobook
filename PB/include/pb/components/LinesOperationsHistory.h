#pragma once

#include <variant>
#include <vector>

#include <gtest/gtest.h>

#include <pb/infra/Traits.h>

namespace PB {

struct AddOperation {
  unsigned index;
  unsigned size;

  std::string toString() const
  {
    return "AddOperation{index=" + std::to_string(index) +
           ", size=" + std::to_string(size) + "}";
  }
};

struct RemoveOperation {
  unsigned index;
  unsigned size;

  std::string toString() const
  {
    return "RemoveOperation{index=" + std::to_string(index) +
           ", size=" + std::to_string(size) + "}";
  }
};

class LinesOperationsHistory final {
public:
  LinesOperationsHistory() = default;
  ~LinesOperationsHistory() = default;

  void registerAddOperation(unsigned index, unsigned size);

  void registerRemoveOperation(unsigned index, unsigned size);

  std::optional<unsigned> updateIndex(unsigned oldVersion, unsigned newVersion,
                                      unsigned oldIndex);

  std::string toString() const;

private:
  unsigned                add(unsigned oldIndex, unsigned index, unsigned size);
  std::optional<unsigned> remove(unsigned oldIndex, unsigned index,
                                 unsigned size);

  std::vector<std::variant<AddOperation, RemoveOperation>> mRegister;

  FRIEND_TEST(TestLinesOperationsHistory, Test0);
};
} // namespace PB
