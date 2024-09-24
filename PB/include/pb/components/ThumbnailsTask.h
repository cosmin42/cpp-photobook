#pragma once

#include <pb/Platform.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/util/Traits.h>

namespace PB {
class ThumbnailsTask final {
public:
  explicit ThumbnailsTask(Path originalPath);
  ~ThumbnailsTask() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProjectManagementService(
      std::shared_ptr<ProjectManagementService> projectManagementService);

  std::tuple<Path, Path, Path> createThumbnails();

private:
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  Path                                      mOriginalPath;

  std::tuple<cv::Size, cv::Size, cv::Size> thumbnailSizes(cv::Size originalSize,
                                                          cv::Size paperSize);
};
} // namespace PB
