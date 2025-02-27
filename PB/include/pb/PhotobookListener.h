#pragma once

#include <pb/components/ImageToPaperTask.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/services/LutService.h>
#include <pb/services/ProgressService.h>
#include <pb/infra/Error.h>
#include <pb/infra/Traits.h>

namespace PB {
class PhotobookListener {
public:
  virtual ~PhotobookListener() = default;

  virtual void onProjectRead() = 0;
  virtual void onMetadataUpdated(std::string focusedProjectName) = 0;
  virtual void onPersistenceError(PBDev::Error) = 0;

  virtual void onExportFinished() = 0;
  virtual void onError(PBDev::Error) = 0;
  virtual void onStagedImageRemoved(std::vector<unsigned> removedIndexes) = 0;

  virtual void onMappingStarted(Path path) = 0;
  virtual void onMappingFinished(Path path) = 0;
  virtual void onCollageThumbnailsCreated() = 0;
  virtual void onMappingAborted(Path path) = 0;

  virtual void onImageUpdated(Path root, int row, int index) = 0;

  virtual void post(std::function<void()> f) = 0;

  virtual void onCollageCreated(GenericImagePtr newImage,
                                Path            thumbnailsLocation) = 0;

  virtual void onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image,
                             Path thumbnailsLocation) = 0;

  virtual void onProgressUpdate(PB::ProgressStatus status) = 0;

  [[deprecated]] virtual void onLutAdded(LutIconInfo iconInfo) = 0;

  virtual void onLutApplied(PBDev::LutApplicationId, GenericImagePtr,
                            Path thumbnailsLocation) = 0;
};

} // namespace PB
