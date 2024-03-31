#pragma once

#include <pb/ImageMonitor.h>

#include <winrt/PhotobookRuntimeComponent.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

class ImageMonitorListener : public PB::ImageMonitorListener {
public:
  explicit ImageMonitorListener(
      PhotobookRuntimeComponent::ImageMonitorListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  virtual ~ImageMonitorListener() = default;

  void onImportFolderAdded() override
  {
    mManagedListener.OnImportFolderAdded();
  }
  void onImportFolderRemoved(unsigned index) override
  {
    mManagedListener.OnImportFolderRemoved(index);
  }
  void onRefresh() override { mManagedListener.OnRefresh(); }
  void onCleared() override { mManagedListener.OnCleared(); }

private:
  PhotobookRuntimeComponent::ImageMonitorListener mManagedListener;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
