// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "PhotobookWin.h"
#include "PhotobookWin.g.cpp"
// clang-format on

#include <filesystem>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::PhotobookRuntimeComponent::implementation {

void PhotobookWin::RecallMetadata() { mPhotobook->recallMetadata(); }

void PhotobookWin::RecallProject(winrt::hstring name)
{
  mPhotobook->recallProject(winrt::to_string(name));
}

void PhotobookWin::NewProject(
    winrt::hstring name, PhotobookRuntimeComponent::PaperSettings paperSettings)
{
  auto nativePaperSettings =
      winrt::get_self<
          winrt::PhotobookRuntimeComponent::implementation::PaperSettings>(
          paperSettings)
          ->Unwrap();

  mPhotobook->newProject(winrt::to_string(name), nativePaperSettings);
}
void PhotobookWin::UnloadProject() { mPhotobook->unloadProject(); }

void PhotobookWin::AddImportFolder(winrt::hstring importPath)
{
  mPhotobook->addImportFolder(winrt::to_string(importPath));
}
void PhotobookWin::RemoveImportFolder(winrt::hstring path)
{
  mPhotobook->removeImportFolder(winrt::to_string(path));
}

void PhotobookWin::ExportPDFAlbum(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportPDFAlbum(winrt::to_string(name), winrt::to_string(path));
}
void PhotobookWin::ExportPDFLibharu(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportPDFLibharu(winrt::to_string(name), winrt::to_string(path));
}
void PhotobookWin::ExportJPGAlbum(winrt::hstring name, winrt::hstring path)
{
  mPhotobook->exportJPGAlbum(winrt::to_string(name), winrt::to_string(path));
}

void PhotobookWin::mapImagesToSPL(
    Windows::Foundation::Collections::IMap<
        winrt::guid, PhotobookRuntimeComponent::VirtualImagePtr>
        frontEndImages)
{
  auto imageToPaperService = mPhotobook->imageToPaperService();

  std::unordered_map<PBDev::ImageToPaperId, PB::GenericImagePtr,
                     boost::hash<PBDev::ImageToPaperId>>
      backendMap;

  for (auto entry : frontEndImages) {

    auto frontendGuid = entry.Key();
    // TODO: Move this conversion to dedicated function
    boost::uuids::uuid nativeUuid;

    nativeUuid.data[0] = ((frontendGuid.Data1 & 0xFF000000) >> 24);
    nativeUuid.data[1] = (uint8_t)((frontendGuid.Data1 & 0xFF0000) >> 16);
    nativeUuid.data[2] = ((frontendGuid.Data1 & 0xFF00) >> 8);
    nativeUuid.data[3] = (frontendGuid.Data1 & 0xFF);

    nativeUuid.data[4] = ((frontendGuid.Data2 & 0xFF00) >> 8);
    nativeUuid.data[5] = (frontendGuid.Data2 & 0xFF);

    nativeUuid.data[6] = ((frontendGuid.Data3 & 0xFF00) >> 8);
    nativeUuid.data[7] = (frontendGuid.Data3 & 0xFF);

    for (int i = 0; i < 8; i++) {
      nativeUuid.data[8 + i] = frontendGuid.Data4[i];
    }

    PBDev::ImageToPaperId imageId = PBDev::ImageToPaperId(nativeUuid);

    auto nativePtr =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
            entry.Value())
            ->Unwrap();

    backendMap[imageId] = nativePtr;
  }

  imageToPaperService->map(
      PBDev::ImageToPaperServiceId(PB::RuntimeUUID::newUUID()), backendMap);
}

void PhotobookWin::StopProjectWork() { mPhotobook->stopProjectWork(); }

bool PhotobookWin::IsProjectWorking() { return mPhotobook->isProjectWorking(); }

} // namespace winrt::PhotobookRuntimeComponent::implementation
