#pragma once

#include <unordered_map>

#include <pb/Config.h>
#include <pb/tasks/FileMapper.h>
#include <pb/tasks/ThumbnailsProcessor.h>

namespace PB {

class ImportFoldersLogicListener {
public:
  virtual ~ImportFoldersLogicListener() = default;

  virtual void onMappingStarted(Path path) = 0;
  virtual void onMappingFinished(Path root, std::vector<Path> newFolders) = 0;
  virtual void onMappingAborted(Path) = 0;

  virtual void onImportStop(Path) = 0;
  virtual void onImageProcessed(Path root, Path full, Path medium, Path small) = 0;
};

class ThreadScheduler {
public:
  virtual void post(std::function<void()> f) = 0;
};

class ImportFoldersLogic final : public PBDev::Observer {
public:
  void configure(ImportFoldersLogicListener *listener);
  void configure(ThreadScheduler *scheduler);
  void configure(std::pair<int, int> screenSize);
  void configure(std::shared_ptr<Project> project);

  std::optional<PBDev::Error> addImportFolder(Path path);

  void start(Path path);
  void stop(Path path);
  void stopAll();
  void update(PBDev::ObservableSubject &subject);

  void clearJob(Path root);

  void processImages(Path root, std::vector<Path> newFolders);

  std::pair<int, int> imageProcessingProgress() const;

private:
  void setObserverManager();

  void onImageProcessed(Path root, Path full, Path medium, Path small,
                        int progress, int progressCap);

  ImportFoldersLogicListener                            *mListener = nullptr;
  ThreadScheduler                                       *mScheduler = nullptr;
  std::unordered_map<Path, std::shared_ptr<MediaMapper>> mMappingJobs;
  std::unordered_map<Path, std::pair<int, int>> mImageProcessingProgress;
  ThumbnailsProcessor                           mThumbnailsProcessor;
};
} // namespace PB