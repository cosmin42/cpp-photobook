#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/MapReducer.h>
#include <pb/Platform.h>
#include <pb/image/ImageReader.h>

namespace PB {
class LutIconsPreprocessingListener {
public:
  virtual ~LutIconsPreprocessingListener() = default;

  virtual void onLutIconsPreprocessingFinished(Path) = 0;
};

class LutIconsPreprocessingJob final : public MapReducer {
public:
  LutIconsPreprocessingJob() = default;
  ~LutIconsPreprocessingJob() = default;

  void configureLuts(std::vector<Path> const &luts) { mLuts = luts; }

  void configureListener(LutIconsPreprocessingListener *listener)
  {
    mListener = listener;
  }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mIndex >= mLuts.size()) {
      return std::nullopt;
    }
    auto lutPath = mLuts.at(mIndex);

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    return std::make_optional<IdentifyableFunction>(
        taskId, [this, taskId, lutPath] {
          auto outImagePath = createTransformedImage(lutPath);
          mListener->onLutIconsPreprocessingFinished(outImagePath);
        });
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override {}

  unsigned taskCount() const override { return (unsigned)mLuts.size(); }

private:
  LutIconsPreprocessingListener *mListener = nullptr;
  std::shared_ptr<PlatformInfo>  mPlatformInfo = nullptr;
  std::vector<Path>              mLuts;
  std::vector<Path>              mIcons;
  unsigned                       mIndex = 0;
  static constexpr const char   *IMAGE_NAME = "singapore.jpg";
  static constexpr const char   *FOLDER_NAME = "others";

  std::shared_ptr<cv::Mat> mOriginalImage = nullptr;

  Path originalImagePath() const
  {
    return mPlatformInfo->installationPath / FOLDER_NAME / IMAGE_NAME;
  }

  Path newImageName()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    return mPlatformInfo->localStatePath / newImageName;
  }

  Path createTransformedImage(Path lutPath)
  {
    auto clone = Process::clone(mOriginalImage);
    clone = Process::applyLutInplace(clone, lutPath);

    auto outImagePath = newImageName();
    Process::writeImageOnDisk(clone, outImagePath);
    return outImagePath;
  }
};
} // namespace PB
