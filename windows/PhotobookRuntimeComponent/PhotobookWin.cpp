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

void PhotobookWin::NewProject(winrt::hstring name,
                              winrt::hstring paperSettingsSerialized)
{
  PB::PaperSettings paperSettings;
  paperSettings.type =
      PB::deserializePaperType(winrt::to_string(paperSettingsSerialized));
  mPhotobook->newProject(winrt::to_string(name), paperSettings);
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

} // namespace winrt::PhotobookRuntimeComponent::implementation
