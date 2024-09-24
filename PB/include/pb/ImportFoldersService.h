#pragma once

#include <unordered_map>

#include <pb/Config.h>
#include <pb/ProjectManagementSystem.h>
#include <pb/RowProcessingData.h>
#include <pb/TaskCruncher.h>
#include <pb/ThreadScheduler.h>
#include <pb/ThumbnailsJob.h>
#include <pb/tasks/PicturesSearchConfig.h>
#include <pb/util/Error.h>

namespace PB {

class ImportFoldersServiceListener {
public:
  virtual ~ImportFoldersServiceListener() = default;

  virtual void onMappingFinished(Path root, std::vector<Path> newFolders) = 0;
  virtual void onImageProcessed(Path key, Path root,
                                ImageResources imageResources) = 0;
};

class ImportFoldersService final : public PicturesSearchConfigListener,
                                 public ThumbnailsJobListener {
public:
  ~ImportFoldersService() = default;

  void configureListener(ImportFoldersServiceListener *listener)
  {
    mListener = listener;
  }

  void configureScheduler(PBDev::ThreadScheduler *scheduler)
  {
    mScheduler = scheduler;
  }

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
  {
    mTaskCruncher = taskCruncher;
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

  std::optional<PBDev::Error> addImportFolder(Path path);

  void onPicturesSearchFinished(PBDev::ThumbnailsJobId jobId, Path root,
                                std::vector<Path> searchResults) override;

  void onPicturesSearchAborted(Path root) override;

  void imageProcessed(PBDev::ThumbnailsJobId       jobId,
                      std::tuple<Path, Path, Path> thumbnailPaths) override;

private:
  ImportFoldersServiceListener              *mListener = nullptr;
  PBDev::ThreadScheduler                  *mScheduler = nullptr;
  std::shared_ptr<TaskCruncher>            mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementSystem> mProjectManagementSystem = nullptr;

  std::unordered_map<PBDev::ThumbnailsJobId, Path,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mRootPaths;
  std::unordered_map<PBDev::ThumbnailsJobId, ThumbnailsJob,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mThumbnailsJobs;
  std::unordered_map<PBDev::ThumbnailsJobId, PicturesSearchConfig,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mSearches;

  void startThumbnailsCreation(PBDev::ThumbnailsJobId jobId,
                               std::vector<Path>      searchResults);
};
} // namespace PB