#include <pb/FileMapper.h>

#include <queue>

#include <pb/Config.h>
#include <pb/FileComparator.h>

namespace PB {

auto mapImages(std::filesystem::path const &root)
    -> std::vector<std::filesystem::path>
{
  assert(std::filesystem::exists(root) && "The root folder is missing");

  std::filesystem::path rootPath(root);

  std::vector<std::filesystem::path> allPaths;
  std::queue<std::filesystem::path>  pathsQueue;
  pathsQueue.push(root);

  while (!pathsQueue.empty()) {
    auto currentPath = pathsQueue.front();
    pathsQueue.pop();

    allPaths.push_back(currentPath);
    if (std::filesystem::is_directory(currentPath)) {
      for (const auto &entry :
           std::filesystem::directory_iterator(currentPath)) {
        if (std::filesystem::is_directory(entry) ||
            std::filesystem::is_regular_file(entry)) {
          pathsQueue.push(entry);
        }
        else {
          printDebug("Skipped invalid file");
        }
      }
    }
  }

  std::sort(allPaths.begin(), allPaths.end(), CustomComparator());

  return allPaths;
}

} // namespace PB