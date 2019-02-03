﻿/*
 * PROJECT:   Nagisa
 * FILE:      CustomConverter.h
 * PURPOSE:   Definition for the CustomConverter.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include "CustomConverter.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::Nagisa::implementation
{
    struct CustomConverter :
        CustomConverterT<CustomConverter>
    {
        CustomConverter() = default;

        winrt::IInspectable Convert(
            winrt::IInspectable const& value,
            winrt::TypeName const& targetType,
            winrt::IInspectable const& parameter,
            winrt::hstring const& language) const;

        winrt::IInspectable ConvertBack(
            winrt::IInspectable const& value,
            winrt::TypeName const& targetType,
            winrt::IInspectable const& parameter,
            winrt::hstring const& language) const;
    };
}

namespace winrt::Nagisa::factory_implementation
{
    struct CustomConverter : CustomConverterT<
        CustomConverter,
        implementation::CustomConverter>
    {
    };
}
