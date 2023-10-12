#include <pb/DataManager.h>

namespace PB {

void ImageSupport::setListener(std::shared_ptr<ImageSupportListener> listener)
{
  mListener = listener;
}

Thumbnails &ImageSupport::image(Path fullPath)
{
  auto &[importPathIndex, pathIndex] = mSupportByFullPath.at(fullPath);
  return mSupport.at(importPathIndex).at(pathIndex);
}

void ImageSupport::addSmallPath(Path fullPath, Path smallSize)
{
  auto &[importPathIndex, pathIndex] = mSupportByFullPath.at(fullPath);
  mSupport[importPathIndex][pathIndex].smallThumbnail = smallSize;
}

void ImageSupport::addMediumPath(Path fullPath, Path mediumPath)
{
  auto &[importPathIndex, pathIndex] = mSupportByFullPath.at(fullPath);
  mSupport[importPathIndex][pathIndex].mediumThumbnail = mediumPath;
}

void ImageSupport::addGroup(std::optional<Path> path)
{
  if (!path) {
    return;
  }
  mSupport.push_back(std::vector<Thumbnails>());
  mGroupIndexes[*path] = (int)mSupport.size() - 1;
}

void ImageSupport::addFullPaths(Path root, std::vector<Path> const &paths)
{
  if (mGroupIndexes.find(root) == mGroupIndexes.end()) {
    addGroup(root);
  }

  int indexGroup = mGroupIndexes.at(root);

  for (auto &p : paths) {
    Thumbnails newThumbnails(p);

    mSupport.at(indexGroup).push_back(newThumbnails);

    mSupportByFullPath[p] = {indexGroup,
                             (int)(mSupport.at(indexGroup).size() - 1)};
  }
  if (mListener) {
    mListener->importFolderAdded(
        root,
        CircularIterator<std::vector<Thumbnails>>(mSupport.at(indexGroup)));
  }
}

void ImageSupport::stagePhoto(Thumbnails paths, int position)
{
  if (position == -1) {
    mStagedPhotos.push_back(paths);
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<Thumbnails>>(mStagedPhotos));
    }
  }
  else if (position < mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, paths);
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<Thumbnails>>(mStagedPhotos));
    }
  }
}

void ImageSupport::unstagePhoto(int index)
{
  if (index < mStagedPhotos.size() && index > -1) {
    mStagedPhotos.erase(mStagedPhotos.begin() + index);
    if (mListener) {
      mListener->stagePhotosUpdated(
          CircularIterator<std::vector<Thumbnails>>(mStagedPhotos));
    }
  }
}

std::vector<Path> ImageSupport::fullPathByGroup(Path group)
{
  if (mGroupIndexes.find(group) == mGroupIndexes.end()) {
    return std::vector<Path>();
  }
  auto             &index = mGroupIndexes.at(group);
  std::vector<Path> result;
  for (auto &entry : mSupport.at(index)) {
    result.push_back(entry.fullPath);
  }
  return result;
}

std::optional<Path> ImageSupport::groupByIndex(int index)
{
  for (auto &[key, value] : mGroupIndexes) {
    if (value == index) {
      return key;
    }
  }
  return std::nullopt;
}

auto ImageSupport::unstagedIterator(Path root)
    -> CircularIterator<std::vector<Thumbnails>>
{
  if (mGroupIndexes.find(root) == mGroupIndexes.end()) {
    return CircularIterator<std::vector<Thumbnails>>();
  }

  return CircularIterator<std::vector<Thumbnails>>(
      mSupport.at(mGroupIndexes.at(root)));
}

auto ImageSupport::unstagedIterator(int importedFolderIndex, int index)
    -> CircularIterator<std::vector<Thumbnails>>
{
  if (index > -1) {
    return CircularIterator<std::vector<Thumbnails>>(
        mSupport.at(importedFolderIndex))[index];
  }
  else {
    return CircularIterator<std::vector<Thumbnails>>(
        mSupport.at(importedFolderIndex));
  }
}

auto ImageSupport::stagedIterator() -> CircularIterator<std::vector<Thumbnails>>
{
  return CircularIterator<std::vector<Thumbnails>>(mStagedPhotos);
}

int ImageSupport::groupSize(std::optional<Path> group)
{
  if (!group) {
    return 0;
  }

  return (int)mSupport.at(mGroupIndexes.at(*group)).size();
}

void ImageSupport::clear()
{
  mGroupIndexes.clear();

  mSupportByFullPath.clear();
  mSupport.clear();

  mStagedPhotos.clear();
}

std::unordered_map<Path, int> const &ImageSupport::groups()
{
  return mGroupIndexes;
}

} // namespace PB