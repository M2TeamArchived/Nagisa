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
#include <wrl\event.h>
#include "M2RemoveReference.h"

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
template<typename TAsync, typename TFunction>
void M2SetAsyncCompletedHandler(TAsync Async, TFunction Function)
{
	Async->Completed =
		ref new M2RemoveReference<decltype(Async->Completed)>::Type(Function);
}

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
inline auto M2AsyncWait(
	TAsync Async, int32 Timeout = -1) -> decltype(Async->GetResults())
{
	using Microsoft::WRL::Wrappers::Event;
	using Platform::COMException;
	using Windows::Foundation::AsyncStatus;
	using Windows::Foundation::IAsyncInfo;

	HRESULT hr = S_OK;

	// Create an event object for wait the asynchronous call.
	Event CompletedEvent(
		CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
	if (CompletedEvent.IsValid())
	{
		// Set event signaled to end wait when completed.
		M2SetAsyncCompletedHandler(
			Async, [&](IAsyncInfo^ asyncInfo, AsyncStatus asyncStatus)
		{
			UNREFERENCED_PARAMETER(asyncInfo);
			UNREFERENCED_PARAMETER(asyncStatus);

			SetEvent(CompletedEvent.Get());
		});

		// Wait event object signaled.
		WaitForSingleObjectEx(CompletedEvent.Get(), Timeout, FALSE);

		switch (Async->Status)
		{
		case AsyncStatus::Completed:
			// Just do nothing if succeeded.
			break;
		case AsyncStatus::Started:
			// Cancel the asynchronous call and set error code if the 
			// status is still Started, the timeout interval has been 
			// elapsed.
			Async->Cancel();
			hr = __HRESULT_FROM_WIN32(ERROR_TIMEOUT);
			break;
		case AsyncStatus::Canceled:
			// If the status is Cancelled, set the error code.
			hr = E_ABORT;
			break;
		default:
			// If the status is other value, get and set the error code.
			hr = Async->ErrorCode.Value;
			break;
		}
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
template<typename TAsync, typename TFunction>
void M2SetAsyncProgressHandler(TAsync Async, TFunction Function)
{
	Async->Progress =
		ref new M2RemoveReference<decltype(Async->Progress)>::Type(Function);
}

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
template<typename TFunction>
Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(TFunction Function)
{
	using Windows::ApplicationModel::Core::CoreApplication;
	using Windows::UI::Core::DispatchedHandler;
	using Windows::UI::Core::CoreDispatcherPriority;

	return CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler(Function));
}

#endif // _M2_ASYNC_HELPERS_
