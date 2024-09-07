#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/MapReducer.h>
#include <pb/Platform.h>
#include <pb/ProjectManagementSystem.h>
#include <pb/image/ImageReader.h>
#include <pb/image/VirtualImage.h>
#include <pb/persistence/PersistenceService.h>
#include <pb/project/Project.h>
#include <pb/tasks/ThumbnailsProcessor.h>

DECLARE_STRONG_UUID(ImageToPaperId)

namespace PB {

class ImageToPaperServiceListener {
public:
  virtual void onImageMapped(PBDev::ImageToPaperId         id,
                             std::shared_ptr<VirtualImage> image) = 0;
};

class ImageToPaperTask final : public MapReducer {
public:
  explicit ImageToPaperTask(
      std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperId>>
          originalImages)
      : MapReducer(), mOriginalImages(originalImages)
  {
    for (const auto &[id, image] : originalImages) {
      mImageIds.push_back(id);
    }
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

  void configureProjectManagementSystem(
      std::shared_ptr<ProjectManagementSystem> projectManagementSystem)
  {
    mProjectManagementSystem = projectManagementSystem;
  }

  void setImageToPaperServiceListener(ImageToPaperServiceListener *listener)
  {
    mListener = listener;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mImageIndex >= mImageIds.size()) {
      return std::nullopt;
    }

    auto result = getFunction(mImageIndex);
    mImageIndex++;
    return result;
  }

  void onTaskFinished(PBDev::MapReducerTaskId taskId) override
  {
    auto imageId = mImageTaskAssociation.at(taskId);

    mListener->onImageMapped(imageId, mResultImages.at(imageId));
  }

  unsigned taskCount() const override { return (unsigned)mImageIds.size(); }

private:
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementSystem> mProjectManagementSystem = nullptr;
  std::shared_ptr<Project>                 mProject = nullptr;
  std::vector<PBDev::ImageToPaperId>       mImageIds;
  unsigned                                 mImageIndex = 0;

  ImageToPaperServiceListener *mListener = nullptr;

  std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                     boost::hash<PBDev::ImageToPaperId>>
      mOriginalImages;

  std::unordered_map<PBDev::MapReducerTaskId, PBDev::ImageToPaperId,
                     boost::hash<PBDev::MapReducerTaskId>>
      mImageTaskAssociation;

  std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                     boost::hash<PBDev::ImageToPaperId>>
      mResultImages;

  IdentifyableFunction getFunction(unsigned index)
  {
    auto imageId = mImageIds.at(index);

    auto originalImage = mOriginalImages.at(imageId);

    auto taskId = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());

    mImageTaskAssociation.emplace(taskId, imageId);

    return {taskId, [this, originalImage{originalImage}, imageId{imageId}]() {
              auto paperImage = CreatePaperImage(originalImage);
              mResultImages[imageId] = paperImage;
            }};
  }

  Path GetNewImagePath()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    auto projectId = mProjectManagementSystem->maybeLoadedProjectInfo()->first;

    Path projectThumbnailsRoot = mPlatformInfo->localStatePath / "th" /
                                 boost::uuids::to_string(projectId);

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

    auto projectId = mProjectManagementSystem->maybeLoadedProjectInfo()->first;

    auto [smallPath, mediumPath] = ThumbnailsProcessor::assembleOutputPaths(
        mPlatformInfo->localStatePath, 0, hashPath.stem().string(),
        boost::uuids::to_string(projectId));

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
