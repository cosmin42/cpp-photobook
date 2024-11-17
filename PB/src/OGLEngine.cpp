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
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  // Additional configuration of createInfo if needed

  if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    PBDev::basicAssert(false);
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

  // Choose a suitable physical device (e.g., first available)
  mPhysicalDevice = devices[0];

  float queuePriority = 1.0f;

  // Define the queue creation info
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = 0; // Replace with the correct family index
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  // Specify device features
  VkPhysicalDeviceFeatures deviceFeatures = {};

  // Create the logical device
  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

  if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  vkGetDeviceQueue(mDevice, 0, 0, &mQueue);
}

void OGLEngine::createCommandPool()
{
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = 0;
  poolInfo.flags = 0;

  if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::createCommandBuffer()
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = mCommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(mDevice, &allocInfo, &mCommandBuffer) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::beginCommandBuffer()
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::endCommandBuffer()
{
  if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer!");
  }
}

void OGLEngine::submitCommandBuffer()
{
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffer;

  if (vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
  vkQueueWaitIdle(mQueue);
}

VkShaderModule OGLEngine::createShaderModule(Path shaderSpv) const
{
  std::ifstream file(shaderSpv, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    PBDev::basicAssert(false);
  }

  size_t            fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);

  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = buffer.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  return shaderModule;
}

void OGLEngine::transitionImageLayout(VkImage image, VkImageLayout oldLayout,
                                      VkImageLayout newLayout)
{

}

void OGLEngine::loadPrograms()
{
  // Load shaders.
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    mShaderPrograms[name] = createShaderModule(path);
  }
}

void OGLEngine::mainloop()
{
  initOpenGL();
  createCommandPool();
  createCommandBuffer();
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
  // Load the image into VKImage
  VkImage inputImage = VK_NULL_HANDLE;
  VkImage outputImage = VK_NULL_HANDLE;

  beginCommandBuffer();

  // Transition input and output images to appropriate layouts
  transitionImageLayout(inputImage, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_GENERAL);
  transitionImageLayout(outputImage, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_GENERAL);

  // Record commands to apply the LUT shader (this step would involve
  // dispatching a compute shader or rendering) Placeholder for shader execution
  // logic

  endCommandBuffer();
  submitCommandBuffer();
}

} // namespace PB::Service
