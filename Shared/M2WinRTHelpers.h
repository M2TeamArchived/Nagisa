/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the C++/WinRT helper functions.
File Name: M2WinRTHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_WINRT_HELPERS_
#define _M2_WINRT_HELPERS_

#include <Windows.h>
#include "M2BaseHelpers.h"

#include <winrt\Windows.Foundation.h>

#include <map>
#include <string>
#include <vector>

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
GUID M2CreateGuid();

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
	bool IgnoreCase);

// Converts a numeric value into a string that represents the number expressed 
// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
// pebibytes or exbibytes, depending on the size.
// Parameters:
//   ByteSize: The numeric byte size value to be converted.
// Return value:
//   Returns a winrt::hstring object which represents the converted string.
winrt::hstring M2ConvertByteSizeToString(uint64_t ByteSize);

#endif // _M2_WINRT_HELPERS_
