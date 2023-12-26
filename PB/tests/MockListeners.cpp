#include "MockListeners.h"

void clearProjectCache()
{
  std::regex uuidRegex = std::regex(
      "[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");

  std::filesystem::remove("database.db");

  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    if (std::regex_match(entry.path().stem().string(), uuidRegex)) {
      std::filesystem::remove(entry.path());
    }
  }
}
