/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the C++/CX helper functions.
File Name: M2CXHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_CX_HELPERS_
#define _M2_CX_HELPERS_

#include <Windows.h>

// Retrieve the IInspectable interface from the provided C++/CX object.
// Parameters:
//   object: The C++/CX object you want to retrieve the raw pointer.
// Return value:
//   Return the IInspectable interface from the provided C++/CX object.
inline IInspectable* M2GetInspectable(Platform::Object^ object) throw()
{
	return reinterpret_cast<IInspectable*>(object);
}

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception.
void M2ThrowPlatformException(HRESULT hr);

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value.
void M2ThrowPlatformExceptionIfFailed(HRESULT hr);

// Convert C++/CX exceptions in the callable code into HRESULTs.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return HRESULT.
HRESULT M2ThrownPlatformExceptionToHResult();

#endif // _M2_CX_HELPERS_
