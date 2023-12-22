#include <pb/persistence/ProjectSnapshot.h>

#include <algorithm>

#include <pb/image/VirtualImage.h>

namespace PB {

Path ProjectSnapshot::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

void ProjectSnapshot::setImportedPaths(std::vector<Path> paths)
{
  mImportedPaths.clear();
  for (auto &path : paths) {
    mImportedPaths.push_back(path);
  }
}

void ProjectSnapshot::setStagedImages(
    std::vector<std::shared_ptr<VirtualImage>> stagedImages)
{
  mStagedImages.clear();
  for (auto image : stagedImages) {
    mStagedImages.push_back(image->resources().full);
  }
}

void ProjectSnapshot::setStagedImages(std::vector<Path> stagedImages)
{
  mStagedImages.clear();
  for (auto &image : stagedImages) {
    mStagedImages.push_back(image);
  }
}

void ProjectSnapshot::setPaperSettings(PaperSettings paperSettings)
{
  mPaperSettings = paperSettings;
}

std::vector<Path> ProjectSnapshot::importedFolderList() const
{
  return mImportedPaths;
}

std::vector<Path> ProjectSnapshot::stagedImagesList() const
{
  return mStagedImages;
}

PaperSettings ProjectSnapshot::paperSettings() const { return mPaperSettings; }

} // namespace PB