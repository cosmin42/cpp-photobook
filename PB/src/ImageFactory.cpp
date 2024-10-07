#include <pb/image/ImageFactory.h>

#include <pb/image/ImageOperations.h>
#include <pb/image/ImageReader.h>

namespace PB {

void ImageFactory::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ImageFactory::configureProjectManagementService(
    std::shared_ptr<ProjectManagementService> projectManagementService)
{
  mProjectManagementService = projectManagementService;
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

std::shared_ptr<TextImageV2> ImageFactory::createTextImage(Path path,
                                                           Path hashPath)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
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

  auto textImage = std::make_shared<TextImageV2>("", path.stem().string());
  return textImage;
}

GenericImagePtr ImageFactory::createImage(Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  PBDev::ProjectId projectId(maybeProject->first);

  if (std::filesystem::is_regular_file(path)) {
    return createRegularImage(path);
  }
  else if (std::filesystem::is_directory(path)) {
    auto coreHash = mDurableHashService->getHash(projectId, path);

    auto hashPath =
        mPlatformInfo->thumbnailByHash(maybeProject->first, coreHash, ".jpg");

    return createTextImage(path, hashPath);
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

} // namespace PB