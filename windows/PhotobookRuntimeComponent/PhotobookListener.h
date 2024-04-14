#pragma once
#include <pb/PhotobookListener.h>

#include "PBError.h"
#include "VirtualImagePtr.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

class PhotobookListener : PB::PhotobookListener {
public:
  explicit PhotobookListener(
      PhotobookRuntimeComponent::PhotobookListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  void onProjectRead() override { mManagedListener.OnProjectRead(); }
  void onProjectRenamed() override { mManagedListener.OnProjectRead(); }
  void onMetadataUpdated() override { mManagedListener.OnMetadataUpdated(); }
  void onPersistenceError(PBDev::Error error) override
  {
    mManagedListener.OnPersistenceError(winrt::make<PBError>(error));
  }

  void onExportFinished() override { mManagedListener.OnExportFinished(); }
  void onError(PBDev::Error error) override
  {
    mManagedListener.OnError(winrt::make<PBError>(error));
  }
  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                          int index) override
  {
    Windows::Foundation::Collections::IVector<
        PhotobookRuntimeComponent::VirtualImagePtr>
        managedPhotos = winrt::single_threaded_vector<
            PhotobookRuntimeComponent::VirtualImagePtr>();

    for (auto photo : photos) {
      managedPhotos.Append(winrt::make<VirtualImagePtr>(photo));
    }

    mManagedListener.OnStagedImageAdded(managedPhotos, index);
  }
  void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override
  {
    Windows::Foundation::Collections::IVector<uint32_t> managedPhotos =
        winrt::single_threaded_vector<uint32_t>();

    for (auto index : removedIndexes) {
      managedPhotos.Append(index);
    }

    mManagedListener.OnStagedImageRemoved(managedPhotos);
  }

  void onMappingFinished(Path path) override
  {
	mManagedListener.OnMappingFinished(winrt::to_hstring(path.string()));
  }

private:
  // TODO: Check if not & is enough;
  PhotobookRuntimeComponent::PhotobookListener mManagedListener;
};

} // namespace winrt::PhotobookRuntimeComponent::implementation