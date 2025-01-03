#pragma once

#include "VICollectionStatefulIterator.g.h"

#include "VICollectionStatefulIterator.h"

#include <pb/entities/GenericImage.h>
#include <pb/util/IteratorWithState.h>

#include "VirtualImagePtr.g.h"
#include "VirtualImagePtr.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct VICollectionStatefulIterator
    : VICollectionStatefulIteratorT<VICollectionStatefulIterator> {
  VICollectionStatefulIterator(
      PBDev::IteratorWithState<std::vector<PB::GenericImagePtr>> iterator)
      : mIterator(iterator)
  {
  }

  ~VICollectionStatefulIterator() = default;

  PhotobookRuntimeComponent::VirtualImagePtr current()
  {
    return winrt::make<VirtualImagePtr>(mIterator.current());
  }

  PhotobookRuntimeComponent::VICollectionStatefulIterator At(unsigned index)
  {
    return winrt::make<VICollectionStatefulIterator>(mIterator[index]);
  }

  unsigned Size() { return mIterator.size(); }

  bool Valid() { return mIterator.valid(); }

private:
  PBDev::IteratorWithState<std::vector<PB::GenericImagePtr>> mIterator;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation