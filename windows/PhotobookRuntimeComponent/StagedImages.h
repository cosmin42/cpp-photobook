#pragma once

#include "StagedImages.g.h"
#include "VirtualImagePtr.g.h"
#include "VirtualImagePtr.h"

#include <pb/StagedImages.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct StagedImages : StagedImagesT<StagedImages> {
  StagedImages(PB::StagedImages &stagedImages) : mStagedImages(stagedImages) {}
  ~StagedImages() = default;

  void AddPictures(Windows::Foundation::Collections::IVector<
                   PhotobookRuntimeComponent::VirtualImagePtr>
                       pictures)
  {
    std::vector<std::shared_ptr<PB::VirtualImage>> nativePictures;
    for (auto &&picture : pictures) {
      auto nativeImagePtr =
          winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                              VirtualImagePtr>(picture)
              ->unwrap();
      nativePictures.push_back(nativeImagePtr);
    }
    mStagedImages.addPictures(nativePictures);
  }

  void AddPictures(Windows::Foundation::Collections::IVector<
                       PhotobookRuntimeComponent::VirtualImagePtr>
                       pictures,
                   int position)
  {
    std::vector<std::shared_ptr<PB::VirtualImage>> nativePictures;
    for (auto &&picture : pictures) {
      auto nativeImagePtr =
          winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                              VirtualImagePtr>(picture)
              ->unwrap();
      nativePictures.push_back(nativeImagePtr);
    }
    mStagedImages.addPictures(nativePictures, position);
  }

  void AddPicture(PhotobookRuntimeComponent::VirtualImagePtr picture)
  {
    auto nativeImagePtr =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
            picture)
            ->unwrap();
    mStagedImages.addPicture(nativeImagePtr);
  }

  void
  RemovePicture(Windows::Foundation::Collections::IVector<unsigned> indexes)
  {
    std::vector<unsigned> nativeIndexes;
    for (auto &&index : indexes) {
      nativeIndexes.push_back(index);
    }
    mStagedImages.removePicture(nativeIndexes);
  }

  void Clear() { mStagedImages.clear(); }

  void StashImages(Windows::Foundation::Collections::IVector<unsigned> indexes)
  {
    std::vector<unsigned> nativeIndexes;

    for (int i = 0; i < (int)indexes.Size(); ++i) {
      nativeIndexes.push_back(indexes.GetAt(i));
    }

    mStagedImages.stashImages(nativeIndexes);
  }
  void PopImages(int position) { mStagedImages.popImages(position); }

  Windows::Foundation::Collections::IVector<
      PhotobookRuntimeComponent::VirtualImagePtr>
  StagedPhotos()
  {
    auto stagedPhotos = winrt::single_threaded_vector<
        PhotobookRuntimeComponent::VirtualImagePtr>();
    for (auto &&photo : mStagedImages.stagedPhotos()) {
      stagedPhotos.Append(
          winrt::make<
              PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
              photo));
    }
    return stagedPhotos;
  }

  PhotobookRuntimeComponent::VirtualImagePtr Picture(int index)
  {
    return winrt::make<
        PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
        mStagedImages.picture(index));
  }

private:
  PB::StagedImages &mStagedImages;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation