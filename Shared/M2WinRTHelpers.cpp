/******************************************************************************
Project: M2-Team Common Library
Description: Implementation for the C++/WinRT helper functions.
File Name: M2WinRTHelpers.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include <Windows.h>

#ifdef _M2_WINRT_HELPERS_

#include "M2WinRTHelpers.h"

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
GUID M2CreateGuid()
{
	GUID guid = { 0 };
	winrt::check_hresult(CoCreateGuid(&guid));
	return guid;
}

// Finds a sub string from a source string. 
// Parameters:
//   SourceString: The source string.
//   SubString: The sub string.
//   IgnoreCase: Determines whether to ignore case.
// Return value:
//   Returns true if successful, or false otherwise.
bool M2FindSubString(
	winrt::hstring SourceString,
	winrt::hstring SubString,
	bool IgnoreCase)
{
	return (::FindNLSStringEx(
		nullptr,
		(IgnoreCase ? NORM_IGNORECASE : 0) | FIND_FROMSTART,
		SourceString.c_str(),
		SourceString.size(),
		SubString.c_str(),
		SubString.size(),
		nullptr,
		nullptr,
		nullptr,
		0) >= 0);
}

// Converts a numeric value into a string that represents the number expressed 
// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
// pebibytes or exbibytes, depending on the size.
// Parameters:
//   ByteSize: The numeric byte size value to be converted.
// Return value:
//   Returns a winrt::hstring object which represents the converted string.
winrt::hstring M2ConvertByteSizeToString(uint64_t ByteSize)
{
	const wchar_t* Systems[] =
	{
		L"Byte",
		L"Bytes",
		L"KiB",
		L"MiB",
		L"GiB",
		L"TiB",
		L"PiB",
		L"EiB"
	};

	size_t nSystem = 0;
	double result = static_cast<double>(ByteSize);

	if (ByteSize > 1)
	{
		for (
			nSystem = 1; 
			nSystem < sizeof(Systems) / sizeof(*Systems); 
			++nSystem)
		{
			if (1024.0 > result)
				break;

			result /= 1024.0;
		}

		result = static_cast<uint64_t>(result * 100) / 100.0;
	}

	return winrt::to_hstring(result) + L" " + Systems[nSystem];
}

namespace M2
{
	void NotifyPropertyChangedBase::RaisePropertyChanged(
		winrt::hstring PropertyName)
	{
		this->m_PropertyChanged(
			*this, winrt::PropertyChangedEventArgs(PropertyName));
	}

	winrt::event_token NotifyPropertyChangedBase::PropertyChanged(
		winrt::PropertyChangedEventHandler const& value)
	{
		return this->m_PropertyChanged.add(value);
	}

	void NotifyPropertyChangedBase::PropertyChanged(
		winrt::event_token const & token)
	{
		this->m_PropertyChanged.remove(token);
	}
}

#endif // _M2_WINRT_HELPERS_
