// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::PhotobookUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        static HWND sMainWindowhandle;

        static winrt::Microsoft::UI::Dispatching::DispatcherQueue sMainthreadDispatcher;

        MainWindow();
    };
}

namespace winrt::PhotobookUI::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
