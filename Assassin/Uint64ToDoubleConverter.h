/******************************************************************************
Project: Assassin
Description: Definition for the Uint64ToDoubleConverter.
File Name: Uint64ToDoubleConverter.h
License: The MIT License
******************************************************************************/

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include "Uint64ToDoubleConverter.g.h"

namespace winrt
{
	using Windows::Foundation::IInspectable;
	using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::Assassin::implementation
{
    struct Uint64ToDoubleConverter :
		Uint64ToDoubleConverterT<Uint64ToDoubleConverter>
    {
		Uint64ToDoubleConverter() = default;

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

namespace winrt::Assassin::factory_implementation
{
    struct Uint64ToDoubleConverter : Uint64ToDoubleConverterT<
		Uint64ToDoubleConverter, 
		implementation::Uint64ToDoubleConverter>
    {
    };
}
