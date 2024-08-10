#pragma once

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

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override
  {
	return std::nullopt;
  }

  void onFinished(PBDev::MapReducerTaskId) override {}

private:
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
};
} // namespace PB
