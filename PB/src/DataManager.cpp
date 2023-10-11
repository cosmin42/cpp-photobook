#include <pb/DataManager.h>

namespace PB {

void ImageSupport::setListener(std::shared_ptr<ImageSupportListener> listener)
{
  mListener = listener;
}

Thumbnails &ImageSupport::image(Path fullPath)
{
  if (mSupportByFullPath.find(fullPath) == mSupportByFullPath.end()) {
    PB::printDebug("Could not find %s", fullPath.string().c_str());
    return;
  }
  auto &[importPathIndex, pathIndex] = mSupportByFullPath.at(fullPath);
  return mSupport.at(importPathIndex).at(pathIndex);
}

void ImageSupport::addSmallPath(Path fullPath, Path smallSize)
{
  image(fullPath).smallThumbnail = smallSize;
}

void ImageSupport::addMediumPath(Path fullPath, Path mediumPath)
{
  image(fullPath).mediumThumbnail = mediumPath;
}

void ImageSupport::addGroup(std::optional<Path> path, unsigned size)
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
    addGroup(root, paths.size());
  }

  int indexGroup = mGroupIndexes.at(root);

  for (auto &p : paths) {
    Thumbnails newThumbnails(p);

    mSupport.at(indexGroup).push_back(newThumbnails);

    mSupportByFullPath[p] = {indexGroup, (int)(mSupport.size() - 1)};
  }
  if (mListener) {
    mListener->importFolderAdded(indexGroup);
  }
}

void ImageSupport::stagePhoto(Thumbnails paths, int position)
{
  if (position == -1) {
    mStagedPhotos.push_back(paths);
    if (mListener) {
      mListener->stagePhotosUpdated();
    }
  }
  else if (position < mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, paths);
    if (mListener) {
      mListener->stagePhotosUpdated();
    }
  }
}

void ImageSupport::unstagePhoto(int index)
{
  if (index < mStagedPhotos.size() && index > -1) {
    mStagedPhotos.erase(mStagedPhotos.begin() + index);
    if (mListener) {
      mListener->stagePhotosUpdated();
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
      mSupport.at(mGroupIndexes.at(root)), [](Thumbnails) { return true; });
}

auto ImageSupport::unstagedIterator(int index)
    -> CircularIterator<std::vector<Thumbnails>>
{
  return CircularIterator<std::vector<Thumbnails>>(
      mSupport.at(index), [](Thumbnails) { return true; });
}

auto ImageSupport::stagedIterator() -> CircularIterator<std::vector<Thumbnails>>
{
  auto filterF = [](Thumbnails) { return true; };
  return CircularIterator<std::vector<Thumbnails>>(mStagedPhotos, filterF);
}

int ImageSupport::groupSize(std::optional<Path> group)
{
  if (!group) {
    return 0;
  }

  return mSupport.at(mGroupIndexes.at(*group)).size();
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