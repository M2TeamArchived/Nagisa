/******************************************************************************
Project: M2-Team Common Library
Description: Implementation for the C++/CX helper functions.
File Name: M2CXHelpers.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#ifdef _M2_CX_HELPERS_

#include <Windows.h>
#include <wrl\client.h>
#include <wrl\implements.h>
#include <robuffer.h>

#include <windows.foundation.h>
#include <windows.storage.streams.h>

#include <string>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::MakeAndInitialize;
using Microsoft::WRL::RuntimeClass;
using Microsoft::WRL::RuntimeClassFlags;
using Microsoft::WRL::RuntimeClassType;

#include "M2CXHelpers.h"

// Handle the completed asynchronous call.
// Parameters:
//   Async: The completed asynchronous call you want to handle.
// Return value:
//   Return the HRESULT determined by the asynchronous call.
HRESULT M2AsyncHandleCompleted(Platform::Object^ Async)
{
	HRESULT hr = S_OK;
	ABI::Windows::Foundation::IAsyncInfo* asyncInfo = nullptr;

	hr = M2GetInspectable(Async)->QueryInterface(&asyncInfo);
	if (SUCCEEDED(hr))
	{
		// Get the error code.
		AsyncStatus asyncStatus;
		hr = asyncInfo->get_Status(&asyncStatus);
		if (SUCCEEDED(hr))
		{
			if (AsyncStatus::Completed == asyncStatus)
			{
				// Just return S_OK if succeeded.
				hr = S_OK;
			}
			else if (AsyncStatus::Started == asyncStatus)
			{
				// Cancel the asynchronous call and return error code if 
				// the status is still Started, the timeout interval has
				// been elapsed.
				hr = asyncInfo->Cancel();
				if (SUCCEEDED(hr)) hr = __HRESULT_FROM_WIN32(ERROR_TIMEOUT);
			}
			else if (AsyncStatus::Canceled == asyncStatus)
			{
				// If the status is Cancelled, return the error code.
				hr = E_ABORT;
			}
			else
			{
				HRESULT hrTemp;

				// If the status is other value, return the error code.	
				hr = asyncInfo->get_ErrorCode(&hrTemp);
				if (SUCCEEDED(hr)) hr = hrTemp;
			}
		}

		asyncInfo->Release();
	}

	return hr;
}

// Execute function on the UI thread with normal priority.
// Parameters:
//   agileCallback: The function you want to execute.
// Return value:
//   The return value is Windows::Foundation::IAsyncAction^.
Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(
	Windows::UI::Core::DispatchedHandler^ agileCallback)
{
	using Windows::ApplicationModel::Core::CoreApplication;
	using Windows::UI::Core::CoreDispatcherPriority;

	return CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal, agileCallback);
}

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
byte* M2GetPointer(Windows::Storage::Streams::IBuffer^ Buffer)
{
	byte* pBuffer = nullptr;
	Windows::Storage::Streams::IBufferByteAccess* pBufferByteAccess = nullptr;
	IInspectable* pBufferABIObject = M2GetInspectable(Buffer);
	if (SUCCEEDED(pBufferABIObject->QueryInterface(&pBufferByteAccess)))
	{
		pBufferByteAccess->Buffer(&pBuffer);
		pBufferByteAccess->Release();
	}

	return pBuffer;
}

class BufferReference : public RuntimeClass<
	RuntimeClassFlags<RuntimeClassType::WinRtClassicComMix>,
	ABI::Windows::Storage::Streams::IBuffer,
	Windows::Storage::Streams::IBufferByteAccess>
{
private:
	UINT32 m_Capacity;
	UINT32 m_Length;
	byte* m_Pointer;

public:
	virtual ~BufferReference()
	{
	}

	STDMETHODIMP RuntimeClassInitialize(
		byte* Pointer, UINT32 Capacity)
	{
		m_Capacity = Capacity;
		m_Length = Capacity;
		m_Pointer = Pointer;
		return S_OK;
	}

	// IBufferByteAccess::Buffer
	STDMETHODIMP Buffer(byte** value)
	{
		*value = m_Pointer;
		return S_OK;
	}

	// IBuffer::get_Capacity
	STDMETHODIMP get_Capacity(UINT32* value)
	{
		*value = m_Capacity;
		return S_OK;
	}

	// IBuffer::get_Length
	STDMETHODIMP get_Length(UINT32* value)
	{
		*value = m_Length;
		return S_OK;
	}

	// IBuffer::put_Length
	STDMETHODIMP put_Length(UINT32 value)
	{
		if (value > m_Capacity)
			return E_INVALIDARG;
		m_Length = value;
		return S_OK;
	}
};

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
	UINT32 Capacity)
{
	using Windows::Storage::Streams::IBuffer;

	IBuffer^ buffer = nullptr;

	ComPtr<BufferReference> bufferReference;
	if (SUCCEEDED(MakeAndInitialize<BufferReference>(
		&bufferReference, Pointer, Capacity)))
	{
		buffer = reinterpret_cast<IBuffer^>(bufferReference.Get());
	}

	return buffer;
}

// Converts from the C++/CX string to the UTF-16 string.
// Parameters:
//   PlatformString: The C++/CX string you want to convert.
// Return value:
//   The return value is the UTF-16 string.
std::wstring M2MakeUTF16String(Platform::String^ PlatformString)
{
	return std::wstring(PlatformString->Data(), PlatformString->Length());
}

// Converts from the C++/CX string to the UTF-8 string.
// Parameters:
//   PlatformString: The C++/CX string you want to convert.
// Return value:
//   The return value is the UTF-8 string.
std::string M2MakeUTF8String(Platform::String^ PlatformString)
{
	std::string UTF8String;

	int UTF8StringLength = WideCharToMultiByte(
		CP_UTF8,
		0,
		PlatformString->Data(),
		static_cast<int>(PlatformString->Length()),
		nullptr,
		0,
		nullptr,
		nullptr);
	if (UTF8StringLength > 0)
	{
		UTF8String.resize(UTF8StringLength);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			PlatformString->Data(),
			static_cast<int>(PlatformString->Length()),
			&UTF8String[0],
			UTF8StringLength,
			nullptr,
			nullptr);
	}

	return UTF8String;
}

// Converts from the UTF-8 string to the C++/CX string.
// Parameters:
//   UTF16String: The UTF-16 string you want to convert.
// Return value:
//   The return value is the C++/CX string.
Platform::String^ M2MakeCXString(const std::wstring& UTF16String)
{
	return ref new Platform::String(
		UTF16String.c_str(), static_cast<unsigned int>(UTF16String.size()));
}

#endif // _M2_CX_HELPERS_
