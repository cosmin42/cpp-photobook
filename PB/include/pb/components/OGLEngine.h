#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <vulkan/vulkan.h>

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

  void start(std::stop_token stopToken);
  void stop(std::stop_source stopSource);

  void applyLut(LutImageProcessingData const &imageProcessingData);

private:
  void initOpenGL();

  void createCommandPool();

  void createCommandBuffer();

  void beginCommandBuffer();

  void endCommandBuffer();

  void submitCommandBuffer();

  void transitionImageLayout(VkImage image, VkImageLayout oldLayout,
                             VkImageLayout newLayout);

  VkShaderModule createShaderModule(Path shaderSpv) const;

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

  VkInstance       mInstance;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice         mDevice;
  VkQueue          mQueue;
  VkCommandPool    mCommandPool;
  VkCommandBuffer  mCommandBuffer;

  std::unordered_map<std::string, VkShaderModule> mShaderPrograms;

  float mImageVertices[20] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                              0.0f,  0.0f, 0.0f, 1.0f, -1.0f, 0.0f,  1.0f,
                              0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f};
  unsigned int mIndices[6] = {0, 1, 2, 0, 2, 3};

  std::mutex              mWorkMutex;
  std::condition_variable mFinishedWorkCondition;
  bool                    mFinishedWork = false;

  const std::unordered_map<std::string, Path> FRAGMENT_SHADERS_PATHS = {
      {"lut", Path("shaders") / "lut.glsl"}};

  const Path VERTEX_SHADER_PATH = Path("shaders") / "vertex.glsl";
};
} // namespace PB::Service
