#include <pb/DataManager.h>
#include <pb/VirtualImage.h>

namespace PB {

void ImageSupport::setListener(std::shared_ptr<ImageSupportListener> listener)
{
  mListener = listener;
}

void ImageSupport::addGroup(std::optional<Path> path)
{
  if (!path) {
    return;
  }
  mUnstagedImagesMatrix.push_back(std::vector<std::shared_ptr<VirtualImage>>());
  mGroupIndexes[*path] = (int)mUnstagedImagesMatrix.size() - 1;
}

void ImageSupport::addImage(Path root, std::vector<Path> paths,
                            std::vector<std::shared_ptr<VirtualImage>> images)
{
  if (mGroupIndexes.find(root) == mGroupIndexes.end()) {
    addGroup(root);
  }
  int indexGroup = mGroupIndexes.at(root);

  PB::basicAssert(paths.size() == images.size());

  for (int i = 0; i < (int)paths.size(); ++i) {
    mUnstagedImagesMatrix.at(indexGroup).push_back(images.at(i));
    mSupportByFullPath[paths.at(i)] = {
        indexGroup, (int)(mUnstagedImagesMatrix.at(indexGroup).size() - 1)};
  }

  if (mListener) {
    mListener->importFolderAdded(
        root, CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
                  mUnstagedImagesMatrix.at(indexGroup)));
  }
}

void ImageSupport::stagePhoto(std::vector<std::shared_ptr<VirtualImage>> paths,
                              int position)
{
  if (position == -1) {
    mStagedPhotos.insert(mStagedPhotos.end(), paths.begin(), paths.end());
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
              mStagedPhotos));
    }
  }
  else if (position < mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, paths.begin(),
                         paths.end());
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
              mStagedPhotos));
    }
  }
  else if (position == mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.end(), paths.begin(), paths.end());
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
              mStagedPhotos));
    }
  }
}

void ImageSupport::updateStagedPhoto(std::shared_ptr<VirtualImage> newImage)
{
  for (auto photo : mStagedPhotos) {
    if (photo->fullSizePath() == newImage->fullSizePath()) {
      photo->setMediumSizePath(newImage->mediumSizePath());
      photo->setSmallSizePath(newImage->smallSizePath());
    }
  }
}

void ImageSupport::unstagePhoto(std::vector<int> indexes)
{
  std::sort(indexes.begin(), indexes.end(), std::greater<int>());
  for (int i = 0; i < indexes.size(); ++i) {
    mStagedPhotos.erase(mStagedPhotos.begin() + indexes.at(i));
  }
  if (indexes.size() > 0 && mListener) {
    mListener->stagePhotosUpdated(
        CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
            mStagedPhotos));
  }
}

void ImageSupport::removeGroup(int index)
{
  auto maybeGroup = groupByIndex(index);
  PB::basicAssert(maybeGroup.has_value());
  mGroupIndexes.erase(maybeGroup.value());
  for (auto &[key, value] : mGroupIndexes) {
    if (value > index) {
      value--;
    }
  }

  for (int i = 0; i < mUnstagedImagesMatrix.at(index).size(); ++i) {
    auto path = mUnstagedImagesMatrix.at(index).at(i)->fullSizePath();
    mSupportByFullPath.erase(path);
  }
  mUnstagedImagesMatrix.erase(mUnstagedImagesMatrix.begin() + index);
  if (mListener) {
    mListener->stagePhotosUpdated(
        CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
            mStagedPhotos));
  }
}

int ImageSupport::fullPathRow(Path path) const
{
  return mSupportByFullPath.at(path).first;
}

std::vector<Path> ImageSupport::fullPathByGroup(Path group) const
{
  if (mGroupIndexes.find(group) == mGroupIndexes.end()) {
    return std::vector<Path>();
  }
  auto             &index = mGroupIndexes.at(group);
  std::vector<Path> result;
  for (auto entry : mUnstagedImagesMatrix.at(index)) {
    result.push_back(entry->fullSizePath());
  }
  return result;
}

std::optional<Path> ImageSupport::groupByIndex(int index)
{
  if (index == -1) {
    return std::nullopt;
  }
  for (auto &[key, value] : mGroupIndexes) {
    if (value == index) {
      return key;
    }
  }
  return std::nullopt;
}

auto ImageSupport::unstagedIterator(Path root)
    -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
{
  if (mGroupIndexes.find(root) == mGroupIndexes.end()) {
    return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>();
  }

  return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
      mUnstagedImagesMatrix.at(mGroupIndexes.at(root)));
}

auto ImageSupport::unstagedIterator(int importedFolderIndex, int index)
    -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
{
  if (importedFolderIndex < 0) {
    return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>();
  }
  if (index > -1) {
    return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
        mUnstagedImagesMatrix.at(importedFolderIndex))[index];
  }
  else {
    if (mUnstagedImagesMatrix.size() > 0) {
      return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
          mUnstagedImagesMatrix.at(importedFolderIndex));
    }
    else {
      return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>();
    }
  }
}

auto ImageSupport::stagedIterator()
    -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
{
  return CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
      mStagedPhotos);
}

int ImageSupport::groupSize(std::optional<Path> group) const
{
  if (!group) {
    return 0;
  }

  return (int)mUnstagedImagesMatrix.at(mGroupIndexes.at(*group)).size();
}

void ImageSupport::clear()
{
  mGroupIndexes.clear();

  mSupportByFullPath.clear();
  mUnstagedImagesMatrix.clear();

  mStagedPhotos.clear();
  if (mListener) {
    mListener->stagePhotosUpdated(
        CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>(
            mStagedPhotos));
  }
}

std::vector<Path> ImageSupport::groups() const
{
  std::vector<Path> result;
  for (auto &[key, value] : mGroupIndexes) {
    result.push_back(key);
    PB::Unused(value);
  }
  return result;
}

int ImageSupport::groupIndex(Path path) const { return mGroupIndexes.at(path); }

bool ImageSupport::containsGroup(Path path)
{
  return (mGroupIndexes.find(path) != mGroupIndexes.end());
}

std::vector<std::shared_ptr<VirtualImage>> &ImageSupport::stagedPhotos()
{
  return mStagedPhotos;
}

std::vector<Path> ImageSupport::stagedPhotosFullPaths() const
{
  std::vector<Path> results;
  for (auto thumbnail : mStagedPhotos) {
    results.push_back(thumbnail->fullSizePath());
  }
  return results;
}

} // namespace PB