#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <include/effects/SkRuntimeEffect.h>
#pragma warning(pop)

#include <pb/Platform.h>
#ifdef __APPLE__
#include <pb/components/ThreadScheduler.h>
#endif
#include <pb/components/TSQueue.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_STRING(OGLRenderId)

namespace PB::Service {
class OGLEngine final {
public:
  ~OGLEngine() = default;

#ifdef __APPLE__
  void configureThreadScheduler(PBDev::ThreadScheduler *threadScheduler);
#endif
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void start();
  void stop();

  void applyLut(LutImageProcessingData const &imageProcessingData);

private:
  void loadPrograms();

  void mainloop();

  Path vertexShaderPath() const;

  void loadTextureAndRender(ImageProcessingData const &imageProcessingData);

#ifdef __APPLE__
  PBDev::ThreadScheduler *mThreadScheduler = nullptr;
#endif

  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  TSQueue<LutImageProcessingData> mWorkQueue;
  std::jthread                    mThread;

  std::stop_token mStopToken;
  std::stop_source mStopSource;

  std::mutex              mWorkMutex;
  std::condition_variable mFinishedWorkCondition;
  bool                    mFinishedWork = false;

  std::unordered_map<std::string, sk_sp<SkRuntimeEffect>> mPrograms;

  const std::unordered_map<std::string, Path> FRAGMENT_SHADERS_PATHS = {
      {"lut", Path("shaders") / "lut.sksl"}};
};
} // namespace PB::Service
