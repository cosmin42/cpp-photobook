#include <pb/services/ImageToPaperService.h>

namespace PB::Service {

void ImageToPaperService::toPaper(
    PBDev::ImageToPaperServiceId id,
    std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{

  auto &&task = ImageToPaperTask(mProject, originalImages);
  task.configurePlatformInfo(mPlatformInfo);
  task.setImageToPaperServiceListener(mListener);
  task.configureNoirMonitor(mNoirMonitor);
  mTasks.emplace(id, task);
  auto stopSource = mTaskCruncher->crunch("upl-to-spl-map", mTasks.at(id),
                                          PBDev::ProgressJobName{"to-paper"});
  UNUSED(stopSource);
}

void ImageToPaperService::copyImages(std::unordered_map<PBDev::ImageToPaperId, GenericImagePtr,
                       boost::hash<PBDev::ImageToPaperId>>
        images)
{
  for (auto &&[id, image] : images) {
    mTaskCruncher->crunch([this, id, image]() {
      auto newImage = mImageFactory->copyImage(image);
      mListener->onImageCopied(id, newImage);
    });
  }
}

void ImageToPaperService::removeTask(PBDev::ImageToPaperServiceId id)
{
  mTasks.erase(id);
}

} // namespace PB::Service
