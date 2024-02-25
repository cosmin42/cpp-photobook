#pragma once

#include "PhotobookWin.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct PhotobookWin : PhotobookWinT<PhotobookWin> {
  PhotobookWin() {}

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
