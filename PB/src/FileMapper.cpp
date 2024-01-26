#include <pb/tasks/FileMapper.h>

namespace PB {

MediaMapper::MediaMapper(Path const &root) : mRoot(root)
{
  mRecursiveIterator = std::filesystem::recursive_directory_iterator(
      root, std::filesystem::directory_options::skip_permission_denied);
}

void MediaMapper::taskStep()
{
  Path path = mRecursiveIterator->path();

#ifdef SIMULATE_SLOW_MAPPER
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
  mSubFiles.push_back(path);
  mRecursiveIterator++;
}

bool MediaMapper::stoppingCondition() const
{
  return mRecursiveIterator == std::filesystem::end(mRecursiveIterator);
}

Path MediaMapper::root() const { return mRoot; }

std::vector<Path> MediaMapper::importedDirectories() const { return mSubFiles; }

int MediaMapper::stepsCount() const { return 0; }
} // namespace PB