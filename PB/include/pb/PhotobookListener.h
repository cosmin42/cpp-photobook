#pragma once
#include <pb/tasks/ThumbnailsProcessor.h>
#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace PB {
class PhotobookListener {
public:
  virtual void onProgressUpdate(Path, int, int) = 0;
  virtual void onExportProgressUpdate(int, int) = 0;
  virtual void onExportFinished() = 0;
  virtual void onError(PBDev::Error) = 0;
  virtual void onUnstagedImageAdded(Path rootPath, Path fullPath,
                                    Path mediumPath, Path smallPath,
                                    int position) = 0;
  virtual void
  onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                     int index = -1) = 0;
  virtual void onStagedImageRemoved(std::vector<unsigned> removedIndexes) = 0;

  virtual void onMappingStarted(Path path) = 0;
  virtual void onMappingFinished(Path path) = 0;
  virtual void onMappingAborted(Path path) = 0;

  virtual void post(std::function<void()> f) = 0;
};

class DashboardListener {
public:
  virtual void
  onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata> metadata) = 0;
  virtual void onProjectRead() = 0;
};

} // namespace PB