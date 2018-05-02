#pragma once

#include "NagisaVersion.h"

#include <winrt\base.h>

#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Networking.Sockets.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>
#include <winrt\Windows.UI.Xaml.Data.h>

#include <map>
#include <string>
#include <vector>

#include <ppltasks.h>

#include "M2BaseHelpers.h"
//#include "M2WinRTHelpers.h"

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception.
__declspec(noreturn) inline void M2ThrowPlatformException(HRESULT hr)
{
	throw winrt::hresult_error(hr);
}

// Throw the appropriate Platform::Exception for the given HRESULT.
// Parameters:
//   hr: The error HRESULT that is represented by the exception. 
// Return value:
//   This function does not return a value, but will throw Platform::Exception
//   if it is a failed HRESULT value.
void inline M2ThrowPlatformExceptionIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		M2ThrowPlatformException(hr);
	}
}

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
inline GUID M2CreateGuid()
{
	GUID guid = { 0 };
	M2ThrowPlatformExceptionIfFailed(CoCreateGuid(&guid));
	return guid;
}

// Finds a sub string from a source string. 
// Parameters:
//   SourceString: The source string.
//   SubString: The sub string.
//   IgnoreCase: Determines whether to ignore case.
// Return value:
//   Returns true if successful, or false otherwise.
inline bool M2FindSubString(
	winrt::hstring SourceString,
	winrt::hstring SubString,
	bool IgnoreCase)
{
	return (::FindNLSStringEx(
		nullptr,
		(IgnoreCase ? NORM_IGNORECASE : 0) | FIND_FROMSTART,
		SourceString.c_str(),
		SourceString.size(),
		SubString.c_str(),
		SubString.size(),
		nullptr,
		nullptr,
		nullptr,
		0) >= 0);
}

// Try to wait asynchronous call.
// Parameters:
//   Async: The asynchronous call you want to wait.
//   Timeout: The maximum time interval for waiting the asynchronous call, in 
//   milliseconds. A value of -1 indicates that the suspension should not time 
//   out.
// Return value:
//   The return value is determined by the asynchronous call.
//   The function will throw a COM exception if the function fails. 
template<typename TAsync>
auto M2AsyncWait(
	TAsync Async, uint32_t Timeout = -1) -> decltype(Async.GetResults())
{
	using M2::CHandle;
	using winrt::Windows::Foundation::AsyncStatus;

	HRESULT hr = S_OK;

	// Create an event object for wait the asynchronous call.
	CHandle CompletedEvent(
		CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
	if (!CompletedEvent.IsInvalid())
	{
		// Set the completed handler.
		Async.Completed([&](TAsync const& asyncInfo, AsyncStatus asyncStatus)
		{
			UNREFERENCED_PARAMETER(asyncInfo);
			UNREFERENCED_PARAMETER(asyncStatus);
			
			// Set event signaled to end wait when completed.
			SetEvent(CompletedEvent);
		});

		// Wait event object signaled.
		WaitForSingleObjectEx(CompletedEvent, Timeout, FALSE);

		// Handle the completed asynchronous call.
		try
		{
			AsyncStatus asyncStatus = Async.Status();
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
				Async.Cancel();
				hr = __HRESULT_FROM_WIN32(ERROR_TIMEOUT);
			}
			else if (AsyncStatus::Canceled == asyncStatus)
			{
				// If the status is Cancelled, return the error code.
				hr = E_ABORT;
			}
			else
			{
				// If the status is other value, return the error code.	
				hr = Async.ErrorCode();
			}
		}
		catch (winrt::hresult_error const& ex)
		{
			hr = ex.code();
		}		
	}
	else
	{
		// Get error code if failed to create an event object.
		hr = M2GetLastError();
	}

	// Throw a COM exception if failed.
	M2ThrowPlatformExceptionIfFailed(hr);

	// Return the result of asynchronous call.
	return Async.GetResults();
}

#include "OpenSSL\crypto.h"
#include "OpenSSL\bio.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
