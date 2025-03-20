#include <pb/components/LinesOperationsHistory.h>

namespace PB {
unsigned LinesOperationsHistory::registerAddOperation(unsigned index,
                                                      unsigned size)
{
  AddOperation addOperation = {index, size};
  mRegister.push_back(addOperation);
  return (unsigned)mRegister.size();
}

unsigned LinesOperationsHistory::registerRemoveOperation(unsigned index,
                                                         unsigned size)
{
  RemoveOperation removeOperation = {index, size};
  mRegister.push_back(removeOperation);
  return (unsigned)mRegister.size();
}

unsigned LinesOperationsHistory::add(unsigned oldIndex, unsigned index,
                                     unsigned size)
{
  if (oldIndex < index) {
    return oldIndex;
  }

  PBDev::basicAssert(std::numeric_limits<unsigned>::max() - size > oldIndex);

  return oldIndex + size;
}

std::optional<unsigned>
LinesOperationsHistory::remove(unsigned oldIndex, unsigned index, unsigned size)
{
  if (oldIndex < index) {
    return oldIndex;
  }
  PBDev::basicAssert(std::numeric_limits<unsigned>::max() - size > oldIndex);

  if (oldIndex < index + size) {
    return index;
  }
  PBDev::basicAssert(oldIndex > size);
  return oldIndex - size;
}

unsigned LinesOperationsHistory::updateIndex(unsigned oldVersion,
                                             unsigned newVersion,
                                             unsigned oldIndex)
{
  unsigned updatedIndex = oldIndex;
  
  for (auto i = oldVersion; i < newVersion; ++i) {
    if (auto addOperation = std::get_if<AddOperation>(&mRegister[i])) {
      updatedIndex = add(updatedIndex, addOperation->index, addOperation->size);
    }
    else if (auto removeOperation =
                 std::get_if<RemoveOperation>(&mRegister[i])) {
      if (auto newIndex = remove(updatedIndex, removeOperation->index,
                                 removeOperation->size)) {
        updatedIndex = *newIndex;
      }
      else
      {
        PBDev::basicAssert(false);
      }
    }
  }

  return updatedIndex;
}

std::string LinesOperationsHistory::toString() const
{
  std::string result;
  for (const auto &operation : mRegister) {
    if (auto addOperation = std::get_if<AddOperation>(&operation)) {
      result += "Add(" + std::to_string(addOperation->index) + ", " +
                std::to_string(addOperation->size) + ")\n";
    }
    else if (auto removeOperation = std::get_if<RemoveOperation>(&operation)) {
      result += "Remove(" + std::to_string(removeOperation->index) + ", " +
                std::to_string(removeOperation->size) + ")\n";
    }
  }
  return result;
}

} // namespace PB
