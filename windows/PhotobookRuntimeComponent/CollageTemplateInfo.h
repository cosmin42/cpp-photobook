#pragma once

#include "CollageTemplateInfo.g.h"

#include <pb/CollageTemplateInfo.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

struct CollageTemplateInfo : CollageTemplateInfoT<CollageTemplateInfo> {
  CollageTemplateInfo() {}
  explicit CollageTemplateInfo(winrt::hstring name, winrt::hstring path,
                               unsigned imageCount)
      : mCollageTemplateInfo{winrt::to_string(name),
                             PBDev::Path(winrt::to_string(path)), imageCount}
  {
  }
  ~CollageTemplateInfo() = default;

  winrt::hstring Name() const
  {
    return winrt::to_hstring(mCollageTemplateInfo.name);
  }
  winrt::hstring Path() const
  {
    return winrt::to_hstring(mCollageTemplateInfo.path.string());
  }

  uint32_t ImageCount() const { return mCollageTemplateInfo.imageCount; }

  void Name(winrt::hstring const value)
  {
    mCollageTemplateInfo.name = winrt::to_string(value);
  }
  void Path(winrt::hstring const value)
  {
    mCollageTemplateInfo.path = PBDev::Path{winrt::to_string(value)};
  }

  void ImageCount(uint32_t value) { mCollageTemplateInfo.imageCount = value; }

  PB::CollageTemplateInfo Unwrap() { return mCollageTemplateInfo; }

private:
  PB::CollageTemplateInfo mCollageTemplateInfo;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct CollageTemplateInfo
    : CollageTemplateInfoT<CollageTemplateInfo,
                           implementation::CollageTemplateInfo> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
