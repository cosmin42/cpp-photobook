// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageResources.h"
#include "ImageResources.g.cpp"
// clang-format on

namespace winrt::PhotobookRuntimeComponent::implementation {

winrt::hstring ImageResources::FullPath()
{
  return winrt::to_hstring(mFull.string());
}
winrt::hstring ImageResources::MediumPath()
{
  return winrt::to_hstring(mMedium.string());
}
winrt::hstring ImageResources::SmallPath()
{
  return winrt::to_hstring(mSmall.string());
}

} // namespace winrt::PhotobookRuntimeComponent::implementation