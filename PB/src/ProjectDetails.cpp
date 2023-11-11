#include <pb/persistence/ProjectDetails.h>

#include <algorithm>

#include <pb/VirtualImage.h>

namespace PB {

Path ProjectDetails::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

void ProjectDetails::setImportedPaths(std::vector<Path> paths)
{
  mImportedPaths.clear();
  for (auto &path : paths) {
    mImportedPaths.push_back(path);
  }
}

void ProjectDetails::setStagedImages(
    std::vector<std::shared_ptr<VirtualImage>> stagedImages)
{
  mStagedImages.clear();
  for (auto image : stagedImages) {
    mStagedImages.push_back(image->fullSizePath());
  }
}

void ProjectDetails::setStagedImages(std::vector<Path> stagedImages)
{
  mStagedImages.clear();
  for (auto &image : stagedImages) {
    mStagedImages.push_back(image);
  }
}

void ProjectDetails::setPaperSettings(PaperSettings paperSettings)
{
  mPaperSettings = paperSettings;
}

std::vector<Path> ProjectDetails::importedFolderList() const
{
  return mImportedPaths;
}

std::vector<Path> ProjectDetails::stagedImagesList() const
{
  return mStagedImages;
}

PaperSettings ProjectDetails::paperSettings() const { return mPaperSettings; }

} // namespace PB