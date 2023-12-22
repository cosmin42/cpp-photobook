#pragma once

#include <unordered_map>

#include <pb/Config.h>
#include <pb/FileMapper.h>
#include <pb/ThumbnailsProcessor.h>

namespace PB {

class ImportFoldersLogicListener {
public:
  virtual ~ImportFoldersLogicListener() = default;

  virtual void onMapped(Path, std::vector<Path> newFolders) = 0;
  virtual void onImportStop(Path) = 0;

  virtual void onImageProcessed(Path root, Path full, Path medium, Path small,
                                int progress, int progressCap) = 0;

  virtual void onMappingAborted(Path) = 0;
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
  void configure(ProjectSnapshot project);

  std::optional<Error> addImportFolder(Path path);

  void start(Path path);
  void stop(Path path);
  void stopAll();
  void update(PBDev::ObservableSubject &subject);

  void clearJob(Path root);

private:
  void setObserverManager();

  void processImages(Path root, std::vector<Path> newFolders);

  void onImageProcessed(Path root, Path full, Path medium, Path small,
                        int progress, int progressCap);

  ImportFoldersLogicListener           *mListener = nullptr;
  ThreadScheduler                      *mScheduler = nullptr;
  std::unordered_map<Path, MediaMapper> mMappingJobs;
  ThumbnailsProcessor                   mThumbnailsProcessor;
};
} // namespace PB