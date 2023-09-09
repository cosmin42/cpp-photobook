// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "TableContentPage.xaml.h"
#if __has_include("TableContentPage.g.cpp")
#include "TableContentPage.g.cpp"
#endif

#include "FirstPage.xaml.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation
{
    TableContentPage::TableContentPage()
    {
        InitializeComponent();
    }

    void TableContentPage::onAddMediaButtonClicked(IInspectable const &,
                                                   RoutedEventArgs const &)
    {
    }

    void TableContentPage::onBackClicked(IInspectable const &,
                                                   RoutedEventArgs const &)
    {
      Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
    }
}
