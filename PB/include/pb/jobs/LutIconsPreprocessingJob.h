#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Platform.h>
#include <pb/components/MapReducer.h>
#include <pb/components/OGLEngine.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/image/ImageReader.h>

namespace PB::Job {
class LutIconsPreprocessingListener {
public:
  virtual ~LutIconsPreprocessingListener() = default;

  virtual void onLutIconsPreprocessingFinished(std::string, Path) = 0;
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

  void configureOriginalImage(std::shared_ptr<cv::Mat> image)
  {
    mOriginalImage = image;
  }

  void configureOGLEngine(std::shared_ptr<OGLEngine> oglEngine)
  {
    mOglEngine = oglEngine;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mIndex >= mLuts.size()) {
      return std::nullopt;
    }
    auto lutPath = mLuts.at(mIndex);
    mIndex++;

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    return std::make_optional<IdentifyableFunction>(taskId, [this, lutPath] {
      auto outImagePath = createTransformedImage(lutPath);
      auto lutName = extractNameFromPath(lutPath);
      mListener->onLutIconsPreprocessingFinished(lutName, outImagePath);
    });
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override {}

  unsigned taskCount() const override { return (unsigned)mLuts.size(); }

private:
  LutIconsPreprocessingListener *mListener = nullptr;
  std::shared_ptr<OGLEngine>     mOglEngine = nullptr;
  std::shared_ptr<PlatformInfo>  mPlatformInfo = nullptr;
  std::vector<Path>              mLuts;
  std::vector<Path>              mIcons;
  unsigned                       mIndex = 0;

  std::shared_ptr<cv::Mat> mOriginalImage = nullptr;

  std::string extractNameFromPath(Path path)
  {
    auto raw = path.stem().string();
    std::replace(raw.begin(), raw.end(), '_', ' ');
    for (size_t i = 0; i < raw.size(); i++) {
      if (i == 0 || raw[i - 1] == ' ') {
        raw[i] = (char)std::toupper(raw[i]);
      }
    }
    return raw;
  }

  Path newImageName()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    return mPlatformInfo->localStatePath / newImageName;
  }

  Path createTransformedImage(Path lutPath)
  {
    auto rgbImage = Process::extractRGBChannels(mOriginalImage);
    auto clone = Process::clone(rgbImage);
    auto lutData = Process::readLutData(lutPath);

    auto outImage = Process::clone(rgbImage);

    LutImageProcessingData lutImageProcessingData;
    lutImageProcessingData.inImage = clone;
    lutImageProcessingData.outImage = outImage;

    for (auto const &data : lutData) {
      lutImageProcessingData.lut.push_back(data);
    }
    mOglEngine->applyLut(lutImageProcessingData);

    auto outImagePath = newImageName();
    Process::writeImageOnDisk(lutImageProcessingData.outImage, outImagePath);
    return outImagePath;
  }
};
} // namespace PB
