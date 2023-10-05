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

void ImageSupport::addFullPaths(Path root, std::vector<Path> const &paths)
{
  if (mGroupContent.find(root) == mGroupContent.end()) {
    mGroup.push_back(root);
  }
  for (auto &p : paths) {
    Thumbnails newThumbnails(p);
    mSupport.push_back(newThumbnails);
    mSupportByFullPath[p] = (int)(mSupport.size() - 1);
    mGroupContent[root].insert((int)(mSupport.size() - 1));
  }
}
} // namespace PB