#pragma once

#include <vector>

#include <pb/Platform.h>
#include <pb/components/MapReducer.h>
#include <pb/components/ThumbnailsTask.h>
#include <pb/image/ImageOperations.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/util/Traits.h>

namespace PB {

class ThumbnailsJobListener {
public:
  virtual void imageProcessed(PBDev::ThumbnailsJobId       jobId,
                              std::tuple<Path, Path, Path> thumbnailPaths) = 0;
};

class ThumbnailsJob : public MapReducer {
public:
  explicit ThumbnailsJob(PBDev::ThumbnailsJobId jobId, std::vector<Path> paths)
      : mJobId(jobId), mPaths(paths)
  {
  }
  ~ThumbnailsJob() = default;

  void configureListener(ThumbnailsJobListener *listener)
  {
    mListener = listener;
  }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configureProjectManagementService(
      std::shared_ptr<ProjectManagementService> projectManagementSystem)
  {
    mProjectManagementService = projectManagementSystem;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mIndex >= mPaths.size()) {
      return std::nullopt;
    }

    auto path = mPaths[mIndex];
    mIndex++;

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    return std::make_optional(
        IdentifyableFunction{taskId, [this, path]() {
                               auto thumbnails = process(path);
                               mListener->imageProcessed(mJobId, thumbnails);
                             }});
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override {}

  unsigned taskCount() const override { return (unsigned)mPaths.size(); }

private:
  ThumbnailsJobListener                    *mListener = nullptr;
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  PBDev::ThumbnailsJobId                    mJobId;
  unsigned                                  mIndex = 0;
  std::vector<Path>                         mPaths;

  std::tuple<Path, Path, Path> process(Path path)
  {
    if (std::filesystem::is_regular_file(path)) {
      ThumbnailsTask task(path);
      task.configurePlatformInfo(mPlatformInfo);
      task.configureProjectManagementService(mProjectManagementService);
      return task.createThumbnails();
    }
    else if (std::filesystem::is_directory(path)) {

      auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
      PBDev::basicAssert(maybeProject != nullptr);

      auto temporaryImagePath =
          mPlatformInfo->newTemporaryImage(maybeProject->first);

      auto directoryName = path.filename().string();
      Process::createTextImage(maybeProject->second.paperSettings,
                               directoryName, temporaryImagePath);

      ThumbnailsTask task(temporaryImagePath);
      task.configurePlatformInfo(mPlatformInfo);
      task.configureProjectManagementService(mProjectManagementService);
      auto results = task.createThumbnails();

      std::filesystem::remove(temporaryImagePath);
      return results;
    }
    else {
      PBDev::basicAssert(false);
    }
    return {};
  }
};

} // namespace PB