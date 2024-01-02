#include <pb/tasks/FileMapper.h>

namespace PB {
MediaMapper::MediaMapper(std::filesystem::path const &root) : mRoot(root)
{
  mRecursiveIterator = std::filesystem::recursive_directory_iterator(
      root, std::filesystem::directory_options::skip_permission_denied);
}

void MediaMapper::taskStep()
{
  Path path = mRecursiveIterator->path();
  mSubFiles.push_back(path);
  mRecursiveIterator++;
}

bool MediaMapper::stoppingCondition()
{
  return mRecursiveIterator == std::filesystem::end(mRecursiveIterator);
}

Path MediaMapper::root() const { return mRoot; }

std::vector<Path> MediaMapper::importedDirectories() const { return mSubFiles; }
} // namespace PB