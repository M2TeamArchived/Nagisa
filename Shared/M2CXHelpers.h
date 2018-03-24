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

#include "M2BaseHelpers.h"

#include <string>

// Retrieve the IInspectable interface from the provided C++/CX object.
// Parameters:
//   object: The C++/CX object you want to retrieve the raw pointer.
// Return value:
//   Return the IInspectable interface from the provided C++/CX object.
inline IInspectable* M2GetInspectable(Platform::Object^ object)
{
	return reinterpret_cast<IInspectable*>(object);
}

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception.
__declspec(noreturn) void M2ThrowPlatformException(HRESULT hr);

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception
//   if it is a failed HRESULT value.
void M2ThrowPlatformExceptionIfFailed(HRESULT hr);

// Convert C++/CX exceptions in the callable code into HRESULTs.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return HRESULT.
HRESULT M2ThrownPlatformExceptionToHResult();

#ifdef __cplusplus_winrt

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
	bool IgnoreCase);

// Converts a numeric value into a string that represents the number expressed 
// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
// pebibytes or exbibytes, depending on the size.
// Parameters:
//   ByteSize: The numeric byte size value to be converted.
// Return value:
//   Returns a Platform::String object which represents the converted string.
Platform::String^ M2ConvertByteSizeToString(uint64 ByteSize);

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return Platform::Guid object.
Platform::Guid M2CreateGuid();

namespace M2
{
	using Platform::String;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Storage::AccessCache::IStorageItemAccessList;
	using Windows::Storage::IStorageItem;

	class CFutureAccessList
	{
	private:
		IStorageItemAccessList ^ m_FutureAccessList = nullptr;

	public:
		// Creates a new CFutureAccessList object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		CFutureAccessList()
		{
			using Windows::Storage::AccessCache::StorageApplicationPermissions;
			this->m_FutureAccessList =
				StorageApplicationPermissions::FutureAccessList;
		}

		// Adds IStorageItem object to the future access list.
		// Parameters:
		//   Item: The IStorageItem object which you want to add. 
		// Return value:
		//   A token that the app can use later to retrieve the storage item.
		String^ AddItem(IStorageItem^ Item)
		{
			return this->m_FutureAccessList->Add(Item);
		}

		// Gets IStorageItem object from the future access list.
		// Parameters:
		//   Token: The token of the IStorageItem object. 
		// Return value:
		//   When this method completes successfully, it returns the item (type
		//   IStorageItem ) that is associated with the specified token.
		IAsyncOperation<IStorageItem^>^ GetItemAsync(String^ Token)
		{
			return this->m_FutureAccessList->GetItemAsync(Token);
		}

		// Gets IStorageItemAccessList object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns an IStorageItemAccessList object.
		IStorageItemAccessList^ Get()
		{
			return this->m_FutureAccessList;
		}
	};
}

// Retrieves the raw pointer from the provided IBuffer object. 
// Parameters:
//   Buffer: The IBuffer object you want to retrieve the raw pointer.
// Return value:
//   If the function succeeds, the return value is the raw pointer from the 
//   provided IBuffer object. If the function fails, the return value is 
//   nullptr.
// Warning: 
//   The lifetime of the returned buffer is controlled by the lifetime of the 
//   buffer object that's passed to this method. When the buffer has been 
//   released, the pointer becomes invalid and must not be used.
byte* M2GetPointer(Windows::Storage::Streams::IBuffer^ Buffer);

// Retrieves the IBuffer object from the provided raw pointer.
// Parameters:
//   Pointer: The raw pointer you want to retrieve the IBuffer object.
//   Capacity: The size of raw pointer you want to retrieve the IBuffer object.
// Return value:
//   If the function succeeds, the return value is the IBuffer object from the 
//   provided raw pointer. If the function fails, the return value is nullptr.
// Warning: 
//   The lifetime of the returned IBuffer object is controlled by the lifetime 
//   of the raw pointer that's passed to this method. When the raw pointer has 
//   been released, the IBuffer object becomes invalid and must not be used.
Windows::Storage::Streams::IBuffer^ M2MakeIBuffer(
	byte* Pointer,
	UINT32 Capacity);

// Converts from the C++/CX string to the UTF-16 string.
// Parameters:
//   PlatformString: The C++/CX string you want to convert.
// Return value:
//   The return value is the UTF-16 string.
std::wstring M2MakeUTF16String(Platform::String^ PlatformString);

// Converts from the C++/CX string to the UTF-8 string.
// Parameters:
//   PlatformString: The C++/CX string you want to convert.
// Return value:
//   The return value is the UTF-8 string.
std::string M2MakeUTF8String(Platform::String^ PlatformString);

// Converts from the UTF-8 string to the C++/CX string.
// Parameters:
//   UTF16String: The UTF-16 string you want to convert.
// Return value:
//   The return value is the C++/CX string.
Platform::String^ M2MakeCXString(const std::wstring& UTF16String);

#endif

#endif // _M2_CX_HELPERS_
