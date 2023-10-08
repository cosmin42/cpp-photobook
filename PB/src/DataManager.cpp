#include <pb/DataManager.h>

namespace PB {

void ImageSupport::addSmall(Path fullSize, Path smallSize)
{
  if (mSupportByFullPath.find(fullSize) == mSupportByFullPath.end()) {
    PB::printDebug("Could not find %s", fullSize.string().c_str());
    return;
  }
  auto index = mSupportByFullPath[fullSize];
  mSupport[index].smallThumbnail = smallSize;
  mSupportBySmallThumbnail[smallSize] = index;
}

void ImageSupport::addMedium(Path fullSize, Path mediumSize)
{
  if (mSupportByFullPath.find(fullSize) == mSupportByFullPath.end()) {
    PB::printDebug("Could not find %s", fullSize.string().c_str());
    return;
  }
  auto index = mSupportByFullPath[fullSize];
  mSupport[index].mediumThumbnail = mediumSize;
  mSupportByMediumThumbnail[mediumSize] = index;
}

std::vector<Path> ImageSupport::fullPathByGroup(Path group)
{
  if (mGroupContent.find(group) == mGroupContent.end()) {
    return std::vector<Path>();
  }
  auto             &pathSet = mGroupContent.at(group);
  std::vector<Path> result;
  for (auto index : pathSet) {
    result.push_back(mSupport.at(index).fullPath);
  }
  return result;
}

std::optional<Path> ImageSupport::groupByIndex(int index)
{
  if (index > -1 && index < mGroup.size()) {
    return mGroup.at(index);
  }
  return std::nullopt;
}

auto ImageSupport::thumbnailsSet(Path root)
    -> CircularIterator<std::vector<Thumbnails>>
{
  auto filterFunction = [this, root{root}](Thumbnails const &th) {
    if (mGroupContent.find(root) == mGroupContent.end()) {
      return false;
    }

    auto finding = std::find_if(
        mGroupContent.at(root).begin(), mGroupContent.at(root).end(),
        [this, path{th.fullPath}](int index) {
          if (index > mSupport.size()) {
            return false;
          }
          return mSupport.at(index).fullPath.string() == path.string();
        });

    return finding != mGroupContent.at(root).end();
  };

  return CircularIterator<std::vector<Thumbnails>>(mSupport, filterFunction);
}

int ImageSupport::groupSize(std::optional<Path> group)
{
  if (!group) {
    return 0;
  }
  if (mGroupContent.find(*group) == mGroupContent.end()) {
    return 0;
  }
  return (int)mGroupContent.at(*group).size();
}

std::optional<Thumbnails> ImageSupport::getByMedium(std::optional<Path> path)
{
  if (!path) {
    return std::nullopt;
  }
  if (mSupportByMediumThumbnail.find(*path) ==
      mSupportByMediumThumbnail.end()) {
    return std::nullopt;
  }
  auto index = mSupportByMediumThumbnail.at(*path);
  return mSupport.at(index);
}

std::optional<Thumbnails> ImageSupport::getBySmall(std::optional<Path> path)
{
  if (!path) {
    return std::nullopt;
  }
  if (mSupportBySmallThumbnail.find(*path) == mSupportBySmallThumbnail.end()) {
    return std::nullopt;
  }
  auto index = mSupportBySmallThumbnail.at(*path);
  return mSupport.at(index);
}

void ImageSupport::addGroup(std::optional<Path> path, unsigned size)
{
  if (!path) {
    return;
  }
  mGroup.push_back(*path);
  mGroupContent[*path] = std::vector<int>(size);
}

void ImageSupport::clear()
{
  mGroupContent.clear();
  mGroup.clear();
  mSupportBySmallThumbnail.clear();
  mSupportByMediumThumbnail.clear();
  mSupportByFullPath.clear();
  mSupport.clear();
}

std::vector<Path> const &ImageSupport::groups() { return mGroup; }

void ImageSupport::stagePhoto(Thumbnails paths, int position)
{
  if (position == -1) {
    mStagedPhotos.push_back(paths);
  }
  else if (position < mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, paths);
  }
}

void ImageSupport::unstagePhoto(int index)
{
  if (index < mStagedPhotos.size() && index > -1) {
    mStagedPhotos.erase(mStagedPhotos.begin() + index);
  }
}

void ImageSupport::addFullPaths(Path root, std::vector<Path> const &paths)
{
  if (mGroupContent.find(root) == mGroupContent.end()) {
    mGroup.push_back(root);
  }
  for (auto &p : paths) {
    Thumbnails newThumbnails(p);
    mSupport.push_back(newThumbnails);
    mSupportByFullPath[p] = (int)(mSupport.size() - 1);
    if (std::find(mGroupContent[root].begin(), mGroupContent[root].end(),
                  (int)(mSupport.size() - 1)) == mGroupContent[root].end()) {
      mGroupContent[root].push_back((int)(mSupport.size() - 1));
    }
  }
}
} // namespace PB