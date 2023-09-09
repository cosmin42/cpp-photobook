// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"

namespace winrt::PhotobookUI::implementation
{
    struct TableContentPage : TableContentPageT<TableContentPage>
    {
        TableContentPage();

        void onAddMediaButtonClicked(
            Windows::Foundation::IInspectable const    &sender,
            Microsoft::UI::Xaml::RoutedEventArgs const &args);

        void onBackClicked(
            Windows::Foundation::IInspectable const    &sender,
            Microsoft::UI::Xaml::RoutedEventArgs const &args);
    };
}

namespace winrt::PhotobookUI::factory_implementation
{
    struct TableContentPage : TableContentPageT<TableContentPage, implementation::TableContentPage>
    {
    };
}
