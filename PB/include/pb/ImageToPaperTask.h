#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/MapReducer.h>
#include <pb/Platform.h>
#include <pb/image/ImageReader.h>
#include <pb/image/VirtualImage.h>
#include <pb/persistence/PersistenceService.h>
#include <pb/project/Project.h>
#include <pb/tasks/ThumbnailsProcessor.h>

DECLARE_STRONG_UUID(ImageToPaperServiceId)

namespace PB {

class ImageToPaperTask final : public MapReducer {
public:
  explicit ImageToPaperTask(
      std::unordered_map<PBDev::ImageToPaperServiceId,
                         std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperServiceId>>
          originalImages)
      : MapReducer()
  {
  }

  ~ImageToPaperTask() override = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configureProject(std::shared_ptr<Project> project)
  {
    mProject = project;
  }

  void configurePersistenceService(
      std::shared_ptr<PersistenceService> persistenceService)
  {
    mPersistenceService = persistenceService;
  }

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override
  {
    return std::nullopt;
  }

  void onFinished(PBDev::MapReducerTaskId) override {}

private:
  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  std::shared_ptr<PersistenceService> mPersistenceService = nullptr;
  std::shared_ptr<Project>            mProject = nullptr;

  Path GetNewImagePath()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    Path projectThumbnailsRoot =
        mPlatformInfo->localStatePath / "th" /
        boost::uuids::to_string(mPersistenceService->currentProjectUUID());

    return projectThumbnailsRoot / newImageName;
  }

  std::shared_ptr<VirtualImage>
  CreatePaperImage(std::shared_ptr<VirtualImage> image)
  {
    auto hashPath = GetNewImagePath();

    auto imageData = ImageReader().read(
        image->frontend().full, true,
        {mProject->paperSettings.width, mProject->paperSettings.height});

    std::shared_ptr<cv::Mat> singleColorImage = PB::Process::singleColorImage(
        mProject->paperSettings.width, mProject->paperSettings.height,
        {255, 255, 255})();

    PBDev::basicAssert(imageData != nullptr);

    PB::Process::overlap(imageData,
                         PB::Process::alignToCenter())(singleColorImage);

    auto [smallPath, mediumPath] = ThumbnailsProcessor::assembleOutputPaths(
        mPlatformInfo->localStatePath, 0, hashPath.stem().string(),
        boost::uuids::to_string(mPersistenceService->currentProjectUUID()));

    Process::writeImageOnDisk(singleColorImage, hashPath);

    Process::imageWriteThumbnail(mProject->paperSettings.width,
                                 mProject->paperSettings.height,
                                 singleColorImage, mediumPath, smallPath);

    ImageResources imageResources = {hashPath, mediumPath, smallPath,
                                     (unsigned)singleColorImage->cols,
                                     (unsigned)singleColorImage->rows};

    auto newImage = std::make_shared<RegularImage>(imageResources, hashPath);

    return newImage;
  }
};
} // namespace PB
