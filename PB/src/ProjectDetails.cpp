#include <pb/persistence/ProjectDetails.h>

#include <algorithm>

#include <pb/VirtualImage.h>

namespace PB {

Path ProjectDetails::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

Path ProjectDetails::projectFile() const
{
  return mParentDirectory / (mSupportDirName + Context::BOOK_EXTENSION);
}

bool ProjectDetails::importedPathsChanged(std::vector<Path> const &paths) const
{
  if (mImportedPaths.size() != paths.size()) {
    return true;
  }
  else {
    return !std::equal(paths.begin(), paths.end(), mImportedPaths.begin());
  }
  return false;
}

bool ProjectDetails::stagedImagesChanged(
    std::vector<std::shared_ptr<VirtualImage>> stagedImages)
{
  if (mImportedPaths.size() != stagedImages.size()) {
    return true;
  }
  else {
    for (int i = 0; i < (int)stagedImages.size(); ++i) {
      if (stagedImages.at(i)->fullSizePath() != mImportedPaths.at(i)) {
        return true;
      }
    }
  }
  return false;
}

bool ProjectDetails::paperSettingsChanged(PaperSettings const &paperSettings)
{
  return !(mPaperSettings == paperSettings);
}

bool ProjectDetails::projectDetailsChanged(
    std::vector<Path> const                   &importedPaths,
    std::vector<std::shared_ptr<VirtualImage>> stagedChanges,
    PaperSettings const                       &paperSettings)
{
  importedPathsChanged(importedPaths) || stagedImagesChanged(stagedChanges) ||
      paperSettingsChanged(paperSettings);
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