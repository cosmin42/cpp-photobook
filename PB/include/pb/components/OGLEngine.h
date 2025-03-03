#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <pb/Platform.h>
#ifdef __APPLE__
#include <pb/infra/ThreadScheduler.h>
#endif
#include <pb/components/VulkanManager.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/infra/TSQueue.h>
#include <pb/infra/Traits.h>

DECLARE_STRONG_STRING(OGLRenderId)

namespace PB::Service {

class OGLEngine final {
public:
  ~OGLEngine() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configVulkanManager(std::shared_ptr<VulkanManager> vulkanManager);

  void start();
  void stop();

  bool isWorking() const { return mWorking; }

  void applyLut(LutImageProcessingData const &imageProcessingData);
  void applyLutInMemory(LutInMemoryData const &imageProcessingData);

private:
  void loadPrograms();

  void mainloop();

  void loadTextureAndRender(ImageProcessingData const &imageProcessingData);

#ifdef __APPLE__
  PBDev::ThreadScheduler *mThreadScheduler = nullptr;
#endif

  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  TSQueue<ImageProcessingData>    mWorkQueue;
  std::jthread                    mThread;
  std::stop_token                 mStopToken;
  std::stop_source                mStopSource;

  std::mutex              mWorkMutex;
  std::condition_variable mFinishedWorkCondition;
  bool                    mFinishedWork = false;
  bool                    mWorking = false;

  std::unordered_map<std::string, sk_sp<SkRuntimeEffect>> mPrograms;

  std::shared_ptr<SkRuntimeShaderBuilder> mShaderBuilder = nullptr;

  const std::unordered_map<std::string, Path> FRAGMENT_SHADERS_PATHS = {
      {"lut", Path("shaders") / "lut.sksl"}};

  std::shared_ptr<VulkanManager> mVulkanManager = nullptr;
};
} // namespace PB::Service
