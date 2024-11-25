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

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
  for (const auto &layer : layers) {
    spdlog::info("Layer name: {}", layer.layerName);
  }

  return true;
}

void OGLEngine::buildInstance()
{
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_NV_optimus"};

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

  // Enable validation layers
  createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  createInfo.ppEnabledLayerNames = validationLayers.data();

  if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::buildPhysicalDevice()
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    PBDev::basicAssert(false);
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

  // Choose a suitable physical device (e.g., first available)
  mPhysicalDevice = devices[0];

  for (auto &device : devices) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    std::string deviceName = deviceProperties.deviceName;
    spdlog::info("Device name: {}", deviceName);
  }
}

void OGLEngine::buildLogicalDevice()
{
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

void OGLEngine::buildDescriptorPool()
{
  // Define the pool sizes for each descriptor type
  VkDescriptorPoolSize poolSizes[2]{};

  // Combined image sampler descriptors
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[0].descriptorCount = 10;

  // Uniform buffer descriptors
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[1].descriptorCount = 5;

  // Create the descriptor pool
  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2; // Number of descriptor types
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.maxSets = 15; // Maximum number of descriptor sets

  if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::buildCommandPool()
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

void OGLEngine::buildCommandBuffer()
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

void OGLEngine::initOpenGL()
{
  if (!checkValidationLayerSupport()) {
    PBDev::basicAssert(false);
  }

  buildInstance();
  buildPhysicalDevice();
  buildLogicalDevice();
  buildDescriptorPool();
}

void OGLEngine::buildDescriptorSetLayout(std::string name)
{
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = 0;
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &layoutBinding;

  mDescriptorSetLayout[name] = VK_NULL_HANDLE;

  if (vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr,
                                  &mDescriptorSetLayout[name]) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::buildPipelineLayout(std::string name)
{
  // Create pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout[name];

  if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr,
                             &mPipelineLayout[name]) != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::buildPipeline(std::string name)
{
  VkComputePipelineCreateInfo pipelineCreateInfo = {};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.stage.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  pipelineCreateInfo.stage.module = mShaderPrograms[name];
  pipelineCreateInfo.stage.pName = "main"; // Entry point function name

  pipelineCreateInfo.layout = mPipelineLayout[name];

  VkPipeline computePipeline = VK_NULL_HANDLE;
  auto       result =
      vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                               nullptr, &computePipeline);
  if (result != VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::buildDescriptorSet(std::string name)
{
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = mDescriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &mDescriptorSetLayout[name];

  VkDescriptorSet descriptorSet;
  if (vkAllocateDescriptorSets(mDevice, &allocInfo, &descriptorSet) !=
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
    PBDev::basicAssert(false);
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
  if (!std::filesystem::exists(shaderSpv)) {
    PBDev::basicAssert(false);
  }
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
  // Define the image memory barrier
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;

  // Define the subresource range of the image
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  // Specify the source and destination pipeline stages based on the layouts
  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
           newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else {
    PBDev::basicAssert(false);
  }

  // Apply the barrier
  vkCmdPipelineBarrier(mCommandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
}

void OGLEngine::loadPrograms()
{
  // Load shaders.
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    mShaderPrograms[name] =
        createShaderModule(mPlatformInfo->installationPath / path);
    buildDescriptorSetLayout(name);
    buildPipelineLayout(name);
    buildPipeline(name);
    buildDescriptorSet(name);
  }
}

void OGLEngine::mainloop()
{
  buildCommandPool();
  buildCommandBuffer();
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

  PBDev::basicAssert(false);
}

VkImage OGLEngine::createImageRaw(std::shared_ptr<cv::Mat> image)
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
  return vkImage;
}

VkDeviceMemory OGLEngine::allocateMemory(VkImage image)
{
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(mDevice, image, &memRequirements);

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
  return imageMemory;
}

VkSampler OGLEngine::createSampler()
{
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
  return textureSampler;
}

VkImageView OGLEngine::createImageView(VkImage image)
{
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;                    // The image to create a view for
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // 2D texture view
  viewInfo.format =
      VK_FORMAT_R8G8B8A8_UNORM; // Match or reinterpret the image format
  viewInfo.subresourceRange.aspectMask =
      VK_IMAGE_ASPECT_COLOR_BIT;                // Access the color aspect
  viewInfo.subresourceRange.baseMipLevel = 0;   // Start at the first mip level
  viewInfo.subresourceRange.levelCount = 1;     // Access one mip level
  viewInfo.subresourceRange.baseArrayLayer = 0; // Start at the first layer
  viewInfo.subresourceRange.layerCount = 1;     // Access one layer

  VkImageView imageView;
  if (vkCreateImageView(mDevice, &viewInfo, nullptr, &imageView) !=
      VK_SUCCESS) {
    PBDev::basicAssert(false);
  }
  return imageView;
}

VulkanImageWrapper OGLEngine::createImage(std::shared_ptr<cv::Mat> image)
{
  auto vkImage = createImageRaw(image);
  auto imageMemory = allocateMemory(vkImage);
  vkBindImageMemory(mDevice, vkImage, imageMemory, 0);
  auto sampelr = createSampler();
  auto imageView = createImageView(vkImage);
  return {vkImage, imageMemory, sampelr, imageView};
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
  auto [inputImage, inputMemory, inputSampler, imageView] =
      createImage(imageProcessingData.inImage);

  VkImage outputImage = VK_NULL_HANDLE;

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageInfo.imageView = imageView;
  imageInfo.sampler = inputSampler;

  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = mDescriptorSets.at("lut");
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pImageInfo = &imageInfo;

  vkUpdateDescriptorSets(mDevice, 1, &descriptorWrite, 0, nullptr);

  beginCommandBuffer();

  // Transition input and output images to appropriate layouts
  transitionImageLayout(inputImage, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_GENERAL);

  transitionImageLayout(outputImage, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_GENERAL);

  // Record commands to apply the LUT shader (this step would involve
  // dispatching a compute shader or rendering) Placeholder for shader
  // execution logic

  endCommandBuffer();
  submitCommandBuffer();
}

} // namespace PB::Service
