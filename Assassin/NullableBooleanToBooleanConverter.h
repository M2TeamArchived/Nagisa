/******************************************************************************
Project: Assassin
Description: Definition for the NullableBooleanToBooleanConverter.
File Name: NullableBooleanToBooleanConverter.h
License: The MIT License
******************************************************************************/

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include "NullableBooleanToBooleanConverter.g.h"

namespace winrt
{
	using Windows::Foundation::IInspectable;
	using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::Assassin::implementation
{
    struct NullableBooleanToBooleanConverter :
		NullableBooleanToBooleanConverterT<NullableBooleanToBooleanConverter>
    {
		NullableBooleanToBooleanConverter() = default;

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
    struct NullableBooleanToBooleanConverter : 
		NullableBooleanToBooleanConverterT<
			NullableBooleanToBooleanConverter, 
			implementation::NullableBooleanToBooleanConverter>
    {
    };
}
