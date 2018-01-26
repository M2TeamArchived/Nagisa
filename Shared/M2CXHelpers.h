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
		//   This function does not return a value.
		void AddItem(IStorageItem^ Item)
		{
			String^ ItemPath = Item->Path;

			if (nullptr == this->FindToken(ItemPath))
			{
				this->m_FutureAccessList->Add(Item, ItemPath);
			}
		}

		// Finds token of the IStorageItem object from the future access list.
		// Parameters:
		//   Path: The path of the IStorageItem object. 
		// Return value:
		//   Returns a token of the IStorageItem object if found, or nullptr 
		//   otherwise.
		String^ FindToken(String^ Path)
		{
			using Windows::Storage::AccessCache::AccessListEntry;

			for (AccessListEntry Entry : this->m_FutureAccessList->Entries)
			{
				if (Path == Entry.Metadata)
				{
					return Entry.Token;
				}
			}

			return nullptr;
		}

		// Gets IStorageItem object from the future access list.
		// Parameters:
		//   Path: The path of the IStorageItem object. 
		// Return value:
		//   Returns an asynchronous object for getting the IStorageItem object
		//   if found, or nullptr otherwise.
		IAsyncOperation<IStorageItem^>^ GetItemAsync(String^ Path)
		{
			String^ ItemToken = this->FindToken(Path);

			if (nullptr != ItemToken)
			{
				return this->m_FutureAccessList->GetItemAsync(ItemToken);
			}

			return nullptr;
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

#endif // _M2_CX_HELPERS_
