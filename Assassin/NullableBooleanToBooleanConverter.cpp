/******************************************************************************
Project: Assassin
Description: Implementation for NullableBooleanToBooleanConverter.
File Name: NullableBooleanToBooleanConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "NullableBooleanToBooleanConverter.h"

using namespace winrt::Assassin::implementation;


winrt::IInspectable NullableBooleanToBooleanConverter::Convert(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);
	
	return winrt::box_value(winrt::unbox_value_or<bool>(value, false));
}

winrt::IInspectable NullableBooleanToBooleanConverter::ConvertBack(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);
	
	return winrt::box_value(winrt::unbox_value_or<bool>(value, false));
}
