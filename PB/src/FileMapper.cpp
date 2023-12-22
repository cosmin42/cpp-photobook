#include <pb/FileMapper.h>

namespace PB {
MediaMapper::MediaMapper(std::filesystem::path const &root)
    : PBDev::SequentialTaskConsumer(Context::inst().sStopSource.get_token()),
      mRoot(root)
{
  mRecursiveIterator = std::filesystem::recursive_directory_iterator(
      root, std::filesystem::directory_options::skip_permission_denied);
}

void MediaMapper::notify()
{
  for (auto observer : mObservers) {
    observer->update(*this);
  }
}

void MediaMapper::executeSingleTask()
{
  if (mState != MediaMapState::Started) {
    mState = MediaMapState::Started;
    notify();
  }

  Path path = mRecursiveIterator->path();
  mSubFiles.push_back(path);
  mRecursiveIterator++;
}

void MediaMapper::finish()
{
  mState = MediaMapState::Finished;
  notify();
  mState = MediaMapState::None;
}

void MediaMapper::aborted()
{
  mState = MediaMapState::Aborted;
  notify();
  mState = MediaMapState::None;
}

bool MediaMapper::stoppingCondition()
{
  return mRecursiveIterator == std::filesystem::end(mRecursiveIterator);
}

MediaMapState MediaMapper::state() const { return mState; }

Path MediaMapper::root() const { return mRoot; }

std::vector<Path> MediaMapper::importedDirectories() const { return mSubFiles; }
} // namespace PB