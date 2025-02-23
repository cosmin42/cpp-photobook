#include <pb/services/ImageToPaperService.h>

namespace PB::Service {

void ImageToPaperService::map(
    PBDev::ImageToPaperServiceId id,
    std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                       boost::hash<PBDev::ImageToPaperId>>
        originalImages)
{

  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  auto &&task =
      ImageToPaperTask(mProjectManagementService,
                       maybeProject->second.paperSettings, originalImages);
  task.configurePlatformInfo(mPlatformInfo);
  task.setImageToPaperServiceListener(mListener);
  mTasks.emplace(id, task);
  auto stopSource = mTaskCruncher->crunch("upl-to-spl-map", mTasks.at(id),
                                          PBDev::ProgressJobName{"to-paper"});
  UNUSED(stopSource);
}

void ImageToPaperService::removeTask(PBDev::ImageToPaperServiceId id)
{
  mTasks.erase(id);
}

} // namespace PB::Service
