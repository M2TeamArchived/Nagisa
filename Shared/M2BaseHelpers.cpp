/******************************************************************************
Project: M2-Team Common Library
Description: Implementation for the basic helper functions.
File Name: M2BaseHelpers.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include <Windows.h>

#include "M2BaseHelpers.h"

// Write formatted data to a string. 
// Parameters:
//   Format: Format-control string.
//   ...: Optional arguments to be formatted.
// Return value:
//   Returns a formatted string if successful, or "N/A" otherwise.
std::wstring M2FormatString(
	_In_z_ _Printf_format_string_ wchar_t const* const Format,
	...)
{
	// Check the argument list.
	if (nullptr != Format)
	{
		va_list ArgList = nullptr;
		va_start(ArgList, Format);

		// Get the length of rhe format result.
		size_t nLength = _vscwprintf(Format, ArgList) + 1;

		// Allocate for the format result.
		std::wstring Buffer(nLength + 1, L'\0');

		// Format the string
		int nWritten = _vsnwprintf_s(
			&Buffer[0],
			Buffer.size(),
			nLength,
			Format,
			ArgList);
		if (nWritten > 0)
		{
			// If succeed, resize to fit and return result.
			Buffer.resize(nWritten);
			return Buffer;
		}

		va_end(ArgList);
	}

	// If failed, return "N/A".
	return L"N/A";
}

// Retrieves the number of milliseconds that have elapsed since the system was
// started.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The number of milliseconds.
ULONGLONG M2GetTickCount()
{
	LARGE_INTEGER Frequency = { 0 }, PerformanceCount = { 0 };

	if (QueryPerformanceFrequency(&Frequency))
	{
		if (QueryPerformanceCounter(&PerformanceCount))
		{
			return (PerformanceCount.QuadPart * 1000 / Frequency.QuadPart);
		}
	}

	return GetTickCount64();
}