#pragma once

#include "ImageViews.g.h"

#include "ImageMonitor.h"
#include "StagedImages.h"

#include <pb/ProjectManagementSystem.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageViews : ImageViewsT<ImageViews> {
  ImageViews(
      std::shared_ptr<PB::ProjectManagementSystem> projectManagementSystem)
      : mProjectManagementSystem(projectManagementSystem)
  {
  }
  ~ImageViews() = default;

  PhotobookRuntimeComponent::ImageMonitor ImageMonitor()
  {
    auto maybeLoadedProject =
        mProjectManagementSystem->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProject != nullptr);

    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::ImageMonitor>(
        maybeLoadedProject->second.imageMonitor());
  }

  PhotobookRuntimeComponent::StagedImages StagedImages()
  {
    auto maybeLoadedProject =
        mProjectManagementSystem->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProject != nullptr);

    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::StagedImages>(
        maybeLoadedProject->second.stagedImages());
    return nullptr;
  }

private:
  std::shared_ptr<PB::ProjectManagementSystem> mProjectManagementSystem =
      nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
