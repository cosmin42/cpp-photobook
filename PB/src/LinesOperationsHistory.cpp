#include <pb/components/LinesOperationsHistory.h>

namespace PB {
void LinesOperationsHistory::registerAddOperation(unsigned index, unsigned size)
{
  AddOperation addOperation = {index, size};
  mRegister.push_back(addOperation);
}

void LinesOperationsHistory::registerRemoveOperation(unsigned index,
                                                     unsigned size)
{
  RemoveOperation removeOperation = {index, size};
  mRegister.push_back(removeOperation);
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

  if (oldIndex <= index + size) {
    return std::nullopt;
  }
  PBDev::basicAssert(oldIndex > size);
  return oldIndex - size;
}

std::optional<unsigned> LinesOperationsHistory::updateIndex(unsigned oldVersion,
                                                            unsigned newVersion,
                                                            unsigned oldIndex)
{
  unsigned updatedIndex = oldIndex;
  //std::cout << "Update index" << oldIndex << std::endl;
  for (auto i = oldVersion; i < newVersion; ++i) {
    if (auto addOperation = std::get_if<AddOperation>(&mRegister[i])) {
      //std::cout << "addOperation: " << addOperation->toString() << std::endl;
      updatedIndex = add(updatedIndex, addOperation->index, addOperation->size);
    }
    else if (auto removeOperation =
                 std::get_if<RemoveOperation>(&mRegister[i])) {

      if (auto newIndex = remove(updatedIndex, removeOperation->index,
                                 removeOperation->size)) {
        //std::cout << "removeOperation: " << removeOperation->toString()
        //          << std::endl;
        updatedIndex = *newIndex;
      }
      else {
        //std::cout << "nullopt" << std::endl;
        return std::nullopt;
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
