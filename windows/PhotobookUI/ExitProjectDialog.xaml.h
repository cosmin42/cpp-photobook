// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "ExitProjectDialog.g.h"

namespace winrt::PhotobookUI::implementation {
struct ExitProjectDialog : ExitProjectDialogT<ExitProjectDialog> {
  ExitProjectDialog();

  void
  onContentDialogSaveClicked(Windows::Foundation::IInspectable const    &sender,
                             Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const &args);

  void onContentDialogDiscardClicked(
      Windows::Foundation::IInspectable const    &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void onContentDialogCancelClicked(
      Windows::Foundation::IInspectable const    &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct ExitProjectDialog
    : ExitProjectDialogT<ExitProjectDialog, implementation::ExitProjectDialog> {
};
} // namespace winrt::PhotobookUI::factory_implementation
