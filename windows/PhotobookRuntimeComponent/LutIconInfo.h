#pragma once

#include "LutIconInfo.g.h"

#include <pb/LutIconInfo.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

struct LutIconInfo : LutIconInfoT<LutIconInfo> {
  explicit LutIconInfo(PB::LutIconInfo lutIconInfo) : mLutIconInfo(lutIconInfo)
  {
  }

  ~LutIconInfo() = default;

  winrt::hstring Name() const { return winrt::to_hstring(mLutIconInfo.name); }

  winrt::hstring Path() const
  {
    return winrt::to_hstring(mLutIconInfo.path.string());
  }

  void Name(winrt::hstring const value)
  {
    mLutIconInfo.name = winrt::to_string(value);
  }
  void Path(winrt::hstring const value)
  {
    mLutIconInfo.path = PBDev::Path{winrt::to_string(value)};
  }

  PB::LutIconInfo Unwrap() { return mLutIconInfo; }

private:
  PB::LutIconInfo mLutIconInfo;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
