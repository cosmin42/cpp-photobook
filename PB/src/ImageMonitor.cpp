#include <pb/ImageMonitor.h>

#include <boost/bimap/support/lambda.hpp>

namespace PB {
/*
void ImageMonitor::addRow(Path path, std::vector<GenericImagePtr> images)
{
  if (mRowIndexes.left.find(path) != mRowIndexes.left.end()) {
    return;
  }
  mRowIndexes.insert({path, (int)mRowIndexes.size()});

  mUnstagedImagesMatrix.push_back(std::vector<GenericImagePtr>());

  for (auto i = 0; i < images.size(); ++i) {
    // TODO: Investigate if hash can be used here instead of full
    mPositions.insert({images.at(i)->full(),
                       {(int)mUnstagedImagesMatrix.size() - 1, (int)i}});
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1)
        .push_back(images.at(i));
  }

  mPendingRows.insert((int)mRowIndexes.size() - 1);
  log();
}
*/
void ImageMonitor::addRow(
    Path path, std::unordered_map<PBDev::ImageId, GenericImagePtr> images)
{
  PBDev::basicAssert(mRowIndexes.left.find(path) == mRowIndexes.left.end());
  mRowIndexes.insert({path, (int)mRowIndexes.size()});

  mUnstagedImagesMatrix.push_back(std::vector<PBDev::ImageId>());

  for (auto &[id, image] : images) {
    mPositionsV2Reverse.emplace(std::pair<int, int>{(int)mRowIndexes.size(), 0},
                                id);
    mImages[id] = image;
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1).push_back(id);
  }

  mPendingRows.insert((int)mRowIndexes.size() - 1);
}
/*
void ImageMonitor::replaceImage(Path root, GenericImagePtr image, int index)
{
  auto row = mRowIndexes.left.at(root);
  if (index == -1) {
    for (auto i = 0; i < mUnstagedImagesMatrix.at(row).size(); ++i) {
      if (mUnstagedImagesMatrix.at(row).at(i)->hash().starts_with(
              "placeholder")) {
        mUnstagedImagesMatrix[row][i] = image;
        auto it = mPositions.right.find(std::pair<int, int>{row, i});
        mPositions.right.modify_data(it, boost::bimaps::_data = image->full());
        return;
      }
    }
    PBDev::basicAssert(false);
  }
  else {
    mUnstagedImagesMatrix[row][index] = image;
    mPositions.right.modify_data(
        mPositions.right.find(std::pair<int, int>{row, index}),
        boost::bimaps::_data = image->full());
  }
}
*/
void ImageMonitor::updateImage(PBDev::ImageId imageId, GenericImagePtr image)
{
  PBDev::basicAssert(mImages.find(imageId) != mImages.end());
  mImages[imageId] = image;
}

/*
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
    bool success =
        mRowIndexes.right.replace_key(mRowIndexes.right.find(i), i - 1);
    PBDev::basicAssert(success);
  }
  log();
}
*/

void ImageMonitor::removeRow(int row)
{
  PBDev::basicAssert(!mPendingRows.contains(row));

  for (auto i = 0; i < mUnstagedImagesMatrix.at(row).size(); ++i) {
    mPositionsV2Reverse.erase(std::pair<int, int>{row, i});
  }

mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + row);

mRowIndexes.right.erase(row);

for (int i = row + 1; i < (int)mUnstagedImagesMatrix.size() + 1; ++i) {
  for (int index = 0; index < (int)mUnstagedImagesMatrix.at(i - 1).size();
       ++index) {
    mPositionsV2Reverse.emplace(
        std::pair<int, int>{i - 1, index},
        mPositionsV2Reverse.at(std::pair<int, int>{i, index}));
    mPositionsV2Reverse.erase({i, index});
  }
}

for (int i = row + 1; i < mRowIndexes.size() + 1; ++i) {
  bool success =
      mRowIndexes.right.replace_key(mRowIndexes.right.find(i), i - 1);
  PBDev::basicAssert(success);
}
log();
}

void ImageMonitor::removeRow(Path path)
{
  int index = mRowIndexes.left.at(path);
  PBDev::basicAssert(!mPendingRows.contains(index));
  removeRow(index);
}

void ImageMonitor::clear()
{
  PBDev::basicAssert(mPendingRows.empty());
  mRowIndexes.clear();
  mPositionsV2Reverse.clear();
  mUnstagedImagesMatrix.clear();
  log();
}

void ImageMonitor::completeRow(int index) { mPendingRows.erase(index); }

void ImageMonitor::completeRowByPath(Path path)
{
  auto index = rowIndex(path);
  completeRow(index);
}

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

bool ImageMonitor::containsRow(Path path, bool subPath) const
{
  if (subPath) {
    for (auto it = mRowIndexes.begin(); it != mRowIndexes.end(); ++it) {
      if (it->left == path) {
        return true;
      }
    }
  }
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

GenericImagePtr ImageMonitor::image(unsigned row, unsigned index) const
{
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
  PBDev::basicAssert(index < mUnstagedImagesMatrix.at(row).size());

  auto imageId = mUnstagedImagesMatrix.at(row).at(index);
  return mImages.at(imageId);
}
/*
GenericImagePtr ImageMonitor::image(Path full) const
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
*/
std::vector<std::vector<PBDev::ImageId>> const &ImageMonitor::unstaged() const
{
  return mUnstagedImagesMatrix;
}

void ImageMonitor::log() const
{
  /*
  PB::printDebug("mRowIndexes\n");
  for (auto it = mRowIndexes.begin(); it != mRowIndexes.end(); ++it) {
    PB::printDebug("(%s %d)\n", it->left.stem().string().c_str(), it->right);
  }
  PB::printDebug("mPositions\n");
  for (auto it = mPositions.begin(); it != mPositions.end(); ++it) {
    PB::printDebug("(%s [%d %d])\n", it->left.stem().string().c_str(),
                   it->right.first, it->right.second);
  }
  */
}

} // namespace PB