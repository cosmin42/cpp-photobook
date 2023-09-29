#include <pb/StagedImagesLogic.h>

namespace PB {
StagedImagesLogic::StagedImagesLogic(ProjectDetails const &projectDetails)
    : mProjectDetails(projectDetails)
{
}

void StagedImagesLogic::generateThumbnails(MediaMap &mediaMap)
{
  for (auto i = 0; i < mediaMap.map().size(); ++i) {
    auto outputPath = assembleOutputPath(i);
    PB::Unused(outputPath);
  }
}

Path StagedImagesLogic::assembleOutputPath(int index)
{
  assert(index >= 0);
  assert(mProjectDetails.dirName.length() > 0);

  auto path = mProjectDetails.parentDirectory / mProjectDetails.dirName /
              (sPrefix + std::to_string(index) + Context::jpgExt);

  return path;
}
} // namespace PB