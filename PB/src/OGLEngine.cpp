#include <pb/components/OGLEngine.h>

#include <fstream>

#include <pb/components/ThreadScheduler.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/util/Traits.h>

namespace PB::Service {

#ifdef __APPLE__
void OGLEngine::configureThreadScheduler(
    PBDev::ThreadScheduler *threadScheduler)
{
  mThreadScheduler = threadScheduler;
}
#endif
void OGLEngine::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void OGLEngine::start(std::stop_token stopToken)
{
  mStopToken = stopToken;
  mThread = std::jthread([this] { mainloop(); });
}

void OGLEngine::stop(std::stop_source stopSource)
{
  stopSource.request_stop();
  mWorkQueue.enqueue(LutImageProcessingData());
}

void OGLEngine::initOpenGL()
{
}


void OGLEngine::loadPrograms()
{
  // Load shaders.
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    
  }
}

void OGLEngine::mainloop()
{
  initOpenGL();
  loadPrograms();

  while (!mStopToken.stop_requested()) {
    auto imageProcessingData = mWorkQueue.dequeue();
    loadTextureAndRender(imageProcessingData);

    mFinishedWork = true;
    mFinishedWorkCondition.notify_one();
  }
}

Path OGLEngine::vertexShaderPath() const
{
  return mPlatformInfo->installationPath / VERTEX_SHADER_PATH;
}

void OGLEngine::loadTextureAndRender(
    ImageProcessingData const &imageProcessingData)
{
  if (imageProcessingData.type() == ImageProcessingType::LUT) {
    auto lutImageProcessingData =
        dynamic_cast<LutImageProcessingData const &>(imageProcessingData);
    applyLut(lutImageProcessingData);
  }
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
 
}

} // namespace PB::Service
