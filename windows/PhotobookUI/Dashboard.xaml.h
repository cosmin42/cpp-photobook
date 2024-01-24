// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "Dashboard.g.h"
#include "ProjectItem.g.h"

#include <pb/PhotoBook.h>
#include <pb/PhotobookListener.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct Dashboard : DashboardT<Dashboard>, public PB::PhotobookListener {
  Dashboard();

  void AddProjectClicked(Windows::Foundation::IInspectable const    &sender,
                         Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnError(PBDev::Error err);

  void OnListViewRightTapped(
      winrt::Windows::Foundation::IInspectable const &,
      winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const &);

  void OnDeleteClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &);

  void OnRenameClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &);

  void OpenProjectClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::
          ItemClickEventArgs const &);

  void OnRenameProjectDialogRename(
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Controls::
          ContentDialogButtonClickEventArgs const &args);

  void OnRenameProjectDialogCancel(
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Controls::
          ContentDialogButtonClickEventArgs const &args);

  auto RenameProjectDialogDisplay() -> winrt::fire_and_forget;

  void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

  virtual void onExportFinished() override;
  virtual void onError(PBDev::Error) override;
  virtual void
  onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                     int index = -1) override;
  virtual void
  onStagedImageRemoved(std::vector<unsigned> removedIndexes) override;

  virtual void onMappingStarted(Path path) override;
  virtual void onMappingFinished(Path path) override;
  virtual void onMappingAborted(Path path) override;

  virtual void onImageUpdated(Path root, int row, int index) override;

  virtual void post(std::function<void()> f) override;

  void onProjectRead() override;

  void onMetadataUpdated() override;

  void onPersistenceError(PBDev::Error) override;

  void onProjectRenamed() override;

  void onProgressUpdate(std::vector<std::string> names,
                        PB::ProgressInfo             definedProgress,
                        PB::ProgressInfo             undefinedProgress) override;

private:
  Microsoft::UI::Xaml::Controls::MenuFlyoutItem DeleteFlyout();
  Microsoft::UI::Xaml::Controls::MenuFlyoutItem RenameFlyout();

  std::string GenerateProjectName();

  IObservableVector<ProjectItem> mProjectsList;

  Microsoft::UI::Xaml::Controls::MenuFlyout mMenuFlyout;

  winrt::hstring mRightClickedId;
  winrt::hstring mOldProjectName;
  winrt::hstring mProjectUUID;

  std::shared_ptr<PB::Photobook> mAPI = nullptr;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct Dashboard : DashboardT<Dashboard, implementation::Dashboard> {};
} // namespace winrt::PhotobookUI::factory_implementation
