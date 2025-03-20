#pragma once

#include <variant>
#include <vector>

#include <pb/infra/Traits.h>

namespace PB {

struct AddOperation {
  unsigned index;
  unsigned size;
};

struct RemoveOperation {
  unsigned index;
  unsigned size;
};

class LinesOperationsHistory {
public:
  LinesOperationsHistory() = default;
  ~LinesOperationsHistory() = default;

  unsigned registerAddOperation(unsigned index, unsigned size);

  unsigned registerRemoveOperation(unsigned index, unsigned size);

  unsigned updateIndex(unsigned oldVersion, unsigned newVersion,
                       unsigned oldIndex);

  std::string toString() const;

private:
  unsigned                add(unsigned oldIndex, unsigned index, unsigned size);
  std::optional<unsigned> remove(unsigned oldIndex, unsigned index,
                                 unsigned size);

  std::vector<std::variant<AddOperation, RemoveOperation>> mRegister;
};
} // namespace PB
