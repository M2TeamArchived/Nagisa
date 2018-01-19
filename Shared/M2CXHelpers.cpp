/******************************************************************************
Project: M2-Team Common Library
Description: Implemention for the C++/CX helper functions.
File Name: M2CXHelpers.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include <Windows.h>

#include "M2CXHelpers.h"

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception.
__declspec(noreturn) void M2ThrowPlatformException(HRESULT hr)
{
	throw Platform::Exception::CreateException(hr);
}

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception
//   if it is a failed HRESULT value.
void M2ThrowPlatformExceptionIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		M2ThrowPlatformException(hr);
	}
}

// Convert C++/CX exceptions in the callable code into HRESULTs.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return HRESULT.
HRESULT M2ThrownPlatformExceptionToHResult()
{
	try
	{
		throw;
	}
	catch (Platform::Exception^ ex)
	{
		return ex->HResult;
	}
	catch (std::bad_alloc const&)
	{
		return E_OUTOFMEMORY;
	}
	catch (...)
	{
		return E_UNEXPECTED;
	}
}

// Finds a sub string from a source string. 
// Parameters:
//   SourceString: The source string.
//   SubString: The sub string.
//   IgnoreCase: Determines whether to ignore case.
// Return value:
//   Returns true if successful, or false otherwise.
bool M2FindSubString(
	Platform::String^ SourceString,
	Platform::String^ SubString,
	bool IgnoreCase)
{
	return (::FindNLSStringEx(
		nullptr,
		(IgnoreCase ? NORM_IGNORECASE : 0) | FIND_FROMSTART,
		SourceString->Data(),
		SourceString->Length(),
		SubString->Data(),
		SubString->Length(),
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
//   Returns a Platform::String object which represents the converted string.
Platform::String^ M2ConvertByteSizeToString(uint64 ByteSize)
{
	double result = static_cast<double>(ByteSize);

	if (0.0 == result)
	{
		return L"0 Byte";
	}

	const wchar_t* Systems[] =
	{
		L"Bytes",
		L"KiB",
		L"MiB",
		L"GiB",
		L"TiB",
		L"PiB",
		L"EiB"
	};

	size_t nSystem = 0;
	for (; nSystem < sizeof(Systems) / sizeof(*Systems); ++nSystem)
	{
		if (1024.0 > result)
			break;

		result /= 1024.0;
	}

	Platform::String^ ByteSizeString = 
		(static_cast<uint64>(result * 100) / 100.0).ToString();

	return ByteSizeString + Platform::StringReference(Systems[nSystem]);
}

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return Platform::Guid object.
Platform::Guid M2CreateGuid()
{
	GUID guid = { 0 };
	M2ThrowPlatformExceptionIfFailed(CoCreateGuid(&guid));
	return Platform::Guid(guid);
}
