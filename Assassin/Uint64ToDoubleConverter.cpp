/******************************************************************************
Project: Assassin
Description: Implementation for Uint64ToDoubleConverter.
File Name: Uint64ToDoubleConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "Uint64ToDoubleConverter.h"

using namespace winrt::Assassin::implementation;


winrt::IInspectable Uint64ToDoubleConverter::Convert(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);
	
	return winrt::box_value(
		static_cast<double>(winrt::unbox_value_or<uint64_t>(value, 0)));
}

winrt::IInspectable Uint64ToDoubleConverter::ConvertBack(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(value);
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);
	
	winrt::throw_hresult(E_NOTIMPL);
}
