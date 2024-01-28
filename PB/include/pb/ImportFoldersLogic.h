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
  virtual void onImageProcessed(Path key, Path root, Path full, Path medium,
                                Path small) = 0;

  virtual void onError(PBDev::Error error) = 0;
};

class ImportFoldersLogic final
    : public PBDev::SequentialTaskConsumerListener<MediaMapper> {
public:
  void configure(ImportFoldersLogicListener *listener);
  void configure(PBDev::ThreadScheduler *scheduler);

  // todo: Remove this, replace with mProject reference
  void configure(std::pair<int, int> screenSize);
  void configure(std::shared_ptr<Project> project);

  std::optional<PBDev::Error> addImportFolder(Path path);

  void start(Path path);
  void stop(Path path);
  void stopAll();

  void clearJob(Path root);

  void processImages(Path root, std::vector<std::pair<Path, Path>> newFolders);

  // todo: Rename to contain mapping
  void STCStarted(MediaMapper const &) override;
  void STCFinished(MediaMapper const &) override;
  void STCAborted(MediaMapper const &) override;

  std::pair<int, int> imageProcessingProgress() const;
  std::pair<int, int> imageProcessingProgress(Path path) const;
  std::vector<Path>   runningImageProcessingJobs() const;

  std::vector<Path> pendingMappingFolders() const;

  // todo: marking should be externalized to photobook.
  void markForDeletion(Path path);
  void removeMarkForDeletion(Path path);

  bool marked(Path path) const;

private:
  void onImageProcessed(Path key, Path root, Path full, Path medium, Path small,
                        int progressCap);

  ImportFoldersLogicListener                   *mListener = nullptr;
  PBDev::ThreadScheduler                       *mScheduler = nullptr;
  std::unordered_map<Path, std::pair<int, int>> mImageProcessingProgress;
  ThumbnailsProcessor                           mThumbnailsProcessor;
  std::unordered_map<Path, PBDev::SequentialTaskConsumer<MediaMapper>>
                           mMappingJobs;
  std::unordered_set<Path> mRemovalMarks;
  std::shared_ptr<Project> mProject = nullptr;
};
} // namespace PB