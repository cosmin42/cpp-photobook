// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "PhotobookWin.h"
#include "PhotobookWin.g.cpp"
// clang-format on

#include <filesystem>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include "WinConversions.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

void PhotobookWin::MakeCollages() { mPhotobook->makeCollages(); }

void PhotobookWin::UnloadProject() { mPhotobook->unloadProject(); }

void PhotobookWin::AddImportFolder(winrt::hstring importPath)
{
  mPhotobook->importFoldersService()->addImportFolder(
      winrt::to_string(importPath));
}
void PhotobookWin::RemoveImportFolder(winrt::hstring path)
{
  mPhotobook->removeImportFolder(winrt::to_string(path));
}

void PhotobookWin::ExportPDFAlbum(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportService()->exportPDFAlbum(winrt::to_string(name),
                                              winrt::to_string(path));
}
void PhotobookWin::ExportPDFLibharu(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportService()->exportPDFLibharu(winrt::to_string(name),
                                                winrt::to_string(path));
}
void PhotobookWin::ExportJPGAlbum(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportService()->exportJPGAlbum(winrt::to_string(name),
                                              winrt::to_string(path));
}

void PhotobookWin::mapImagesToSPL(
    Windows::Foundation::Collections::IMap<
        winrt::guid, PhotobookRuntimeComponent::VirtualImagePtr>
        frontEndImages)
{
  auto imageToPaperService = mPhotobook->imageToPaperService();

  std::unordered_map<PBDev::ImageToPaperId, PB::ImageToPaperData,
                     boost::hash<PBDev::ImageToPaperId>>
      backendMap;

  for (auto entry : frontEndImages) {

    auto frontendGuid = entry.Key();
    auto nativeUuid = WinConversions::toNativeUuid(frontendGuid);

    PBDev::ImageToPaperId imageId = PBDev::ImageToPaperId(nativeUuid);

    auto nativePtr =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
            entry.Value())
            ->Unwrap();

    backendMap[imageId].image = nativePtr;
    backendMap[imageId].backgroundColor = {255, 255, 255};
    backendMap[imageId].overlapType = PB::Geometry::OverlapType::Inscribed;
  }

  auto maybeProject =
      mPhotobook->projectManagementService()->maybeLoadedProjectInfo();

  imageToPaperService->toPaper(
      PBDev::ImageToPaperServiceId(PB::RuntimeUUID::newUUID()), backendMap);
}

} // namespace winrt::PhotobookRuntimeComponent::implementation
