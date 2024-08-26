#pragma once

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/GPUAcceleratedImageOperations.h>
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

  void configureOriginalImage(std::shared_ptr<cv::Mat> image)
  {
    mOriginalImage = image;
  }

  void configureGPUAcceleratedImageOperations(
      std::shared_ptr<GPUAcceleratedImageOperations>
          gpuAcceleratedImageOperations)
  {
    mGPUAcceleratedImageOperations = gpuAcceleratedImageOperations;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mInitFlag && mIndex >= mLuts.size() /* && mDestroyFlag*/) {
      return std::nullopt;
    }

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    if (!mInitFlag) {
      mInitFlag = true;
      return std::make_optional<IdentifyableFunction>(taskId, [this] {
        mGPUAcceleratedImageOperations->initOpenGL();
        mGPUAcceleratedImageOperations->createFrameBuffer();
        
      });
    }

    /*
    if (!mDestroyFlag && mIndex >= mLuts.size()) {
      mDestroyFlag = true;
      return std::make_optional<IdentifyableFunction>(
          taskId, [this] { mGPUAcceleratedImageOperations->cleanup(); });
    }
    */
    auto lutPath = mLuts.at(mIndex);
    mIndex++;

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
  bool                           mInitFlag = false;
  bool                           mDestroyFlag = false;

  std::shared_ptr<cv::Mat> mOriginalImage = nullptr;
  std::shared_ptr<GPUAcceleratedImageOperations>
      mGPUAcceleratedImageOperations = nullptr;

  Path newImageName()
  {
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    return mPlatformInfo->localStatePath / newImageName;
  }

  Path createTransformedImage(Path lutPath)
  {
    // auto clone = Process::clone(mOriginalImage);
    auto lutData = Process::readLutData3D(lutPath);

    // clone = Process::applyLutInplace(clone, lutData);

    // auto outImagePath = newImageName();
    // Process::writeImageOnDisk(clone, outImagePath);

    auto clone = Process::clone(mOriginalImage);

    auto rgbImage = Process::extractRGBChannels(clone);

    auto outImage = mGPUAcceleratedImageOperations->processImage(
        rgbImage, lutData, lutData.size());

    auto rgbaImage = Process::completeWithAlphaChannel(outImage);
    /*
    auto lutData = Process::readLutData(lutPath);

    clone = Process::applyLutInplace(clone, lutData);
    */
    auto outImagePath = newImageName();

    Process::writeImageOnDisk(outImage, outImagePath);
    return outImagePath;
  }
};
} // namespace PB
