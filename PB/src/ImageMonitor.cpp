#include <pb/ImageMonitor.h>

namespace PB {
void ImageMonitor::setListener(std::shared_ptr<ImageMonitorListener> listener)
{
  mListener = listener;
}

void ImageMonitor::addRow(Path                                       path,
                          std::vector<std::shared_ptr<VirtualImage>> images)
{
  mRowIndexes.insert({path, (int)mRowIndexes.size()});

  mUnstagedImagesMatrix.push_back(std::vector<std::shared_ptr<VirtualImage>>());

  for (auto i = 0; i < images.size(); ++i) {
    mPositions.insert({images.at(i)->resources().full,
                       {(int)mUnstagedImagesMatrix.size(), (int)i}});
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1)
        .push_back(images.at(i));
  }

  mListener->onImportFolderAdded();
}

void ImageMonitor::removeRow(int index)
{
  basicAssert(!mPendingRows.contains(index));

  for (auto i = 0; i < mUnstagedImagesMatrix.at(index).size(); ++i) {
    mPositions.right.erase(std::pair<int, int>{index, i});
  }
  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + index);
  mRowIndexes.right.erase(index);
  for (int i = index + 1; i < mRowIndexes.size() + 1; ++i) {
    mRowIndexes.right.replace_key(mRowIndexes.right.find(i), i - 1);
  }

  mListener->onImportFolderRemoved(index);
}

void ImageMonitor::removeRow(Path path)
{
  int index = mRowIndexes.left.at(path);
  basicAssert(!mPendingRows.contains(index));
  removeRow(index);

  mListener->onImportFolderRemoved(index);
}

void ImageMonitor::clear()
{
  basicAssert(mPendingRows.empty());
  mRowIndexes.clear();
  mPositions.clear();
  mUnstagedImagesMatrix.clear();

  mListener->onCleared();
}

void ImageMonitor::completeRow(int index) { mPendingRows.erase(index); }

unsigned ImageMonitor::importListSize() const
{
  return (unsigned)mRowIndexes.size();
}

unsigned ImageMonitor::rowSize(unsigned row)
{
  basicAssert(row < mUnstagedImagesMatrix.size());
  return (unsigned)mUnstagedImagesMatrix.at(row).size();
}

unsigned ImageMonitor::rowIndex(Path path) const
{
  return mRowIndexes.left.at(path);
}

bool ImageMonitor::containsRow(Path path) const
{
  return mRowIndexes.left.find(path) != mRowIndexes.left.end();
}

std::vector<Path> ImageMonitor::rowList() const
{
  std::vector<Path> result;
  for (int i = 0; i < mRowIndexes.size(); ++i) {
    result.push_back(mRowIndexes.right.at(i));
  }
  return result;
}

Path ImageMonitor::rowPath(unsigned row) const
{
  return mRowIndexes.right.at(row);
}

std::shared_ptr<VirtualImage> ImageMonitor::image(unsigned row,
                                                  unsigned index) const
{
  basicAssert(row < mUnstagedImagesMatrix.size());
  basicAssert(index < mUnstagedImagesMatrix.at(row).size());

  return mUnstagedImagesMatrix.at(row).at(index);
}

auto ImageMonitor::statefulIterator(Path root)
    -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
{
  return statefulIterator(mRowIndexes.left.at(root));
}

auto ImageMonitor::statefulIterator(unsigned row)
    -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
{
  return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
      mUnstagedImagesMatrix.at(row));
}

} // namespace PB