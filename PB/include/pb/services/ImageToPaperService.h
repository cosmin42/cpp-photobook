#pragma once

#include <pb/Platform.h>
#include <pb/components/ImageToPaperTask.h>
#include <pb/entities/GenericImage.h>
#include <pb/image/ImageFactory.h>
#include <pb/infra/TaskCruncher.h>
#include <pb/services/ProjectManagementService.h>

DECLARE_STRONG_UUID(ImageToPaperServiceId)

namespace PB::Service {

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

  void configureProjectManagementService(
      std::shared_ptr<ProjectManagementService> projectManagementService)
  {
    mProjectManagementService = projectManagementService;
  }

  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory)
  {
    mImageFactory = imageFactory;
  }

  void setImageToPaperServiceListener(ImageToPaperServiceListener *listener)
  {
    mListener = listener;
  }

  void map(PBDev::ImageToPaperServiceId,
           std::unordered_map<PBDev::ImageToPaperId, ImageToPaperData,
                              boost::hash<PBDev::ImageToPaperId>>
               originalImages);

  void copyImages(PBDev::ImageToPaperServiceId,
                  std::unordered_map<PBDev::ImageToPaperId, GenericImagePtr,
                                     boost::hash<PBDev::ImageToPaperId>>);

  void removeTask(PBDev::ImageToPaperServiceId id);

private:
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<TaskCruncher>             mTaskCruncher = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  std::shared_ptr<ImageFactory>             mImageFactory = nullptr;
  std::unordered_map<PBDev::ImageToPaperServiceId, ImageToPaperTask,
                     boost::hash<PBDev::ImageToPaperServiceId>>
      mTasks;

  ImageToPaperServiceListener *mListener = nullptr;
};
} // namespace PB::Service
