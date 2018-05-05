/******************************************************************************
Project: Assassin
Description: Implementation for Uint64RemainTimeToStringConverter.
File Name: Uint64RemainTimeToStringConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "Uint64RemainTimeToStringConverter.h"

using namespace winrt::Assassin::implementation;


winrt::IInspectable Uint64RemainTimeToStringConverter::Convert(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);

	uint64_t Seconds = winrt::unbox_value_or<uint64_t>(
		value, static_cast<uint64_t>(-1));

	if (static_cast<uint64_t>(-1) != Seconds)
	{
		int Hour = static_cast<int>(Seconds / 3600);
		int Minute = static_cast<int>(Seconds / 60 % 60);
		int Second = static_cast<int>(Seconds % 60);

		return winrt::box_value(
			M2FormatString(L"%d:%02d:%02d", Hour, Minute, Second));
	}
	
	return winrt::box_value(L"N/A");
}

winrt::IInspectable Uint64RemainTimeToStringConverter::ConvertBack(
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
