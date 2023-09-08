// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"

namespace winrt::PhotobookUI::implementation
{
    struct TableContentPage : TableContentPageT<TableContentPage>
    {
        TableContentPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::PhotobookUI::factory_implementation
{
    struct TableContentPage : TableContentPageT<TableContentPage, implementation::TableContentPage>
    {
    };
}
