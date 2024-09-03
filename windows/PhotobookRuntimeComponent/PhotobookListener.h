#pragma once
#include <pb/PhotobookListener.h>

#include "LutIconInfo.g.h"
#include "LutIconInfo.h"

#include "PBError.h"
#include "VirtualImagePtr.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

// TODO: Is this class even used anymore?
class PhotobookListener : PB::PhotobookListener {
public:
  explicit PhotobookListener(
      PhotobookRuntimeComponent::PhotobookListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  void onLutAdded(PB::LutIconInfo iconInfo) override
  {
    mManagedListener.OnLutAdded(winrt::make<LutIconInfo>(iconInfo));
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

  void onCollageThumbnailsCreated() override
  {
    mManagedListener.OnCollageThumbnailsCreated();
  }

  void onCollageCreated(unsigned                          index,
                        std::shared_ptr<PB::VirtualImage> newImage) override
  {
    mManagedListener.OnCollageCreated(index,
                                      winrt::make<VirtualImagePtr>(newImage));
  }

  void onImageMapped(PBDev::ImageToPaperId             id,
                     std::shared_ptr<PB::VirtualImage> image) override
  {
    auto nativeUuid = id.raw();

    uint64_t data1 = nativeUuid.data[0] << 24 | nativeUuid.data[1] << 16 |
                     nativeUuid.data[2] << 8 | nativeUuid.data[3];

    uint16_t data2 = nativeUuid.data[4] << 8 | nativeUuid.data[5];
    uint16_t data3 = nativeUuid.data[6] << 8 | nativeUuid.data[7];

    GUID existingGuid = {(unsigned long)data1,
                         data2,
                         data3,
                         {nativeUuid.data[8], nativeUuid.data[9],
                          nativeUuid.data[10], nativeUuid.data[11],
                          nativeUuid.data[12], nativeUuid.data[13],
                          nativeUuid.data[14], nativeUuid.data[15]}};

    winrt::guid managedGuid(existingGuid);

    mManagedListener.OnImageMapped(managedGuid,
                                   winrt::make<VirtualImagePtr>(image));
  }

private:
  // TODO: Check if not & is enough;
  PhotobookRuntimeComponent::PhotobookListener mManagedListener;
};

} // namespace winrt::PhotobookRuntimeComponent::implementation