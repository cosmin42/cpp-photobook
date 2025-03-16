#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Platform.h>
#include <pb/components/MapReducer.h>
#include <pb/components/OGLEngine.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/infra/FileSupport.h>

using namespace PB::Service;

namespace PB::Job {
class LutIconsPreprocessingListener {
public:
  virtual ~LutIconsPreprocessingListener() = default;

  virtual void onLutIconsPreprocessingFinished(std::string, Path, Path) = 0;
};

class LutIconsPreprocessingJob final : public MapReducer {
public:
  static std::string extractNameFromPath(Path path)
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

  void configureImagePath(Path imagePath) { mOriginalImage = imagePath; }

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
      mListener->onLutIconsPreprocessingFinished(lutName, lutPath,
                                                 outImagePath);
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

  Path mOriginalImage;

  Path newImageName()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    return mPlatformInfo->processedLutsPath() / newImageName;
  }

  Path createTransformedImage(Path lutPath)
  {
    auto lutData = infra::readLutData(lutPath);
    auto outImagePath = newImageName();

    LutImageProcessingData lutImageProcessingData;
    lutImageProcessingData.inImage = mOriginalImage;
    lutImageProcessingData.outImage = outImagePath;

    for (auto const &data : lutData) {
      lutImageProcessingData.lut.push_back(
          cv::Vec4f(data[0], data[1], data[2], 1.0));
    }
    mOglEngine->applyLut(lutImageProcessingData);

    return outImagePath;
  }
};
} // namespace PB::Job
