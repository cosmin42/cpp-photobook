#include <pb/persistence/ProjectDetails.h>

namespace PB {

Path ProjectDetails::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

Path ProjectDetails::projectFile() const
{
  return mParentDirectory / (mSupportDirName + Context::BOOK_EXTENSION);
}

void ProjectDetails::setImportedPaths(std::vector<Path> paths)
{
  mImportedPaths.clear();
  for (auto &path : paths) {
    mImportedPaths.push_back(path);
  }
}

void ProjectDetails::setStagedImages(std::vector<Thumbnails> stagedImages)
{
  mStagedImages.clear();
  for (auto &image : stagedImages) {
    mStagedImages.push_back(image.fullPath);
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

void ProjectDetails::removeStagedImage(int index)
{
  if (index >= mStagedImages.size()) {
    return;
  }
  mStagedImages.erase(mStagedImages.begin() + index);
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