// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "FirstPage.xaml.h"
#if __has_include("FirstPage.g.cpp")
#include "FirstPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::PhotobookUI::implementation
{
    FirstPage::FirstPage()
    {
        InitializeComponent();
    }

    void FirstPage::addProjectClick(IInspectable const &,
                                     RoutedEventArgs const &)
    {
        MainFrame().Navigate(winrt::xaml_typename<TableContentPage>());
    }
    }
