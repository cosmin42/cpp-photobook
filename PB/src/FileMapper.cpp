#include <FileMapper.h>

#include <queue>

#include <Config.h>
#include <FileComparator.h>

namespace PB {

FilesMap::FilesMap(const std::string &rootDirectory)
    : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

FilesMap::FilesMap(std::string &&rootDirectory) : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

auto FilesMap::map() const -> std::vector<std::filesystem::path>
{
  assert(std::filesystem::exists(mRootDirectory) &&
         "The root folder is missing");

  std::filesystem::path rootPath(mRootDirectory);

  std::vector<std::filesystem::path> allPaths;
  std::queue<std::filesystem::path>  pathsQueue;
  pathsQueue.push(mRootDirectory);

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
        else
        {
          printDebug("Skipped invalid file");
        }
      }
    }
  }

  std::sort(allPaths.begin(), allPaths.end(), CustomComparator());

  return allPaths;
}

} // namespace PB