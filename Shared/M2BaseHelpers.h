/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the basic helper functions.
File Name: M2BaseHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_BASE_HELPERS_
#define _M2_BASE_HELPERS_

#include <Windows.h>

// Write formatted data to a string. 
// Parameters:
//   Format: Format-control string.
//   ...: Optional arguments to be formatted.
// Return value:
//   Returns a formatted string if successful, or an empty string otherwise.
std::wstring M2FormatString(
	_In_z_ _Printf_format_string_ wchar_t const* const Format,
	...);

// Retrieves the number of milliseconds that have elapsed since the system was
// started.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The number of milliseconds.
ULONGLONG M2GetTickCount();

// Searches a path for a file name.
// Parameters:
//   Path: A pointer to a null-terminated string of maximum length MAX_PATH 
//   that contains the path to search.
// Return value:
//   Returns a pointer to the address of the string if successful, or a pointer
//   to the beginning of the path otherwise.
template<typename CharType>
CharType M2PathFindFileName(CharType Path)
{
	CharType FileName = Path;

	for (size_t i = 0; i < MAX_PATH; ++i)
	{
		if (!(Path && *Path))
			break;

		if (L'\\' == *Path || L'/' == *Path)
			FileName = Path + 1;

		++Path;
	}

	return FileName;
}

#endif // _M2_BASE_HELPERS_
