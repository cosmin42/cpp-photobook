// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageResources.h"
#include "ImageResources.g.cpp"
// clang-format on

namespace winrt::PhotobookRuntimeComponent::implementation {

winrt::hstring ImageResources::FullPath()
{
  return winrt::to_hstring(mImageResource.full.string());
}
winrt::hstring ImageResources::MediumPath()
{
  return winrt::to_hstring(mImageResource.medium.string());
}
winrt::hstring ImageResources::SmallPath()
{
  return winrt::to_hstring(mImageResource.small.string());
}

} // namespace winrt::PhotobookRuntimeComponent::implementation