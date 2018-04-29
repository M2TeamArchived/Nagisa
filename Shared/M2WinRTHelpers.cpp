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

#include <robuffer.h>

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception.
__declspec(noreturn) void M2ThrowPlatformException(HRESULT hr)
{
	throw winrt::hresult_error(hr);
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
	catch (winrt::hresult_error const & ex)
	{
		return ex.code();
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

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
GUID M2CreateGuid()
{
	GUID guid = { 0 };
	M2ThrowPlatformExceptionIfFailed(CoCreateGuid(&guid));
	return guid;
}

// Execute function on the UI thread with normal priority.
// Parameters:
//   agileCallback: The function you want to execute.
// Return value:
//   The return value is Windows::Foundation::IAsyncAction.
winrt::IAsyncAction M2ExecuteOnUIThread(
	winrt::DispatchedHandler agileCallback)
{
	return winrt::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
		winrt::CoreDispatcherPriority::Normal, agileCallback);
}

struct BufferReference : winrt::implements<
	BufferReference, 
	winrt::IBuffer, 
	Windows::Storage::Streams::IBufferByteAccess>
{
private:
	uint32_t m_Capacity;
	uint32_t m_Length;
	uint8_t* m_Pointer;

public:
	
	BufferReference(uint8_t* Pointer, uint32_t Capacity) :
		m_Capacity(Capacity),
		m_Length(Capacity),
		m_Pointer(Pointer)
	{

	}

	// IBuffer::get_Capacity
	uint32_t Capacity() const noexcept
	{
		return m_Capacity;
	}

	// IBuffer::get_Length
	uint32_t Length() const noexcept
	{
		return m_Length;
	}

	// IBuffer::put_Length
	void Length(uint32_t value)
	{
		if (value > m_Capacity)
		{
			throw winrt::hresult_invalid_argument();
		}

		m_Length = value;
	}

	// IBufferByteAccess::Buffer
	STDMETHODIMP Buffer(uint8_t** value) noexcept final
	{
		*value = m_Pointer;
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
winrt::IBuffer M2MakeIBuffer(
	uint8_t* Pointer,
	uint32_t Capacity)
{
	return winrt::make<BufferReference>(Pointer, Capacity);
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
uint8_t* M2GetPointer(winrt::IBuffer Buffer)
{
	uint8_t* pBuffer = nullptr;

	Buffer.try_as<Windows::Storage::Streams::IBufferByteAccess>()->Buffer(
		&pBuffer);

	return pBuffer;
}

// Converts a numeric value into a string that represents the number expressed 
// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
// pebibytes or exbibytes, depending on the size.
// Parameters:
//   ByteSize: The numeric byte size value to be converted.
// Return value:
//   Returns a winrt::hstring object which represents the converted string.
winrt::hstring M2ConvertByteSizeToString(uint64 ByteSize)
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

	winrt::hstring ByteSizeString = winrt::to_hstring(
		static_cast<uint64>(result * 100) / 100.0);

	return ByteSizeString + Systems[nSystem];
}





#include <windows.foundation.h>

#include <string>

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

#endif // _M2_WINRT_HELPERS_
