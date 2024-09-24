#pragma once

#include "PaperSettings.g.h"

#include <pb/entities/PaperSettings.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct PaperSettings : PaperSettingsT<PaperSettings> {
  explicit PaperSettings(PB::PaperSettings paperSettings)
      : mPaperSettings(paperSettings)
  {
  }
  ~PaperSettings() = default;

  void Type(PhotobookRuntimeComponent::PaperType paperType)
  {
    mPaperSettings.type = (PB::PaperType)paperType;
  }

  void Ppi(int ppi) { mPaperSettings.ppi = ppi; }
  void Width(int width) { mPaperSettings.width = width; }
  void Height(int height) { mPaperSettings.height = height; }

  PhotobookRuntimeComponent::PaperType Type()
  {
    return (PhotobookRuntimeComponent::PaperType)mPaperSettings.type;
  }

  int Ppi() const { return mPaperSettings.ppi; }
  int Width() const { return mPaperSettings.width; }
  int Height() const { return mPaperSettings.height; }

  PB::PaperSettings Unwrap() { return mPaperSettings; }

private:
  PB::PaperSettings mPaperSettings;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
