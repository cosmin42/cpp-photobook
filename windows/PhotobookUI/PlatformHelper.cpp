#include "pch.h"

#include "PlatformHelper.h"

#include <winrt/Windows.Storage.h>

Path PlatformHelper::CurrentAppLocation()
{
  winrt::Windows::Storage::StorageFolder folder =
      winrt::Windows::Storage::ApplicationData::Current().LocalFolder();

  return Path(winrt::to_string(folder.Path()));
}

Path PlatformHelper::CurrentAppInstallationFolder()
{
  winrt::Windows::ApplicationModel::Package package =
      winrt::Windows::ApplicationModel::Package::Current();
  winrt::Windows::Storage::StorageFolder installedLocation =
      package.InstalledLocation();

  return winrt::to_string(installedLocation.Path());
}