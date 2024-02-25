#pragma once

#include "PhotobookWin.g.h"

#include <pb/PhotoBook.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct PhotobookWin : PhotobookWinT<PhotobookWin> {
  PhotobookWin() {}

  void recallMetadata();
  void recallProject(winrt::hstring const name);

  void newProject(winrt::hstring const name);

private:
  std::shared_ptr<PB::Photobook> mPhotobook = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
