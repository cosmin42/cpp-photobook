#include <pb/image/ImageFactory.h>

#include <pb/image/ImageOperations.h>
#include <pb/image/ImageReader.h>
#include <pb/tasks/ThumbnailsProcessor.h>

namespace PB {

void ImageFactory::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ImageFactory::configureProjectManagementSystem(
    std::shared_ptr<ProjectManagementSystem> projectManagementSystem)
{
  mProjectManagementSystem = projectManagementSystem;
}

void ImageFactory::configureDurableHashService(
    std::shared_ptr<DurableHashService> durableHashService)
{
  mDurableHashService = durableHashService;
}

std::shared_ptr<RegularImage> ImageFactory::createRegularImage(Path path)
{
  auto regularImage =
      std::make_shared<RegularImage>(defaultImageFrontend(), path);
  return regularImage;
}

std::shared_ptr<TextImage> ImageFactory::createTextImage(Path path,
                                                         Path hashPath)
{
  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  auto project = maybeProject->second;

  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      project.paperSettings.width, project.paperSettings.height,
      {255, 255, 255})();

  Process::CVFontInfo fontInfo;
  fontInfo.color = {0, 0, 0};
  fontInfo.pixelSize = Process::pointsFromPixels(24, project.paperSettings.ppi);
  fontInfo.thickness = 8;

  image = PB::Process::addText(
      {project.paperSettings.width / 2, project.paperSettings.height / 2},
      path.stem().string(), fontInfo)(image);

  Process::writeImageOnDisk(image, hashPath);

  auto textImage =
      std::make_shared<TextImage>(defaultImageFrontend(), hashPath);
  return textImage;
}

std::shared_ptr<VirtualImage> ImageFactory::createImage(Path path)
{
  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  PBDev::ProjectId projectId(maybeProject->first);

  if (std::filesystem::is_regular_file(path)) {
    return createRegularImage(path);
  }
  else if (std::filesystem::is_directory(path)) {
    auto coreHash = mDurableHashService->getHash(projectId, path);

    auto hashPath =
        mPlatformInfo->thumbnailByHash(maybeProject->first, coreHash);

    return createTextImage(path, hashPath);
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

std::shared_ptr<VirtualImage>
ImageFactory::copyImage(std::shared_ptr<VirtualImage> image)
{
  if (image->type() == VirtualImageType::Regular) {
    return std::make_shared<RegularImage>(image->frontend(), image->processed(),
                                          image->resources());
  }
  else if (image->type() == VirtualImageType::Text) {
    return std::make_shared<TextImage>(image->frontend(), image->processed(),
                                       image->resources());
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

std::shared_ptr<VirtualImage>
ImageFactory::mapImageToPaper(std::shared_ptr<VirtualImage> image,
                              Path                          hashPath)
{
  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto project = maybeProject->second;

  auto imageData = ImageReader().read(
      image->frontend().full, true,
      {project.paperSettings.width, project.paperSettings.height});

  std::shared_ptr<cv::Mat> singleColorImage = PB::Process::singleColorImage(
      project.paperSettings.width, project.paperSettings.height,
      {255, 255, 255})();

  PBDev::basicAssert(imageData != nullptr);

  PB::Process::overlap(imageData,
                       PB::Process::alignToCenter())(singleColorImage);

  auto [smallPath, mediumPath] = ThumbnailsProcessor::assembleOutputPaths(
      mPlatformInfo->localStatePath, 0, hashPath.stem().string(),
      boost::uuids::to_string(maybeProject->first));

  Process::writeImageOnDisk(singleColorImage, hashPath);

  Process::imageWriteThumbnail(project.paperSettings.width,
                               project.paperSettings.height, singleColorImage,
                               mediumPath, smallPath);

  ImageResources imageResources = {hashPath, mediumPath, smallPath,
                                   (unsigned)singleColorImage->cols,
                                   (unsigned)singleColorImage->rows};

  auto newImage = std::make_shared<RegularImage>(imageResources, hashPath);

  return newImage;
}

} // namespace PB