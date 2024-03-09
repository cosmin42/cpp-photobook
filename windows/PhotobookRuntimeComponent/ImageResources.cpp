// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageResources.h"
#include "ImageResources.g.cpp"
// clang-format on

namespace winrt::PhotobookRuntimeComponent::implementation {

winrt::hstring ImageResources::fullPath()
{
  return winrt::to_hstring(mImageResource.full.string());
}
winrt::hstring ImageResources::mediumPath()
{
  return winrt::to_hstring(mImageResource.medium.string());
}
winrt::hstring ImageResources::smallPath()
{
  return winrt::to_hstring(mImageResource.small.string());
}

void ImageResources::setFullPath(winrt::hstring const fullPath)
{
  mImageResource.full = winrt::to_string(fullPath);
}

void ImageResources::setMediumPath(winrt::hstring const mediumPath)
{
  mImageResource.medium = winrt::to_string(mediumPath);
}

void ImageResources::setSmallPath(winrt::hstring const smallPath)
{
  mImageResource.small = winrt::to_string(smallPath);
}

} // namespace winrt::PhotobookRuntimeComponent::implementation