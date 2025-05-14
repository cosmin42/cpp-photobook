#pragma once

#include <vector>

#include <pb/Platform.h>
#include <pb/NoirMonitor.h>
#include <pb/components/MapReducer.h>
#include <pb/components/ThumbnailsTask.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/entities/CollageImage.h>
#include <pb/image/ImageOperations.h>
#include <pb/infra/FileSupport.h>
#include <pb/infra/Traits.h>
#include <pb/services/ProjectManagementService.h>
#include <queue>

namespace PB::Job {

class ThumbnailsJobListener {
public:
  virtual void imageProcessed(PBDev::ThumbnailsJobId jobId,
                              PBDev::ImageId         imageId,
                              GenericImagePtr        image) = 0;

  virtual void taskEnded() = 0;
};

class ThumbnailsJob : public MapReducer {
public:
  explicit ThumbnailsJob(PBDev::ThumbnailsJobId jobId,
                         std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                            boost::hash<PBDev::ImageId>>
                             placeholders)
      : mJobId(jobId), mPlaceholders(placeholders)
  {
    for (auto &[imageId, image] : mPlaceholders) {
      mImageIds.push(imageId);
    }
  }
  ~ThumbnailsJob() = default;

  void configureListener(ThumbnailsJobListener *listener)
  {
    mListener = listener;
  }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configureProject(IdentifiableProject project) { mProject = project; }

  void configureNoirMonitor(std::shared_ptr<NoirMonitor> noirMonitor)
  {
    mNoirMonitor = noirMonitor;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mImageIds.empty()) {
      return std::nullopt;
    }

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    auto imageId = mImageIds.front();
    auto image = mPlaceholders.at(imageId);

    mImageIds.pop();

    return std::make_optional(IdentifyableFunction{
        taskId, [this, imageId, image]() {
          mNoirMonitor->start("Thumbnails creation", boost::uuids::to_string(imageId.raw()));
          auto processedImage = processDiscriminator(image);
          mNoirMonitor->stop("Thumbnails creation", boost::uuids::to_string(imageId.raw()));
          mListener->imageProcessed(mJobId, imageId, processedImage);
        }});
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override
  {
    if (mImageIds.empty()) {
      mListener->taskEnded();
    }
  }

  unsigned taskCount() const override { return (unsigned)mPlaceholders.size(); }

private:
  ThumbnailsJobListener        *mListener = nullptr;
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
  std::shared_ptr<NoirMonitor>  mNoirMonitor = nullptr;
  IdentifiableProject           mProject = nullptr;
  PBDev::ThumbnailsJobId        mJobId;
  const std::unordered_map<PBDev::ImageId, GenericImagePtr,
                           boost::hash<PBDev::ImageId>>
      mPlaceholders;

  std::queue<PBDev::ImageId> mImageIds;

  GenericImagePtr processRegularImage(Path path)
  {
    ThumbnailsTask task(path);
    task.configurePlatformInfo(mPlatformInfo);
    task.configureProject(mProject);
    task.configureNoirMonitor(mNoirMonitor);
    auto hash = task.createThumbnails();
    return std::make_shared<RegularImageV2>(hash, path);
  }

  GenericImagePtr processTextImage(Path path)
  {
    auto temporaryImagePath = mPlatformInfo->newTemporaryImage(mProject->id);
    auto directoryName = path.filename().string();
    auto image =
        Process::createTextImage(mProject->value.paperSettings, directoryName);
    PB::infra::writeImageOnDisk(image, temporaryImagePath);
    ThumbnailsTask task(temporaryImagePath);
    task.configurePlatformInfo(mPlatformInfo);
    task.configureProject(mProject);
    task.configureNoirMonitor(mNoirMonitor);
    auto hash = task.createThumbnails();
    std::filesystem::remove(temporaryImagePath);
    return std::make_shared<TextImageV2>(hash, directoryName);
  }

  GenericImagePtr processDiscriminator(GenericImagePtr image)
  {
    if (image->type() == ImageType::Regular) {
      auto regularImage = std::dynamic_pointer_cast<RegularImageV2>(image);
      return processRegularImage(regularImage->original());
    }
    else if (image->type() == ImageType::Text) {
      auto textImage = std::dynamic_pointer_cast<TextImageV2>(image);
      return processTextImage(textImage->name());
    }
    else if (image->type() == ImageType::Collage) {
      auto collageImage = std::dynamic_pointer_cast<CollageImage>(image);
      return processTextImage(collageImage->name());
    }
    PBDev::basicAssert(false);
    return {};
  }
};

} // namespace PB::Job
