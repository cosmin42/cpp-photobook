#include <pb/FileMapper.h>

#include <queue>

#include <pb/Config.h>
#include <pb/FileComparator.h>

namespace PB {

MediaMapper::MediaMapper(std::filesystem::path const &root,
                         GradualControllableListener &listener)
    : Thread(Context::inst().sStopSource.get_token()), mListener(listener)
{
  printDebug("MediaMapper constructor.\n");
  mFinish = [this]() { mListener.doFinish(); };
  mRecursiveIterator = std::filesystem::recursive_directory_iterator(
      root, std::filesystem::directory_options::skip_permission_denied);
}

void MediaMapper::executeSingleTask()
{
  if (mRecursiveIterator == std::filesystem::end(mRecursiveIterator)) {
    stop();
  }
  else {
    auto path = mRecursiveIterator->path();
    if (std::filesystem::is_directory(path) ||
        std::filesystem::is_regular_file(path)) {
      mPaths.push_back(path);
    }
  }
}

auto MediaMapper::paths() -> std::vector<std::filesystem::path> &
{
  return mPaths;
}

} // namespace PB