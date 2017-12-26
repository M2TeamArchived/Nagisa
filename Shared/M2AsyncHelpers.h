/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the asynchronous helper functions.
File Name: M2AsyncHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_ASYNC_HELPERS_
#define _M2_ASYNC_HELPERS_

#include <Windows.h>
#include <wrl\client.h>

#include "M2RemoveReference.h"
#include "M2Object.h"

// The M2SetAsyncCompletedHandler function sets the completed handler function 
// for asynchronous call.
//
// Parameters:
//
// Async
//     The asynchronous call you want to set.
// Function
//     The completed handler function you want to set.
//
// Return value:
//
// This function does not return a value.
template<typename TAsync, typename... TFunction>
inline void M2SetAsyncCompletedHandler(
	TAsync Async, const TFunction&... Function)
{
	Async->Completed = 
		ref new M2RemoveReference<decltype(Async->Completed)>::Type(
			Function...);
}

// The M2SetAsyncProgressHandler function sets the progress handler function 
// for asynchronous call.
//
// Parameters:
//
// Async
//     The asynchronous call you want to set.
// Function
//     The progress handler function you want to set.
//
// Return value:
//
// This function does not return a value.
template<typename TAsync, typename... TFunction>
inline void M2SetAsyncProgressHandler(
	TAsync Async, const TFunction&... Function)
{
	Async->Progress = 
		ref new M2RemoveReference<decltype(Async->Progress)>::Type(
			Function...);
}

// The M2GetInspectable function retrieves the IInspectable interface from the 
// provided C++/CX object. 
//
// Parameters:
//
// object
//     The C++/CX object you want to retrieve the raw pointer.
//
// Return value:
//
// The return value is the IInspectable interface from the provided C++/CX 
// object.
inline IInspectable* M2GetInspectable(Platform::Object^ object) throw()
{
	return reinterpret_cast<IInspectable*>(object);
}

// The M2AsyncHandleCompleted function handles the completed asynchronous call.
//
// Parameters:
//
// Async
//     The completed asynchronous call you want to handle.
//
// Return value:
//
// The return value is determined by the asynchronous call.
// The function will return error code if the function fails. 
HRESULT M2AsyncHandleCompleted(Platform::Object^ Async);

// The M2AsyncWait function uses the non-blocking way to try to wait 
// asynchronous call.
//
// Parameters:
//
// Async
//     The asynchronous call you want to wait.
// Timeout
//     The maximum time interval for waiting the asynchronous call, in 
//     milliseconds. A value of -1 indicates that the suspension should not 
//     time out.
//
// Return value:
//
// The return value is determined by the asynchronous call.
// The function will throw a COM exception if the function fails. 
template<typename TAsync>
auto M2AsyncWait(
	TAsync Async, int32 Timeout = -1) -> decltype(Async->GetResults())
{
	using M2::CHandle;
	using Platform::COMException;
	using Windows::Foundation::AsyncStatus;
	using Windows::Foundation::IAsyncInfo;

	HRESULT hr = S_OK;

	// Create an event object for wait the asynchronous call.
	CHandle CompletedEvent(
		CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
	if (!CompletedEvent.IsInvalid())
	{
		// Set the completed handler.
		M2SetAsyncCompletedHandler(
			Async, [&](IAsyncInfo^ /*asyncInfo*/, AsyncStatus /*asyncStatus*/)
		{
			// Set event signaled to end wait when completed.
			SetEvent(CompletedEvent);
		});

		// Wait event object signaled.
		WaitForSingleObjectEx(CompletedEvent, Timeout, FALSE);

		// Handle the completed asynchronous call.
		hr = M2AsyncHandleCompleted(Async);
	}
	else
	{
		// Get error code if failed to create an event object.
		hr = __HRESULT_FROM_WIN32(GetLastError());
	}

	// Throw a COM exception if failed.
	if (FAILED(hr))
	{
		throw COMException::CreateException(hr);
	}

	// Return the result of asynchronous call.
	return Async->GetResults();
}

// The M2ExecuteOnUIThread function executes function on the UI thread with 
// normal priority.
//
// Parameters:
//
// agileCallback
//     The function you want to execute.
//
// Return value:
//
// The return value is Windows::Foundation::IAsyncAction^.
Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(
	Windows::UI::Core::DispatchedHandler^ agileCallback);

// The M2ExecuteOnUIThread function executes function on the UI thread with 
// normal priority.
//
// Parameters:
//
// Function
//     The function you want to execute.
//
// Return value:
//
// The return value is Windows::Foundation::IAsyncAction^.
template<typename... TFunction>
inline Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(
	const TFunction&... Function)
{
	return M2ExecuteOnUIThread(
		ref new Windows::UI::Core::DispatchedHandler(Function...));
}

#endif // _M2_ASYNC_HELPERS_
