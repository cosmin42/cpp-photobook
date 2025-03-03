#include <pb/components/VulkanManager.h>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/GrRecordingContext.h>
#include <include/gpu/ganesh/vk/GrVkDirectContext.h>
#include <include/gpu/vk/VulkanBackendContext.h>
#pragma warning(pop)

#include <vulkan/vulkan_core.h>

#include <pb/Config.h>

namespace PB {
VulkanManager::~VulkanManager()
{
  if (mGpuContext != nullptr) {
    delete mGpuContext;
  }
}

bool VulkanManager::initInstance()
{
  VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.pApplicationName = "PBNoir";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "PBNoir";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;
  instanceCreateInfo.enabledExtensionCount =
      sizeof(INSTANCE_EXTENSIONS) / sizeof(INSTANCE_EXTENSIONS[0]);
  instanceCreateInfo.ppEnabledExtensionNames = INSTANCE_EXTENSIONS;

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);

  if (result != VK_SUCCESS) {
    return false;
  }

  return true;
}

bool VulkanManager::initPhysicalDevice()
{
  // Enumerate physical devices
  uint32_t physicalDeviceCount = 0;
  VkResult result =
      vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

  if (result != VK_SUCCESS) {
    return false;
  }

  mPhysicalDevices = std::vector<VkPhysicalDevice>(physicalDeviceCount);
  result = vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount,
                                      mPhysicalDevices.data());

  if (result != VK_SUCCESS) {
    return false;
  }

  return !mPhysicalDevices.empty();
}

bool VulkanManager::selectPhysicalDevice()
{
  uint32_t deviceExtensionCount = 0;
  vkEnumerateDeviceExtensionProperties(mPhysicalDevices[0], nullptr,
                                       &deviceExtensionCount, nullptr);

  std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
  vkEnumerateDeviceExtensionProperties(mPhysicalDevices[0], nullptr,
                                       &deviceExtensionCount,
                                       deviceExtensions.data());

  mSelectedPhysicalDevice = mPhysicalDevices[0];

  return true;
}

bool VulkanManager::initDevice()
{
  VkDeviceQueueCreateInfo queueCreateInfo = {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  queueCreateInfo.queueFamilyIndex = 0;
  queueCreateInfo.queueCount = 1;
  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

  VkResult result = vkCreateDevice(mSelectedPhysicalDevice, &deviceCreateInfo,
                                   nullptr, &mDevice);

  if (result != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool VulkanManager::initQueue()
{
  // Get the queue
  vkGetDeviceQueue(mDevice, 0, 0, &mQueue);

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mSelectedPhysicalDevice,
                                           &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      mSelectedPhysicalDevice, &queueFamilyCount, queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      mQueueIndex = i; // Found a graphics queue
      break;           // Stop searching if we only need graphics support
    }
  }

  if (mQueueIndex == -1) {
    return false;
  }

  return true;
}

bool VulkanManager::initVulkanContext()
{
  skgpu::VulkanBackendContext backendContext;

  // Create a Vulkan context
  backendContext.fInstance = mInstance;
  backendContext.fPhysicalDevice = mSelectedPhysicalDevice;
  backendContext.fDevice = mDevice;
  backendContext.fQueue = mQueue;
  backendContext.fGraphicsQueueIndex = mQueueIndex;
  backendContext.fMaxAPIVersion = VK_MAKE_VERSION(1, 0, 0);
  backendContext.fVkExtensions = nullptr;
  backendContext.fDeviceFeatures = nullptr;
  backendContext.fDeviceFeatures2 = nullptr;
  backendContext.fMemoryAllocator = nullptr;
  backendContext.fGetProc = [](const char *name, VkInstance inst,
                               VkDevice dev) -> PFN_vkVoidFunction {
    if (dev) {
      return vkGetDeviceProcAddr(dev, name);
    }
    return vkGetInstanceProcAddr(inst, name);
  };
  ;
  backendContext.fProtectedContext = skgpu::Protected::kNo;
  backendContext.fDeviceLostContext = nullptr;
  backendContext.fDeviceLostProc = nullptr;

  sk_sp<GrDirectContext> directContext =
      GrDirectContexts::MakeVulkan(backendContext);

  mGpuContext = static_cast<GrRecordingContext *>(directContext.release());

  return mGpuContext != nullptr;
}

void VulkanManager::printValidationLayersInfo()
{

  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  spdlog::info("Available Vulkan validation layers:");
  for (const auto &layer : availableLayers) {
    spdlog::info("{}", layer.layerName);
  }
}

void VulkanManager::printAvailableExtensions()
{
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  spdlog::info("Available Vulkan extensions:");
  for (const auto &extension : extensions) {
    spdlog::info("{}", extension.extensionName);
  }
}

void VulkanManager::init()
{
  if (OneConfig::SHOW_VULKAN_DEBUG_INFO) {
    Noir::inst().getLogger()->info("Initializing Vulkan Manager");
    printValidationLayersInfo();
    printAvailableExtensions();
  }
  if (!initInstance()) {
    Noir::inst().getLogger()->error("Failed to initialize Vulkan instance");
    return;
  }
  if (!initPhysicalDevice()) {
    Noir::inst().getLogger()->error(
        "Failed to initialize Vulkan physical device");
    return;
  }
  if (!selectPhysicalDevice()) {
    Noir::inst().getLogger()->error("Failed to select Vulkan physical device");
    return;
  }
  if (!initDevice()) {
    Noir::inst().getLogger()->error("Failed to initialize Vulkan device");
    return;
  }
  if (!initQueue()) {
    Noir::inst().getLogger()->error("Failed to initialize Vulkan queue");
    return;
  }
  if (!initVulkanContext()) {
    Noir::inst().getLogger()->error("Failed to initialize Vulkan context");
    return;
  }

  Noir::inst().getLogger()->info("Vulkan Manager initialized");
}

} // namespace PB
