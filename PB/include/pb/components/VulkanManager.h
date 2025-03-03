#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)
#pragma warning(disable : 4201)
#include <include/core/SkSurface.h>
#include <include/effects/SkRuntimeEffect.h>
#ifdef WIN32
#include <include/gpu/GrRecordingContext.h>
#include <include/gpu/vk/GrVkTypes.h>
#elif __MACH__
#include <include/gpu/ganesh/GrRecordingContext.h>
#include <include/gpu/ganesh/vk/GrVkTypes.h>
#endif

#pragma warning(pop)
#pragma warning(push)

#ifdef _WIN32
#include <Windows.h>
#include <include/third_party/vulkan/vulkan/vulkan_win32.h>
#elif __ANDROID__
#include <include/third_party/vulkan/vulkan/vulkan_android.h>
#elif __MACH__
#include <include/third_party/vulkan/vulkan/vulkan_macos.h>
#elif __IOS__
#include <include/third_party/vulkan/vulkan/vulkan_ios.h>
#else
#error "Unsupported platform"
#endif

namespace PB {
class VulkanManager {
public:
  VulkanManager() = default;
  ~VulkanManager();

  void init();

  GrRecordingContext *gpuContext() const { return mGpuContext; }

  sk_sp<SkSurface> getSurface(SkImageInfo imageInfo);

private:
  const char *INSTANCE_EXTENSIONS[2] = {
      VK_KHR_SURFACE_EXTENSION_NAME, // Required for window rendering
#ifdef _WIN32
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME // Windows-specific
#elif __ANDROID__
      VK_KHR_ANDROID_SURFACE_EXTENSION_NAME // Android-specific
#elif __MACH__
      VK_MVK_MACOS_SURFACE_EXTENSION_NAME // macOS-specific (MoltenVK)
#elif __IOS__
      VK_MVK_IOS_SURFACE_EXTENSION_NAME // iOS-specific (MoltenVK)
#endif
  };

  VkInstance                    mInstance;
  std::vector<VkPhysicalDevice> mPhysicalDevices;
  VkPhysicalDevice              mSelectedPhysicalDevice;
  VkDevice                      mDevice;
  VkQueue                       mQueue;
  int                           mQueueIndex = -1;
  GrRecordingContext           *mGpuContext = nullptr;

  bool initInstance();
  bool initPhysicalDevice();
  bool selectPhysicalDevice();
  bool initDevice();
  bool initQueue();
  bool initVulkanContext();

  void printValidationLayersInfo();
  void printAvailableExtensions();
};
} // namespace PB
