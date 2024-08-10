#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/MapReducer.h>
#include <pb/Platform.h>
#include <pb/image/VirtualImage.h>

DECLARE_STRONG_UUID(ImageToPaperServiceId)

namespace PB {

class ImageToPaperTask final : public MapReducer {
public:
  explicit ImageToPaperTask(
      std::unordered_map<PBDev::ImageToPaperServiceId,
                         std::shared_ptr<VirtualImage>,
                         boost::hash<PBDev::ImageToPaperServiceId>>
          originalImages)
      : MapReducer()
  {
  }

  ~ImageToPaperTask() override = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configurePersistenceService(
      std::shared_ptr<PersistenceService> persistenceService)
  {
    mPersistenceService = persistenceService;
  }

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override
  {
    return std::nullopt;
  }

  void onFinished(PBDev::MapReducerTaskId) override {}

private:
  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  std::shared_ptr<PersistenceService> mPersistenceService = nullptr;

  Path GetNewImagePath()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    Path projectThumbnailsRoot =
        mPlatformInfo->localStatePath / "th" /
        boost::uuids::to_string(mPersistenceService->currentProjectUUID());

    return projectThumbnailsRoot / newImageName;
  }
};
} // namespace PB
