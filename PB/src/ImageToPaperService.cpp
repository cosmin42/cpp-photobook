#include <pb/ImageToPaperService.h>

namespace PB {

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId id,
    std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{

  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  auto &&task =
      ImageToPaperTask(PBDev::ProjectId(maybeProject->first),
                       maybeProject->second.paperSettings, originalImages);
  task.configurePlatformInfo(mPlatformInfo);
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
  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  auto &&task = ImageToPaperTask(
      PBDev::ProjectId(maybeProject->first), maybeProject->second.paperSettings,
      std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperId>>{originalImage});

  task.configurePlatformInfo(mPlatformInfo);
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
