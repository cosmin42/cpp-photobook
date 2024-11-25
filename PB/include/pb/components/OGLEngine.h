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

struct VulkanImageWrapper final {
  void destroy(VkDevice device)
  {
    vkDestroySampler(device, sampler, nullptr);
    vkDestroyImageView(device, view, nullptr);
    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, memory, nullptr);
  }

  VkImage        image;
  VkDeviceMemory memory;
  VkSampler      sampler;
  VkImageView    view;
};

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

  void beginCommandBuffer();

  void endCommandBuffer();

  void submitCommandBuffer();

  uint32_t findMemoryType(uint32_t              typeFilter,
                          VkMemoryPropertyFlags properties);

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

  VkInstance       mInstance = VK_NULL_HANDLE;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice         mDevice = VK_NULL_HANDLE;
  VkQueue          mQueue = VK_NULL_HANDLE;
  VkCommandPool    mCommandPool = VK_NULL_HANDLE;
  VkCommandBuffer  mCommandBuffer = VK_NULL_HANDLE;
  VkPipeline       mPipeline = VK_NULL_HANDLE;
  VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;

  std::unordered_map<std::string, VkShaderModule>        mShaderPrograms;
  std::unordered_map<std::string, VkDescriptorSetLayout> mDescriptorSetLayout;
  std::unordered_map<std::string, VkPipelineLayout>      mPipelineLayout;
  std::unordered_map<std::string, VkPipeline>            mPipelines;
  std::unordered_map<std::string, VkDescriptorSet>       mDescriptorSets;

  float mImageVertices[20] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                              0.0f,  0.0f, 0.0f, 1.0f, -1.0f, 0.0f,  1.0f,
                              0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f};
  unsigned int mIndices[6] = {0, 1, 2, 0, 2, 3};

  std::mutex              mWorkMutex;
  std::condition_variable mFinishedWorkCondition;
  bool                    mFinishedWork = false;

  const std::unordered_map<std::string, Path> FRAGMENT_SHADERS_PATHS = {
      {"lut", Path("shaders") / "lut.spv"}};

  const Path VERTEX_SHADER_PATH = Path("shaders") / "vertex.spv";

  void buildInstance();
  void buildPhysicalDevice();
  void buildLogicalDevice();
  void buildDescriptorPool();
  void buildCommandPool();
  void buildCommandBuffer();

  void buildDescriptorSetLayout(std::string name);
  void buildPipelineLayout(std::string name);
  void buildPipeline(std::string name);
  void buildDescriptorSet(std::string name);

  VkImage        createImageRaw(std::shared_ptr<cv::Mat> image);
  VkDeviceMemory allocateMemory(VkImage image);
  VkSampler      createSampler();
  VkImageView    createImageView(VkImage image);

  VulkanImageWrapper createImage(std::shared_ptr<cv::Mat> image);
};
} // namespace PB::Service
