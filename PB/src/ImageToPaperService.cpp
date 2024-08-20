#include <pb/ImageToPaperService.h>

namespace PB {

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId id,
    std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{
  auto &&task = ImageToPaperTask(originalImages);
  task.configurePersistenceService(mPersistenceService);
  task.configurePlatformInfo(mPlatformInfo);
  task.configureProject(mProject);
  task.setImageToPaperServiceListener(mListener);
  mTasks.emplace(id, task);
  mTaskCruncher->crunch("upl-to-spl-map", mTasks.at(id),
                        PBDev::ProgressJobName{"to-paper"});
}

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId id,
    std::pair<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>>
        originalImage)
{
  auto &&task = ImageToPaperTask(
      std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperId>>{originalImage});

  task.configurePersistenceService(mPersistenceService);
  task.configurePlatformInfo(mPlatformInfo);
  task.configureProject(mProject);
  task.setImageToPaperServiceListener(mListener);
  mTasks.emplace(id, task);
  mTaskCruncher->crunch("upl-to-spl-map", mTasks.at(id),
                        PBDev::ProgressJobName{"to-paper"});
}

void ImageToPaperService::removeTask(PBDev::ImageToPaperServiceId id)
{
  mTasks.erase(id);
}

} // namespace PB
