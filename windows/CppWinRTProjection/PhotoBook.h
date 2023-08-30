#pragma once

#include "PhotoBook.g.h"

namespace winrt::CppWinRTProjection::implementation {
struct PhotoBook : PhotoBookT<PhotoBook> {
  PhotoBook() = default;
};
} // namespace winrt::CppWinRTProjection::implementation

namespace winrt::CppWinRTProjection::factory_implementation {
struct PhotoBook : PhotoBookT<PhotoBook, implementation::PhotoBook> {};
} // namespace winrt::CppWinRTProjection::factory_implementation
