#include <pb/ImageMonitor.h>

namespace PB {
void ImageMonitor::setListener(std::shared_ptr<ImageMonitorListener> listener)
{
  mListener = listener;
}

void ImageMonitor::addGroup(Path                                       path,
                            std::vector<std::shared_ptr<VirtualImage>> images)
{
  mGroupIndexes.insert({path, (int)mGroupIndexes.size()});

  mUnstagedImagesMatrix.push_back(std::vector<std::shared_ptr<VirtualImage>>());

  for (auto i = 0; i < images.size(); ++i) {
    mPositions.insert({images.at(i)->resources().full,
                       {(int)mUnstagedImagesMatrix.size(), (int)i}});
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1)
        .push_back(images.at(i));
  }
}

void ImageMonitor::removeGroup(int index)
{
  basicAssert(!mPendingGroups.contains(index));

  for (auto i = 0; i < mUnstagedImagesMatrix.at(index).size(); ++i) {
    mPositions.right.erase(std::pair<int, int>{index, i});
  }
  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + index);
  mGroupIndexes.right.erase(index);
  for (int i = index + 1; i < mGroupIndexes.size() + 1; ++i) {
    mGroupIndexes.right.replace_key(mGroupIndexes.right.find(i), i - 1);
  }
}

void ImageMonitor::removeGroup(Path path)
{
  int index = mGroupIndexes.left.at(path);
  basicAssert(!mPendingGroups.contains(index));
  removeGroup(index);
}

void ImageMonitor::clear()
{
  basicAssert(mPendingGroups.empty());
  mGroupIndexes.clear();
  mPositions.clear();
  mUnstagedImagesMatrix.clear();
}

void ImageMonitor::completeGroup(int index) { mPendingGroups.erase(index); }

} // namespace PB