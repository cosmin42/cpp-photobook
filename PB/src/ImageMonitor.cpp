#include <pb/ImageMonitor.h>

#include <boost/bimap/support/lambda.hpp>

namespace PB {
void ImageMonitor::setListener(ImageMonitorListener *listener)
{
  mListener = listener;
}

void ImageMonitor::addRow(Path                                       path,
                          std::vector<std::shared_ptr<VirtualImage>> images)
{
  if (mRowIndexes.left.find(path) != mRowIndexes.left.end()) {
    return;
  }
  mRowIndexes.insert({path, (int)mRowIndexes.size()});

  mUnstagedImagesMatrix.push_back(std::vector<std::shared_ptr<VirtualImage>>());

  for (auto i = 0; i < images.size(); ++i) {
    mPositions.insert({images.at(i)->resources().full,
                       {(int)mUnstagedImagesMatrix.size() - 1, (int)i}});
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1)
        .push_back(images.at(i));
  }

  mListener->onImportFolderAdded();

  mPendingRows.insert((int)mRowIndexes.size() - 1);
}

void ImageMonitor::removeRow(int row)
{
  PBDev::basicAssert(!mPendingRows.contains(row));

  for (auto i = 0; i < mUnstagedImagesMatrix.at(row).size(); ++i) {
    mPositions.right.erase(std::pair<int, int>{row, i});
  }
  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + row);
  mRowIndexes.right.erase(row);

  for (int i = row + 1; i < (int)mUnstagedImagesMatrix.size() + 1; ++i) {
    for (int index = 0; index < (int)mUnstagedImagesMatrix.at(i - 1).size();
         ++index) {
      auto it = mPositions.right.find(std::pair<int, int>(i, index));
      mPositions.right.modify_key(it, boost::bimaps::_key =
                                          std::pair<int, int>(i - 1, index));
    }
  }

  for (int i = row + 1; i < mRowIndexes.size() + 1; ++i) {
    mRowIndexes.right.replace_key(mRowIndexes.right.find(i), i - 1);
  }

  mListener->onImportFolderRemoved(row);
}

void ImageMonitor::removeRow(Path path)
{
  int index = mRowIndexes.left.at(path);
  PBDev::basicAssert(!mPendingRows.contains(index));
  removeRow(index);

  mListener->onImportFolderRemoved(index);
}

void ImageMonitor::clear()
{
  PBDev::basicAssert(mPendingRows.empty());
  mRowIndexes.clear();
  mPositions.clear();
  mUnstagedImagesMatrix.clear();

  mListener->onCleared();
}

void ImageMonitor::completeRow(int index) { mPendingRows.erase(index); }

bool ImageMonitor::isPending(Path path) const
{
  return isPending(mRowIndexes.left.at(path));
}

bool ImageMonitor::isPending(int index) const
{
  return mPendingRows.contains(index);
}

unsigned ImageMonitor::importListSize() const
{
  return (unsigned)mRowIndexes.size();
}

unsigned ImageMonitor::rowSize(unsigned row)
{
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
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
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
  PBDev::basicAssert(index < mUnstagedImagesMatrix.at(row).size());

  return mUnstagedImagesMatrix.at(row).at(index);
}

std::shared_ptr<VirtualImage> ImageMonitor::image(Path full) const
{
  PBDev::basicAssert(mPositions.left.find(full) != mPositions.left.end());

  auto &[row, index] = mPositions.left.at(full);

  return mUnstagedImagesMatrix.at(row).at(index);
}

std::pair<int, int> ImageMonitor::position(Path full) const
{
  PBDev::basicAssert(mPositions.left.find(full) != mPositions.left.end());
  return mPositions.left.at(full);
}

auto ImageMonitor::statefulIterator(Path root)
    -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>
{
  return statefulIterator(mRowIndexes.left.at(root));
}

auto ImageMonitor::statefulIterator(unsigned row)
    -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>
{
  if (row >= mUnstagedImagesMatrix.size()) {
    return PBDev::IteratorWithState<
        std::vector<std::shared_ptr<VirtualImage>>>();
  }
  return PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>(
      mUnstagedImagesMatrix.at(row));
}

void ImageMonitor::log() const
{
  PB::printDebug("mRowIndexes\n");
  for (auto it = mRowIndexes.begin(); it != mRowIndexes.end(); ++it) {
    PB::printDebug("(%s %d)\n", it->left.stem().string().c_str(), it->right);
  }
  PB::printDebug("mPositions\n");
  for (auto it = mPositions.begin(); it != mPositions.end(); ++it) {
    PB::printDebug("(%s [%d %d])\n", it->left.stem().string().c_str(),
                   it->right.first, it->right.second);
  }
}

} // namespace PB