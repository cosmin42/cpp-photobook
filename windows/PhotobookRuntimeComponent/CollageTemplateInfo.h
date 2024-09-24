#pragma once

#include "CollageTemplateInfo.g.h"

#include <pb/entities/CollageTemplateInfo.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

struct CollageTemplateInfo : CollageTemplateInfoT<CollageTemplateInfo> {
  explicit CollageTemplateInfo(PB::CollageTemplateInfo collageTemplateInfo)
      : mCollageTemplateInfo(collageTemplateInfo)
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

  winrt::hstring Info() const { return mInfoText; }

  void Name(winrt::hstring const value)
  {
    mCollageTemplateInfo.name = winrt::to_string(value);
  }
  void Path(winrt::hstring const value)
  {
    mCollageTemplateInfo.path = PBDev::Path{winrt::to_string(value)};
  }

  void ImageCount(uint32_t value) { mCollageTemplateInfo.imageCount = value; }

  void Info(winrt::hstring value) { mInfoText = value; }

  PB::CollageTemplateInfo Unwrap() { return mCollageTemplateInfo; }

private:
  PB::CollageTemplateInfo mCollageTemplateInfo;
  winrt::hstring          mInfoText;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
