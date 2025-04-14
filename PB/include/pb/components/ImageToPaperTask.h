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
#include <pb/entities/TextImageV2.h>
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
      IdentifiableProject project,
      std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                         boost::hash<PBDev::ImageToPaperId>>
          originalImages)
      : MapReducer(), mProject(project), mOriginalImages(originalImages)
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
  static std::string shortName(GenericImagePtr image)
  {
    if (image->type() == ImageType::Regular) {
      auto regularImage = std::dynamic_pointer_cast<RegularImageV2>(image);
      return regularImage->original().filename().string();
    }
    else if (image->type() == ImageType::Text) {
      auto textImage = std::dynamic_pointer_cast<TextImageV2>(image);
      return textImage->text();
    }
    else {
      PBDev::basicAssert(false);
      return "";
    }
  }

  std::shared_ptr<PlatformInfo>      mPlatformInfo = nullptr;
  std::vector<PBDev::ImageToPaperId> mImageIds;
  unsigned                           mImageIndex = 0;

  IdentifiableProject mProject = nullptr;

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

    auto imagePath =
        mPlatformInfo->thumbnailByHash(mProject->id, image->hash());

    auto imageData = infra::loadImageToCvMat(imagePath);
    PBDev::basicAssert(imageData != nullptr);

    auto paperSettings = mProject->value.paperSettings;

    auto newImageSize =
        Geometry::resizeBox({imageData->cols, imageData->rows},
                            {paperSettings.width, paperSettings.height},
                            overlapType, Geometry::ScalePolicy::OnlyDown);

    auto maybeNewImage = PB::Process::resize(imageData, newImageSize, false);

    PBDev::basicAssert(maybeNewImage != nullptr);

    std::shared_ptr<cv::Mat> singleColorImage = PB::Process::singleColorImage(
        paperSettings.width, paperSettings.height, backgroundColor)();

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
      UNUSED(e);
      PB_UNREACHABLE;
    }

  croppedSource.copyTo(
        singleColorImage->operator()(cv::Rect(dstOrigin, sizeToCopy)));

    auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

    auto maybeNewHash = ThumbnailsTask::createThumbnails(
        singleColorImage, mPlatformInfo, mProject, newHash);

    PBDev::basicAssert(maybeNewHash == newHash);

    auto descriptiveShortName = shortName(image);
    
    return std::make_shared<RegularImageV2>(newHash, descriptiveShortName);
  }
};
} // namespace PB
