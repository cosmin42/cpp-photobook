#pragma once

#include "ImageResources.g.h"

#include <pb/entities/GenericImage.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageResources : ImageResourcesT<ImageResources> {

  explicit ImageResources(PBDev::Path full, PBDev::Path medium,
                          PBDev::Path smallPath)
      : mFull(full), mMedium(medium), mSmall(smallPath)
  {
  }
  ~ImageResources() = default;

  winrt::hstring FullPath();
  winrt::hstring MediumPath();
  winrt::hstring SmallPath();

private:
  PBDev::Path mFull;
  PBDev::Path mMedium;
  PBDev::Path mSmall;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation