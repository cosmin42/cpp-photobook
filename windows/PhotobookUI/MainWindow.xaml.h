// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::PhotobookUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void addProjectClick(Windows::Foundation::IInspectable const    &sender,
                             Microsoft::UI::Xaml::RoutedEventArgs const &args);
    };
}

namespace winrt::PhotobookUI::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
