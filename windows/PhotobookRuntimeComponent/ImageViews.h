#pragma once

#include "ImageViews.g.h"

#include "ImageMonitor.h"
#include "StagedImages.h"

#include <pb/services/ProjectManagementService.h>

using namespace PB::Service;

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageViews : ImageViewsT<ImageViews> {
  ImageViews(std::shared_ptr<ProjectManagementService> projectManagementService)
      : mProjectManagementService(projectManagementService)
  {
  }
  ~ImageViews() = default;

  PhotobookRuntimeComponent::ImageMonitor ImageMonitor()
  {
    auto maybeLoadedProject =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProject != nullptr);

    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::ImageMonitor>(
        maybeLoadedProject->second.imageMonitor());
  }

  PhotobookRuntimeComponent::StagedImages StagedImages()
  {
    auto maybeLoadedProject =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProject != nullptr);

    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::StagedImages>(
        maybeLoadedProject->second.stagedImages());
  }

private:
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
