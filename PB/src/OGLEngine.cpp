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
  initOpenGL();
  mThread = std::jthread([this] { mainloop(); });
}

void OGLEngine::stop(std::stop_source stopSource)
{
  stopSource.request_stop();
  mWorkQueue.enqueue(LutImageProcessingData());
}

bool checkValidationLayerSupport()
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  const char *validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

  for (const char *layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}


void OGLEngine::initOpenGL()
{
  if (!checkValidationLayerSupport()) {
    PBDev::basicAssert(false);
  }

  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "PhotoboookNoir";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "PhotoboookNoirEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  const char *extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  createInfo.enabledExtensionCount = 1;
  createInfo.ppEnabledExtensionNames = extensions;


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

  createCommandPool();
  createCommandBuffer();
  loadPrograms();

  while (!mStopToken.stop_requested()) {
    auto imageProcessingData = mWorkQueue.dequeue();
    loadTextureAndRender(imageProcessingData);

    mFinishedWork = true;
    mFinishedWorkCondition.notify_one();
  }

  vkDestroyInstance(mInstance, nullptr);
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

uint32_t OGLEngine::findMemoryType(uint32_t              typeFilter,
                                   VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memoryProperties);

  // Iterate over all memory types
  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
    // Check if the memory type is compatible with the requested properties
    if ((typeFilter & (1 << i)) &&
        (memoryProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i; // Return the index of the first suitable memory type
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

std::pair<VkImage, VkSampler>
OGLEngine::createVkImage(std::shared_ptr<cv::Mat> image)
{
  VkImageCreateInfo imageCreateInfo = {};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA format
  imageCreateInfo.extent = {(uint32_t)image->cols, (uint32_t)image->rows,
                            1}; // Width and height of the image
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
  imageCreateInfo.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT |
      VK_IMAGE_USAGE_SAMPLED_BIT; // We will sample from this image
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VkImage vkImage;
  if (vkCreateImage(mDevice, &imageCreateInfo, nullptr, &vkImage) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(mDevice, vkImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDeviceMemory imageMemory;
  if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &imageMemory) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  vkBindImageMemory(mDevice, vkImage, imageMemory, 0);

  VkSamplerCreateInfo samplerCreateInfo = {};
  samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
  samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
  samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.anisotropyEnable = VK_FALSE;
  samplerCreateInfo.maxAnisotropy = 1.0f;
  samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
  samplerCreateInfo.compareEnable = VK_FALSE;
  samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerCreateInfo.mipLodBias = 0.0f;
  samplerCreateInfo.minLod = 0.0f;
  samplerCreateInfo.maxLod = 1.0f;

  VkSampler textureSampler;
  if (vkCreateSampler(mDevice, &samplerCreateInfo, nullptr, &textureSampler) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }

  return {vkImage, textureSampler};
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
  // Load the image into VKImage
  auto [inputImage, inputSampler] = createVkImage(imageProcessingData.inImage);
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
