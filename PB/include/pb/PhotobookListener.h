#pragma once

#include <pb/entities/LutIconInfo.h>
#include <pb/components/ImageToPaperTask.h>
#include <pb/services/ProgressService.h>
#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace PB {
class PhotobookListener {
public:
  virtual ~PhotobookListener() = default;

  virtual void onProjectRead() = 0;
  virtual void onProjectRenamed() = 0;
  virtual void onMetadataUpdated() = 0;
  virtual void onPersistenceError(PBDev::Error) = 0;

  virtual void onExportFinished() = 0;
  virtual void onError(PBDev::Error) = 0;
  virtual void
  onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                     int index = -1) = 0;
  virtual void onStagedImageRemoved(std::vector<unsigned> removedIndexes) = 0;

  virtual void onMappingStarted(Path path) = 0;
  virtual void onMappingFinished(Path path) = 0;
  virtual void onCollageThumbnailsCreated() = 0;
  virtual void onMappingAborted(Path path) = 0;

  virtual void onImageUpdated(Path root, int row, int index) = 0;

  virtual void post(std::function<void()> f) = 0;

  virtual void onCollageCreated(unsigned                          index,
                                std::shared_ptr<PB::VirtualImage> newImage) = 0;

  virtual void onImageMapped(PBDev::ImageToPaperId         id,
                             std::shared_ptr<VirtualImage> image) = 0;

  virtual void onProgressUpdate(PB::ProgressStatus status) = 0;

  virtual void onLutAdded(LutIconInfo iconInfo) = 0;
};

} // namespace PB