// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageUIData.h"
#include "ImageUIData.g.cpp"
// clang-format on

#include <PlatformHelper.h>

#include <pb/Config.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

ImageUIData::ImageUIData()
    : mFullPath(
          winrt::to_hstring((PlatformHelper::CurrentAppInstallationFolder() /
                             Path(PB::Context::LOADING_PHOTO_PLACEHOLDER))
                                .string())),
      mMediumPath(
          winrt::to_hstring((PlatformHelper::CurrentAppInstallationFolder() /
                             Path(PB::Context::LOADING_PHOTO_PLACEHOLDER))
                                .string())),
      mSmallPath(
          winrt::to_hstring((PlatformHelper::CurrentAppInstallationFolder() /
                             Path(PB::Context::PHOTO_TIMELINE_DEFAULT_IMAGE))
                                .string())),
      mProcessed(false)
{
}

ImageUIData::ImageUIData(winrt::hstring key, winrt::hstring full,
                         winrt::hstring medium, winrt::hstring smallPath, bool processed)
    : mKey(key), mFullPath(full), mMediumPath(medium), mSmallPath(smallPath),
      mProcessed(processed)
{
}

winrt::hstring ImageUIData::KeyPath() { return mKey; }
winrt::hstring ImageUIData::FullPath() { return mFullPath; }
winrt::hstring ImageUIData::MediumPath() { return mMediumPath; }
winrt::hstring ImageUIData::SmallPath() { return mSmallPath; }

bool ImageUIData::Processed() { return mProcessed; }

} // namespace winrt::PhotobookRuntimeComponent::implementation