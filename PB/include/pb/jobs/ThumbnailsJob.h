#pragma once

#include <vector>

#include <pb/Platform.h>
#include <pb/components/MapReducer.h>
#include <pb/components/ThumbnailsTask.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/image/ImageOperations.h>
#include <pb/infra/FileSupport.h>
#include <pb/infra/Traits.h>
#include <pb/services/ProjectManagementService.h>

namespace PB::Job {

class ThumbnailsJobListener {
public:
  virtual void imageProcessed(PBDev::ThumbnailsJobId jobId,
                              PBDev::ImageId         imageId,
                              GenericImagePtr        image) = 0;
};

class ThumbnailsJob : public MapReducer {
public:
  explicit ThumbnailsJob(PBDev::ThumbnailsJobId jobId,
                         std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                            boost::hash<PBDev::ImageId>>
                             placeholders)
      : mJobId(jobId), mPlaceholders(placeholders),
        mIterator(mPlaceholders.begin())
  {
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

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mIterator == mPlaceholders.end()) {
      return std::nullopt;
    }

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    return std::make_optional(IdentifyableFunction{
        taskId, [this, imageId{mIterator->first}, image{mIterator->second}]() {
          auto processedImage = processDiscriminator(image);
          mListener->imageProcessed(mJobId, imageId, processedImage);
        }});
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override {}

  unsigned taskCount() const override { return (unsigned)mPlaceholders.size(); }

private:
  ThumbnailsJobListener        *mListener = nullptr;
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
  IdentifiableProject           mProject = nullptr;
  PBDev::ThumbnailsJobId        mJobId;
  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>
      mPlaceholders;

  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>::iterator mIterator;

  GenericImagePtr processRegularImage(Path path)
  {
    ThumbnailsTask task(path);
    task.configurePlatformInfo(mPlatformInfo);
    task.configureProject(mProject);
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
      return processTextImage(textImage->text());
    }
    PBDev::basicAssert(false);
    return {};
  }
};

} // namespace PB::Job
