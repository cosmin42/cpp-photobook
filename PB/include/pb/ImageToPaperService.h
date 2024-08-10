#pragma once

#include <pb/ImageToPaperTask.h>
#include <pb/Platform.h>
#include <pb/TaskCruncher.h>
#include <pb/image/VirtualImage.h>

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

  void map(std::unordered_map<PBDev::ImageToPaperServiceId,
                              std::shared_ptr<VirtualImage>,
                              boost::hash<PBDev::ImageToPaperServiceId>>
               originalImages);

  void
  map(std::pair<PBDev::ImageToPaperServiceId, std::shared_ptr<VirtualImage>>
          image);

private:
  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  std::shared_ptr<TaskCruncher>       mTaskCruncher = nullptr;
  std::shared_ptr<PersistenceService> mPersistenceService = nullptr;
  std::shared_ptr<Project>            mProject = nullptr;
  ImageToPaperServiceListener        *mListener = nullptr;
};
} // namespace PB
