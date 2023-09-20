// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ExitProjectDialog.xaml.h"
#if __has_include("ExitProjectDialog.g.cpp")
#include "ExitProjectDialog.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation
{
    ExitProjectDialog::ExitProjectDialog()
    {
        InitializeComponent();
    }

    
    void ExitProjectDialog::onContentDialogSaveClicked(
        [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
        [[maybe_unused]] Microsoft::UI::Xaml::Controls::
            ContentDialogButtonClickEventArgs const &args)
    {
    }

    void ExitProjectDialog::onContentDialogDiscardClicked(
        [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
        [[maybe_unused]] Microsoft::UI::Xaml::Controls::
            ContentDialogButtonClickEventArgs const &args)
    {
    }

    void ExitProjectDialog::onContentDialogCancelClicked(
        [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
        [[maybe_unused]] Microsoft::UI::Xaml::Controls::
            ContentDialogButtonClickEventArgs const &args)
    {
    }


}
