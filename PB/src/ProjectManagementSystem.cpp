#include <pb/ProjectManagementSystem.h>

namespace PB {
#ifdef SIMULATE_FEW_HAPPY_WORDS
std::vector<std::string> ProjectManagementSystem::HAPPY_WORDS = {"Joyful",
                                                                 "Blissful"};
#else
std::vector<std::string> ProjectManagementSystem::HAPPY_WORDS = {
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

std::string ProjectManagementSystem::newAlbumName()
{
  auto size = HAPPY_WORDS.size();
  std::srand(static_cast<unsigned int>(std::time(0)));
  int         randomIndex = std::rand() % size;
  std::string prefix = HAPPY_WORDS.at(randomIndex);
  int         index = 0;
  std::string name = prefix;

  while (hasProjectName(name)) {
    name = prefix + std::to_string(index);
    index++;
    PBDev::basicAssert(index < 1000);
  }
  return name;
}

bool ProjectManagementSystem::hasProjectName(std::string name) const
{
  return mProjectsMetadata.right.find(name) != mProjectsMetadata.right.end();

} // namespace PB
