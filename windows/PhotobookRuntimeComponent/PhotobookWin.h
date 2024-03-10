#pragma once

#include "PhotobookWin.g.h"

#include "ImageViews.h"

#include <pb/PhotoBook.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct PhotobookWin : PhotobookWinT<PhotobookWin> {
  PhotobookWin(winrt::hstring localStatePath, winrt::hstring installPath)
  {
    mPhotobook = std::make_shared<PB::Photobook>(
        winrt::to_string(localStatePath), winrt::to_string(installPath));
  }

  ~PhotobookWin() = default;

  void RecallMetadata();
  void RecallProject(winrt::hstring name);

  void NewProject(winrt::hstring name);
  void UnloadProject();

  PhotobookRuntimeComponent::ImageViews GetImageViews()
  {
    return winrt::make<ImageViews>(mPhotobook->imageViews());
  }

  void AddImportFolder(winrt::hstring importPath);
  void RemoveImportFolder(winrt::hstring path);

  void ExportPDFAlbum(winrt::hstring name, winrt::hstring path);
  void ExportPDFLibharu(winrt::hstring name, winrt::hstring path);
  void ExportJPGAlbum(winrt::hstring name, winrt::hstring path);

private:
  std::shared_ptr<PB::Photobook> mPhotobook = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation