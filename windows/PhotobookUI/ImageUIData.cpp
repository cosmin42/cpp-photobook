// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageUIData.h"
#if __has_include("ImageUIData.g.cpp")
#include "ImageUIData.g.cpp"
#endif
// clang-format on

#include <pb/Config.h>

namespace winrt::PhotobookUI::implementation {

ImageUIData::ImageUIData()
    : mFullPath(winrt::to_hstring(PB::Context::LOADING_PHOTO_PLACEHOLDER)),
      mMediumPath(winrt::to_hstring(PB::Context::LOADING_PHOTO_PLACEHOLDER)),
      mSmallPath(winrt::to_hstring(PB::Context::PHOTO_TIMELINE_DEFAULT_IMAGE))
{
}

ImageUIData::ImageUIData(winrt::hstring fullPath, winrt::hstring mediumPath,
                         winrt::hstring smallPath)
    : mFullPath(fullPath), mMediumPath(mediumPath), mSmallPath(smallPath)
{
}

winrt::hstring ImageUIData::FullPath() { return mFullPath; }
winrt::hstring ImageUIData::MediumPath() { return mMediumPath; }
winrt::hstring ImageUIData::SmallPath() { return mSmallPath; }

} // namespace winrt::PhotobookUI::implementation