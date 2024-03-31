#pragma once

#include <pb/StagedImages.h>

#include "PhotobookListener.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

class StagedImagesListener : PB::StagedImagesListener {
public:
  explicit StagedImagesListener(
      PhotobookRuntimeComponent::StagedImagesListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  virtual ~StagedImagesListener() = default;

  virtual void onPicturesAdded(int index, int size) override
  {
    mManagedListener.OnPictureAdded(index, size);
  }

  virtual void onPictureRemoved(std::vector<unsigned> indices) override
  {
    Windows::Foundation::Collections::IVector<int32_t> managedPhotos =
        winrt::single_threaded_vector<int32_t>();
    for (auto i : indices) {
      managedPhotos.Append(i);
    }

    mManagedListener.OnPictureRemoved(managedPhotos);
  }

private:
  PhotobookRuntimeComponent::StagedImagesListener mManagedListener;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
