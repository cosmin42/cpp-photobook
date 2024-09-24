#pragma once

#include <vector>

#include <pb/MapReducer.h>
#include <pb/Platform.h>
#include <pb/ProjectManagementSystem.h>
#include <pb/ThumbnailsTask.h>
#include <pb/image/ImageOperations.h>
#include <pb/util/Traits.h>

namespace PB {

class ThumbnailsJobListener {
public:
  virtual void
  imageProcessed(PBDev::ThumbnailsJobId jobId, std::tuple<Path, Path, Path>
                                        thumbnailPaths) = 0;
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

  void configureProjectManagementSystem(
      std::shared_ptr<ProjectManagementSystem> projectManagementSystem)
  {
    mProjectManagementSystem = projectManagementSystem;
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
  ThumbnailsJobListener                   *mListener = nullptr;
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementSystem> mProjectManagementSystem = nullptr;
  PBDev::ThumbnailsJobId                   mJobId;
  unsigned                                 mIndex = 0;
  std::vector<Path>                        mPaths;

  std::tuple<Path, Path, Path> process(Path path)
  {
    if (std::filesystem::is_regular_file(path)) {
      ThumbnailsTask task(path);
      task.configurePlatformInfo(mPlatformInfo);
      task.configureProjectManagementSystem(mProjectManagementSystem);
      return task.createThumbnails();
    }
    else if (std::filesystem::is_directory(path)) {

      auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
      PBDev::basicAssert(maybeProject != nullptr);

      auto temporaryImagePath =
          mPlatformInfo->newTemporaryImage(maybeProject->first);

      auto directoryName = path.filename().string();
      Process::createTextImage(maybeProject->second.paperSettings,
                               directoryName, temporaryImagePath);

      ThumbnailsTask task(temporaryImagePath);
      task.configurePlatformInfo(mPlatformInfo);
      task.configureProjectManagementSystem(mProjectManagementSystem);
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
