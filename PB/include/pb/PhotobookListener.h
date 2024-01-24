#pragma once
#include <pb/ProgressManager.h>
#include <pb/tasks/ThumbnailsProcessor.h>
#include <pb/util/Util.h>

namespace PB {
class PhotobookListener {
public:
  virtual ~PhotobookListener() = default;

  virtual void onProjectRead() = 0;
  virtual void onProjectRenamed() = 0;
  virtual void onMetadataUpdated() = 0;
  virtual void onPersistenceError(PBDev::Error) = 0;

  virtual void onProgressUpdate(int, int) = 0;
  virtual void onExportProgressUpdate(int, int) = 0;
  virtual void onExportFinished() = 0;
  virtual void onError(PBDev::Error) = 0;
  virtual void
  onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                     int index = -1) = 0;
  virtual void onStagedImageRemoved(std::vector<unsigned> removedIndexes) = 0;

  virtual void onMappingStarted(Path path) = 0;
  virtual void onMappingFinished(Path path) = 0;
  virtual void onMappingAborted(Path path) = 0;

  virtual void onImageUpdated(Path root, int row, int index) = 0;

  virtual void post(std::function<void()> f) = 0;

  virtual void onProgressUpdate(std::vector<std::string> names,
                                PB::ProgressInfo         definedProgress,
                                PB::ProgressInfo         undefinedProgress) = 0;
};

} // namespace PB