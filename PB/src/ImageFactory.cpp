#include <pb/image/ImageFactory.h>

#include <pb/components/ThumbnailsTask.h>
#include <pb/image/ImageOperations.h>

namespace PB {

void ImageFactory::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ImageFactory::configureProject(
    std::shared_ptr<IdentifyableProject> project)
{
  mProject = project;
}

void ImageFactory::configureDurableHashService(
    std::shared_ptr<DurableHashService> durableHashService)
{
  mDurableHashService = durableHashService;
}

std::shared_ptr<RegularImageV2> ImageFactory::createRegularImage(Path path)
{
  auto regularImage =
      std::make_shared<RegularImageV2>(RegularImageV2::defaultHash(), path);
  return regularImage;
}

std::shared_ptr<RegularImageV2>
ImageFactory::createRegularImage(std::string hash)
{
  return std::make_shared<RegularImageV2>(hash, Path());
}

std::shared_ptr<TextImageV2> ImageFactory::createTextImage(Path        path,
                                                           std::string hash)
{
  auto project = mProject->second;

  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      project.paperSettings.width, project.paperSettings.height,
      {255, 255, 255})();

  Process::CVFontInfo fontInfo;
  fontInfo.color = {0, 0, 0};
  fontInfo.pixelSize = Process::pointsFromPixels(4, project.paperSettings.ppi);
  fontInfo.thickness = 8;

  image = PB::Process::addText(
      {project.paperSettings.width / 2, project.paperSettings.height / 2},
      path.stem().string(), fontInfo)(image);

  ThumbnailsTask::createThumbnails(image, mPlatformInfo, mProject, hash);

  auto textImage = std::make_shared<TextImageV2>(hash, path.stem().string());
  return textImage;
}

GenericImagePtr ImageFactory::createImage(Path path)
{
  PBDev::ProjectId projectId(mProject->first);

  if (std::filesystem::is_regular_file(path)) {
    return createRegularImage(path);
  }
  else if (std::filesystem::is_directory(path)) {
    auto coreHash = mDurableHashService->getHash(projectId, path);

    return createTextImage(path, coreHash);
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

GenericImagePtr ImageFactory::copyImage(GenericImagePtr image)
{
  auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

  auto largeFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::LARGE);
  auto mediumFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::MEDIUM);
  auto smallFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::SMALL);

  auto newLargeFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                     ThumbnailsSize::LARGE);
  auto newMediumFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                      ThumbnailsSize::MEDIUM);
  auto newSmallFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                     ThumbnailsSize::SMALL);

  std::filesystem::copy(largeFile, newLargeFile);
  std::filesystem::copy(mediumFile, newMediumFile);
  std::filesystem::copy(smallFile, newSmallFile);

  if (image->type() == ImageType::Regular) {
    auto regularImage = std::dynamic_pointer_cast<RegularImageV2>(image);

    return std::make_shared<RegularImageV2>(newHash, regularImage->original());
  }
  else if (image->type() == ImageType::Text) {
    auto textImage = std::dynamic_pointer_cast<TextImageV2>(image);

    return std::make_shared<TextImageV2>(newHash, textImage->text());
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

GenericImagePtr ImageFactory::weakCopyImage(GenericImagePtr image)
{
  auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

  auto largeFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::LARGE);
  auto mediumFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::MEDIUM);
  auto smallFile = mPlatformInfo->thumbnailByHash(
      mProject->first, image->hash(), ThumbnailsSize::SMALL);

  auto newLargeFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                     ThumbnailsSize::LARGE);
  auto newMediumFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                      ThumbnailsSize::MEDIUM);
  auto newSmallFile = mPlatformInfo->thumbnailByHash(mProject->first, newHash,
                                                     ThumbnailsSize::SMALL);

  if (image->type() == ImageType::Regular) {
    auto regularImage = std::dynamic_pointer_cast<RegularImageV2>(image);

    return std::make_shared<RegularImageV2>(newHash, regularImage->original());
  }
  else if (image->type() == ImageType::Text) {
    auto textImage = std::dynamic_pointer_cast<TextImageV2>(image);

    return std::make_shared<TextImageV2>(newHash, textImage->text());
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

} // namespace PB
