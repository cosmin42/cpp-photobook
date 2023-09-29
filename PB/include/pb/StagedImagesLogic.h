#pragma once

#include <pb/MediaMap.h>
#include <pb/Project.h>

namespace PB {
class StagedImagesLogic final {
public:
  explicit StagedImagesLogic(ProjectDetails const &);
  void generateThumbnails(MediaMap &mediaMap);

private:
  static constexpr const char *sPrefix = "thumbnail";
  Path assembleOutputPath(int index);

  ProjectDetails mProjectDetails;
};
} // namespace PB