/******************************************************************************
Project: M2-Team Common Library
Description: Implemention for the asynchronous helper functions.
File Name: M2AsyncHelpers.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include <Windows.h>
#include <windows.foundation.h>

#include "M2AsyncHelpers.h"

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
			else if(AsyncStatus::Started == asyncStatus)
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
	Windows::UI::Core::DispatchedHandler^ agileCallback)
{
	using Windows::ApplicationModel::Core::CoreApplication;
	using Windows::UI::Core::CoreDispatcherPriority;

	return CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal, agileCallback);
}
