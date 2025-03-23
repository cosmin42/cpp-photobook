#pragma once

#include <pb/components/ImageToPaperTask.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/infra/Error.h>
#include <pb/infra/Traits.h>
#include <pb/services/EffectsService.h>
#include <pb/services/LutService.h>
#include <pb/services/ProgressService.h>

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
  virtual void onMappingFinished(Path path, unsigned imagesCount) = 0;
  virtual void onCollageThumbnailsCreated() = 0;
  virtual void onMappingAborted(Path path) = 0;

  virtual void onImageUpdated(Path root, int row, int index) = 0;

  virtual void post(std::function<void()> f) = 0;

  virtual void onCollageCreated(GenericImagePtr newImage,
                                Path            thumbnailsLocation) = 0;

  virtual void onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image,
                             Path thumbnailsLocation) = 0;

  virtual void onImageCopied(PBDev::ImageToPaperId imageId,
                             PB::GenericImagePtr   image,
                             Path                  thumbnailsLocation) = 0;

  virtual void onProgressUpdate(PB::ProgressStatus status) = 0;

  [[deprecated]] virtual void onLutAdded(LutIconInfo iconInfo) = 0;

  virtual void onLutApplied(PBDev::LutApplicationId, GenericImagePtr, Path) = 0;

  virtual void onLutAppliedInMemory(PBDev::LutApplicationId,
                                    std::shared_ptr<cv::Mat>) = 0;

  virtual void onLutAppliedOnDiskInplace(PBDev::LutApplicationId) = 0;

  virtual void onLutAppliedOnDisk(PBDev::LutApplicationId, GenericImagePtr,
                                  Path) = 0;

  virtual void onEffectsApplied(PBDev::EffectId effectId,
                                GenericImagePtr image, Path thumbnailsPath) = 0;
  virtual void onEffectsAppliedInplace(PBDev::EffectId effectId) = 0;

  virtual void onSearchingFinished(Path root,
    std::unordered_map<PBDev::ImageId, GenericImagePtr,
                       boost::hash<PBDev::ImageId>>
        placeholders, Path) = 0;
};

} // namespace PB
