#pragma once

#include <unordered_map>

#include <pb/Config.h>
#include <pb/infra/TaskCruncher.h>
#include <pb/infra/ThreadScheduler.h>
#include <pb/entities/RowProcessingData.h>
#include <pb/jobs/PicturesSearchJob.h>
#include <pb/jobs/ThumbnailsJob.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/infra/Error.h>

using namespace PB::Job;

namespace PB::Service {

class ImportFoldersServiceListener {
public:
  virtual ~ImportFoldersServiceListener() = default;

  virtual void onMappingFinished(Path root, std::vector<Path> newFolders) = 0;
  virtual void onImageProcessed(Path key, Path root, GenericImagePtr image) = 0;
  virtual void onImportError(PBDev::Error error) = 0;
};

class ImportFoldersService final : public PicturesSearchJobListener,
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

  void configureProjectManagementService(
      std::shared_ptr<ProjectManagementService> projectManagementService)
  {
    mProjectManagementService = projectManagementService;
  }

  std::optional<PBDev::Error> addImportFolder(Path path);

  void onPicturesSearchFinished(PBDev::ThumbnailsJobId jobId, Path root,
                                std::vector<Path> searchResults) override;

  void onPicturesSearchAborted(Path root) override;

  void imageProcessed(PBDev::ThumbnailsJobId jobId,
                      GenericImagePtr        image) override;

  bool isFinished(Path path);

private:
  ImportFoldersServiceListener             *mListener = nullptr;
  PBDev::ThreadScheduler                   *mScheduler = nullptr;
  std::shared_ptr<TaskCruncher>             mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;

  std::unordered_map<PBDev::ThumbnailsJobId, Path,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mRootPaths;
  std::unordered_map<PBDev::ThumbnailsJobId, ThumbnailsJob,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mThumbnailsJobs;
  std::unordered_map<PBDev::ThumbnailsJobId, PicturesSearchJob,
                     boost::hash<PBDev::ThumbnailsJobId>>
      mSearches;

  void startThumbnailsCreation(PBDev::ThumbnailsJobId jobId,
                               std::vector<Path>      searchResults);
};
} // namespace PB