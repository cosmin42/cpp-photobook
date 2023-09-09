// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "FirstPage.g.h"

namespace winrt::PhotobookUI::implementation
{
    struct FirstPage : FirstPageT<FirstPage>
    {
        FirstPage();

        void addProjectClick(Windows::Foundation::IInspectable const    &sender,
                             Microsoft::UI::Xaml::RoutedEventArgs const &args);
    };
}

namespace winrt::PhotobookUI::factory_implementation
{
    struct FirstPage : FirstPageT<FirstPage, implementation::FirstPage>
    {
    };
}
