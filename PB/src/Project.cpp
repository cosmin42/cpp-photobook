#include <pb/project/Project.h>

namespace PB {
std::string
generateAlbumName(std::function<bool(std::string)> stoppingCondition)
{
  auto size = Context::HAPPY_WORDS.size();
  std::srand(static_cast<unsigned int>(std::time(0)));
  int randomIndex = std::rand() % size;
  if (!stoppingCondition) {
    return Context::HAPPY_WORDS.at(randomIndex);
  }

  std::string prefix = Context::HAPPY_WORDS.at(randomIndex);
  int         index = 0;
  std::string name = prefix;

  while (!stoppingCondition(name)) {
    name = prefix + std::to_string(index);
    index++;
    PBDev::basicAssert(index < 1000);
  }
  return name;
}

} // namespace PB