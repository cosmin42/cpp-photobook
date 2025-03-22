#include <pb/ImageMonitor.h>

#include <boost/bimap/support/lambda.hpp>

namespace PB {

void ImageMonitor::addRow(Path path,
                          std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                             boost::hash<PBDev::ImageId>>
                              images)
{
  PBDev::basicAssert(importedPathsIndices.left.find(path) ==
                     importedPathsIndices.left.end());
  importedPathsIndices.insert({path, (int)importedPathsIndices.size()});

  mUnstagedImagesMatrix.push_back(std::vector<PBDev::ImageId>());

  for (auto &[id, image] : images) {
    mPositionsV2Reverse.emplace(
        std::pair<int, int>{(int)importedPathsIndices.size(), 0}, id);
    mPositions.emplace(
        id, std::pair<int, int>{(int)importedPathsIndices.size(), 0});
    mImages[id] = image;
    mUnstagedImagesMatrix.at(mUnstagedImagesMatrix.size() - 1).push_back(id);
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
    mPositionsV2Reverse.erase(std::pair<int, int>{row, i});
    mPositions.erase(mPositionsV2Reverse.at(std::pair<int, int>{row, i}));
  }

  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + row);

  importedPathsIndices.right.erase(row);

  for (int i = row + 1; i < (int)mUnstagedImagesMatrix.size() + 1; ++i) {
    for (int index = 0; index < (int)mUnstagedImagesMatrix.at(i - 1).size();
         ++index) {
      auto imageId = mPositionsV2Reverse.at(std::pair<int, int>{i, index});
      mPositionsV2Reverse.emplace(std::pair<int, int>{i - 1, index}, imageId);
      mPositions.emplace(imageId, std::pair<int, int>{i - 1, index});

      mPositionsV2Reverse.erase({i, index});
      mPositions.erase(imageId);
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
  mPositionsV2Reverse.clear();
  mPositions.clear();
  mUnstagedImagesMatrix.clear();
  log();
}

unsigned ImageMonitor::importsCount() const
{
  return (unsigned)importedPathsIndices.size();
}

unsigned ImageMonitor::rowSize(unsigned row)
{
  PBDev::basicAssert(row < mUnstagedImagesMatrix.size());
  return (unsigned)mUnstagedImagesMatrix.at(row).size();
}

unsigned ImageMonitor::rowIndex(Path path) const
{
  return importedPathsIndices.left.at(path);
}

bool ImageMonitor::containsRow(Path path, bool subPath) const
{
  if (subPath) {
    for (auto it = importedPathsIndices.begin();
         it != importedPathsIndices.end(); ++it) {
      if (it->left == path) {
        return true;
      }
    }
  }
  return importedPathsIndices.left.find(path) !=
         importedPathsIndices.left.end();
}

std::vector<Path> ImageMonitor::rowList() const
{
  std::vector<Path> result;
  for (int i = 0; i < importedPathsIndices.size(); ++i) {
    result.push_back(importedPathsIndices.right.at(i));
  }
  return result;
}

Path ImageMonitor::rowPath(unsigned row) const
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
  PBDev::basicAssert(mPositions.find(imageId) != mPositions.end());
  return mPositions.at(imageId);
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