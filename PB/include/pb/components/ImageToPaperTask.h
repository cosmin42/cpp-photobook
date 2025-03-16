#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Platform.h>
#include <pb/components/MapReducer.h>
#include <pb/components/Project.h>
#include <pb/components/ThumbnailsTask.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/image/ImageReader.h>
#include <pb/infra/FileSupport.h>
#include <pb/services/ProjectManagementService.h>

DECLARE_STRONG_UUID(ImageToPaperId)

namespace PB {

struct ImageToPaperData {
  PB::GenericImagePtr   image;
  cv::Scalar            backgroundColor;
  Geometry::OverlapType overlapType;
};

class ImageToPaperServiceListener {
public:
  virtual void onImageMapped(PBDev::ImageToPaperId id,
                             GenericImagePtr       image) = 0;
  virtual void onImageCopied(PBDev::ImageToPaperId id,
                             GenericImagePtr       image) = 0;
};

class ImageToPaperTask final : public MapReducer {
public:
  explicit ImageToPaperTask(
      std::shared_ptr<ProjectManagementService> projectManagementService,
      PaperSettings                             paperSettings,
      std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                         boost::hash<PBDev::ImageToPaperId>>
          originalImages)
      : MapReducer(), mPaperSettings(paperSettings),
        mProjectManagementService(projectManagementService),
        mOriginalImages(originalImages)
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
  std::shared_ptr<PlatformInfo>      mPlatformInfo = nullptr;
  std::vector<PBDev::ImageToPaperId> mImageIds;
  unsigned                           mImageIndex = 0;
  PaperSettings                      mPaperSettings;

  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;

  ImageToPaperServiceListener *mListener = nullptr;

  std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                     boost::hash<PBDev::ImageToPaperId>>
      mOriginalImages;

  std::unordered_map<PBDev::MapReducerTaskId, PBDev::ImageToPaperId,
                     boost::hash<PBDev::MapReducerTaskId>>
      mImageTaskAssociation;

  std::unordered_map<PBDev::ImageToPaperId, GenericImagePtr,
                     boost::hash<PBDev::ImageToPaperId>>
      mResultImages;

  IdentifyableFunction getFunction(unsigned index)
  {
    auto imageId = mImageIds.at(index);

    auto originalImage = mOriginalImages.at(imageId).image;
    auto backgroundColor = mOriginalImages.at(imageId).backgroundColor;
    auto overlapType = mOriginalImages.at(imageId).overlapType;

    auto taskId = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());

    mImageTaskAssociation.emplace(taskId, imageId);

    return {taskId,
            [this, originalImage{originalImage}, imageId{imageId},
             backgroundColor{backgroundColor}, overlapType{overlapType}]() {
              auto paperImage =
                  CreatePaperImage(originalImage, backgroundColor, overlapType);
              mResultImages[imageId] = paperImage;
            }};
  }

  GenericImagePtr CreatePaperImage(GenericImagePtr       image,
                                   cv::Scalar            backgroundColor,
                                   Geometry::OverlapType overlapType)
  {
    auto maybeProjectInfo = mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeProjectInfo != nullptr);

    auto imagePath =
        mPlatformInfo->thumbnailByHash(maybeProjectInfo->first, image->hash());

    auto imageData = infra::loadImageToCvMat(imagePath);
    PBDev::basicAssert(imageData != nullptr);

    auto newImageSize =
        Geometry::resizeBox({imageData->cols, imageData->rows},
                            {mPaperSettings.width, mPaperSettings.height},
                            overlapType, Geometry::ScalePolicy::OnlyDown);

    auto maybeNewImage = PB::Process::resize(imageData, newImageSize, false);

    PBDev::basicAssert(maybeNewImage != nullptr);

    std::shared_ptr<cv::Mat> singleColorImage = PB::Process::singleColorImage(
        mPaperSettings.width, mPaperSettings.height, backgroundColor)();

    PBDev::basicAssert(imageData != nullptr);

    auto [srcOrigin, dstOrigin, sizeToCopy] =
        Geometry::overlapCenter(newImageSize, singleColorImage->size());

    cv::Rect sourceROI(srcOrigin.x, srcOrigin.y, sizeToCopy.width,
                       sizeToCopy.height);

    cv::Mat croppedSource;
    try {
      croppedSource = (*maybeNewImage)(sourceROI);
    }
    catch (cv::Exception &e) {
      PBDev::basicAssert(false);
    }

    croppedSource.copyTo(
        singleColorImage->operator()(cv::Rect(dstOrigin, sizeToCopy)));

    auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

    auto maybeNewHash = ThumbnailsTask::createThumbnails(
        singleColorImage, mPlatformInfo, mProjectManagementService, newHash);

    PBDev::basicAssert(maybeNewHash == newHash);
    // TODO: Full is not original here, improve this
    return std::make_shared<RegularImageV2>(newHash, image->full());
  }
};
} // namespace PB
