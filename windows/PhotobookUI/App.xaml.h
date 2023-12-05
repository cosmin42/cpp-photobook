// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "App.xaml.g.h"

#include <pb/persistence/Persistence.h>
#include <pb/PhotoBook.h>

namespace winrt::PhotobookUI::implementation {
struct App : AppT<App>,
             public PB::PersistenceMetadataListener,
             public PB::PersistenceProjectListener {
  static PB::Path CurrentAppLocation();

  App();

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const &);

  void OnNavigationFailed(
      IInspectable const &,
      Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

  void onProjectRead(PB::Project project) override;
  void onMetadataRead(PB::ProjectMetadata projectMetadata) override;
  void
  onMetadataRead(std::vector<PB::ProjectMetadata> projectMetadata) override;
  void onProjectPersistenceError(PB::Error) override;
  void onMetadataPersistenceError(PB::Error) override;

  std::shared_ptr<PB::Persistence> persistence() { return mPersistence; }

  std::shared_ptr<PB::Photobook> api() const;

private:
  winrt::Microsoft::UI::Xaml::Window mWindow{nullptr};

  std::shared_ptr<PB::Persistence> mPersistence;

  std::shared_ptr<PB::Photobook> mAPI;
};
} // namespace winrt::PhotobookUI::implementation
