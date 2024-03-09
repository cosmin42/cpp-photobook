#pragma once

#include "StagedImages.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct StagedImages : StagedImagesT<StagedImages> {
  StagedImages() {}
  ~StagedImages() = default;

  void addPictures(
      Windows::Foundation::Collections::IVector<VirtualImagePtr> pictures)
  {
  }

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
