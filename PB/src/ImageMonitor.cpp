#include <pb/ImageMonitor.h>

#include <boost/bimap/support/lambda.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace PB {

void ImageMonitor::addRow(Path importFolderPath,
                          std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                             boost::hash<PBDev::ImageId>>
                              images)
{
  PBDev::basicAssert(importedPathsIndices.left.find(importFolderPath) ==
                     importedPathsIndices.left.end());
  importedPathsIndices.insert(
      {importFolderPath, (int)importedPathsIndices.size()});

  mUnstagedImagesMatrix.push_back(std::vector<PBDev::ImageId>());
  int index = 0;
  for (auto &[id, image] : images) {
    mPositionsV2.insert(id, {(int)(mUnstagedImagesMatrix.size() - 1), index});
    mImages[id] = image;
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1).push_back(id);
      index++;
  }
}

void ImageMonitor::updateImage(PBDev::ImageId imageId, GenericImagePtr image)
{
  PBDev::basicAssert(mImages.find(imageId) != mImages.end());
  mImages[imageId] = image;
}

void ImageMonitor::removeRow(int row)
{
  for (auto i = 0; i < mUnstagedImagesMatrix.at(row).size(); ++i) {
    mPositionsV2.removeByValue({row, i});
  }

  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + row);

  importedPathsIndices.right.erase(row);

  for (int i = row + 1; i < (int)mUnstagedImagesMatrix.size() + 1; ++i) {
    for (int index = 0; index < (int)mUnstagedImagesMatrix.at(i - 1).size();
         ++index) {

      auto imageId = mPositionsV2.getKey({i, index});
      mPositionsV2.insert(imageId, {i - 1, index});
      mPositionsV2.removeByKey(imageId);
    }
  }

  for (int i = row + 1; i < importedPathsIndices.size() + 1; ++i) {
    bool success = importedPathsIndices.right.replace_key(
        importedPathsIndices.right.find(i), i - 1);
    PBDev::basicAssert(success);
  }
  log();
}

void ImageMonitor::removeRow(Path path)
{
  int index = importedPathsIndices.left.at(path);
  removeRow(index);
}

void ImageMonitor::clear()
{
  importedPathsIndices.clear();
  mPositionsV2.clear();
  mUnstagedImagesMatrix.clear();
  log();
}

unsigned ImageMonitor::rowSize(unsigned row)
{
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
  return (unsigned)mUnstagedImagesMatrix.at(row).size();
}

unsigned ImageMonitor::rowIndexByPath(Path path) const
{
  return importedPathsIndices.left.at(path);
}

std::vector<Path> ImageMonitor::rowList() const
{
  std::vector<Path> result;
  for (int i = 0; i < importedPathsIndices.size(); ++i) {
    result.push_back(importedPathsIndices.right.at(i));
  }
  return result;
}

Path ImageMonitor::rowPathByIndex(unsigned row) const
{
  return importedPathsIndices.right.at(row);
}

std::pair<PBDev::ImageId, GenericImagePtr>
ImageMonitor::image(unsigned row, unsigned index) const
{
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
  PBDev::basicAssert(index < mUnstagedImagesMatrix.at(row).size());

  auto imageId = mUnstagedImagesMatrix.at(row).at(index);
  return {imageId, mImages.at(imageId)};
}

std::pair<int, int> ImageMonitor::position(PBDev::ImageId imageId) const
{
  PBDev::basicAssert(mPositionsV2.hasKey(imageId));
  return mPositionsV2.getValue(imageId);
}

std::vector<std::vector<PBDev::ImageId>> const &ImageMonitor::unstaged() const
{
  return mUnstagedImagesMatrix;
}

GenericImagePtrMatrix ImageMonitor::imagesMatrix() const
{
  GenericImagePtrMatrix result;
  for (auto &line : mUnstagedImagesMatrix) {
    GenericImagePtrLine lineVector;
    for (auto &imageId : line) {
      lineVector.push_back(mImages.at(imageId));
    }
    result.push_back(lineVector);
  }
  return result;
}

std::unordered_map<PBDev::ImageId, GenericImagePtr,
                   boost::hash<PBDev::ImageId>> const &
ImageMonitor::imagesSet() const
{
  return mImages;
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
