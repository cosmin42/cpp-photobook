#pragma once

#include <pb/Platform.h>
#include <pb/ProjectManagementSystem.h>
#include <pb/util/Traits.h>

namespace PB {
class ThumbnailsTask final {
public:
  explicit ThumbnailsTask(Path originalPath);
  ~ThumbnailsTask() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProjectManagementSystem(
	  std::shared_ptr<ProjectManagementSystem> projectManagementSystem);

  std::tuple<Path, Path, Path> createThumbnails();

private:
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementSystem> mProjectManagementSystem = nullptr;
  Path                                     mOriginalPath;


  std::tuple<cv::Size, cv::Size, cv::Size> thumbnailSizes(cv::Size originalSize, cv::Size paperSize);
};
} // namespace PB
