#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#pragma warning(pop)

#include <pb/Platform.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/infra/Traits.h>

namespace PB {
class ThumbnailsTask final {
public:
  static std::string createThumbnails(
      std::shared_ptr<cv::Mat>                  originalImage,
      std::shared_ptr<PlatformInfo>             platformInfo,
      std::shared_ptr<ProjectManagementService> projectManagementService,
      std::string                               targetHash = "");

  static std::string
  createThumbnailsByPath(Path                                 originalImage,
                         std::shared_ptr<PlatformInfo>        platformInfo,
                         std::shared_ptr<IdentifyableProject> project,
                         std::string                          targetHash = "");

  explicit ThumbnailsTask(Path originalPath);
  ~ThumbnailsTask() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProjectManagementService(
      std::shared_ptr<ProjectManagementService> projectManagementService);

  std::string createThumbnails();

private:
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  Path                                      mOriginalPath;
};
} // namespace PB
