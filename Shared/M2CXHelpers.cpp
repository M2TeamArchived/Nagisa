/******************************************************************************
Project: M2-Team Common Library
Description: Implemention for the C++/CX helper functions.
File Name: M2CXHelpers
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
void M2ThrowPlatformException(HRESULT hr)
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
