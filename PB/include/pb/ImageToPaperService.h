#pragma once

#include <pb/ImageToPaperTask.h>
#include <pb/Platform.h>
#include <pb/TaskCruncher.h>
#include <pb/image/VirtualImage.h>

DECLARE_STRONG_UUID(ImageToPaperServiceId)

namespace PB {

class ImageToPaperService final {
public:
  ImageToPaperService() = default;
  ~ImageToPaperService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
  {
    mTaskCruncher = taskCruncher;
  }

  void configurePersistenceService(
      std::shared_ptr<PersistenceService> persistenceService)
  {
    mPersistenceService = persistenceService;
  }

  void configureProject(std::shared_ptr<Project> project)
  {
    mProject = project;
  }

  void setImageToPaperServiceListener(ImageToPaperServiceListener *listener)
  {
    mListener = listener;
  }

  void
  map(std::unordered_map<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperId>>
          originalImages);

  void
  map(std::pair<PBDev::ImageToPaperId, std::shared_ptr<VirtualImage>> image);

  void removeTask(PBDev::ImageToPaperServiceId id);

private:
  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  std::shared_ptr<TaskCruncher>       mTaskCruncher = nullptr;
  std::shared_ptr<PersistenceService> mPersistenceService = nullptr;
  std::shared_ptr<Project>            mProject = nullptr;
  std::unordered_map<PBDev::ImageToPaperServiceId, ImageToPaperTask> mTasks;

  ImageToPaperServiceListener *mListener = nullptr;
};
} // namespace PB
