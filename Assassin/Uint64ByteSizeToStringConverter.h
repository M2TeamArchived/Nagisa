/******************************************************************************
Project: Assassin
Description: Definition for the Uint64ByteSizeToStringConverter.
File Name: Uint64ByteSizeToStringConverter.h
License: The MIT License
******************************************************************************/

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include "Uint64ByteSizeToStringConverter.g.h"

namespace winrt
{
	using Windows::Foundation::IInspectable;
	using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::Assassin::implementation
{
    struct Uint64ByteSizeToStringConverter : 
		Uint64ByteSizeToStringConverterT<Uint64ByteSizeToStringConverter>
    {
		Uint64ByteSizeToStringConverter() = default;

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
    struct Uint64ByteSizeToStringConverter : Uint64ByteSizeToStringConverterT<
		Uint64ByteSizeToStringConverter,
		implementation::Uint64ByteSizeToStringConverter>
    {
    };
}
