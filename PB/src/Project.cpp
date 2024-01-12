#include <pb/project/Project.h>

namespace PB {

Project::Project(ProjectSnapshot snapshot) { mActive = mCache = snapshot; }

#ifdef SIMULATE_FEW_HAPPY_WORDS
std::vector<std::string> Project::HAPPY_WORDS = {"Joyful", "Blissful"};
#else
std::vector<std::string> Project::HAPPY_WORDS = {
    "Joyful",       "Blissful",   "Radiant",       "Cheerful",
    "Exuberant",    "Jubilant",   "Gleeful",       "Delighted",
    "Content",      "Ecstatic",   "Merry",         "Jovial",
    "Upbeat",       "Elated",     "Happy",         "Sunny",
    "Lighthearted", "Buoyant",    "Festive",       "Vibrant",
    "Grateful",     "Satisfied",  "Pleased",       "Overjoyed",
    "Optimistic",   "Pleased",    "Thrilled",      "Euphoric",
    "Playful",      "Carefree",   "Radiant",       "Wonderful",
    "Jolly",        "Zestful",    "Bubbly",        "Heartwarming",
    "Cheery",       "Upbeat",     "Chirpy",        "Sanguine",
    "Enthusiastic", "Serene",     "Up-lifting",    "Giddy",
    "Gleaming",     "Bountiful",  "Exultant",      "Merry-making",
    "Comical",      "Hilarious",  "Dynamic",       "Sweet",
    "Whimsical",    "Spirited",   "Sprightly",     "Delightful",
    "Radiant",      "Uplifting",  "Optimistic",    "Tickled",
    "Chipper",      "Pleasant",   "Buoyant",       "Lively",
    "Sparkling",    "Tickled",    "Exhilarating",  "Bouncy",
    "Festal",       "Vivacious",  "Gladsome",      "Pleased",
    "Elated",       "Blithesome", "Merry-hearted", "Gleamy",
    "Zippy",        "Snappy",     "Piquant",       "Buoyant",
    "Enlivened",    "Beaming",    "Radiant",       "Jocund",
    "Grinning",     "Giggly",     "Sunny",         "Uplifted",
    "Hopeful",      "Amused",     "Merry-making",  "Overjoyed",
    "Glad",         "Contented",  "Radiant",       "Effervescent",
    "Spry",         "Snug",       "Jovial",        "Untroubled"};
#endif

// TODO: move this to ProjectPersistence
std::string
Project::generateAlbumName(std::function<bool(std::string)> stoppingCondition)
{
  auto size = HAPPY_WORDS.size();
  std::srand(static_cast<unsigned int>(std::time(0)));
  int randomIndex = std::rand() % size;
  if (!stoppingCondition) {
    return HAPPY_WORDS.at(randomIndex);
  }

  std::string prefix = HAPPY_WORDS.at(randomIndex);
  int         index = 0;
  std::string name = prefix;

  while (!stoppingCondition(name)) {
    name = prefix + std::to_string(index);
    index++;
    PBDev::basicAssert(index < 1000);
  }
  return name;
}

bool Project::isSynced() { return mActive.operator==(mCache); }

} // namespace PB