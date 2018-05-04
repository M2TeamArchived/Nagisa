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

template <typename T>
struct single_threaded_observable_vector : winrt::implements<single_threaded_observable_vector<T>,
	winrt::Windows::Foundation::Collections::IObservableVector<T>,
	winrt::Windows::Foundation::Collections::IVector<T>,
	winrt::Windows::Foundation::Collections::IVectorView<T>,
	winrt::Windows::Foundation::Collections::IIterable<T>>
{
	winrt::event_token VectorChanged(winrt::Windows::Foundation::Collections::VectorChangedEventHandler<T> const& handler)
	{
		return m_changed.add(handler);
	}

	void VectorChanged(winrt::event_token const cookie)
	{
		m_changed.remove(cookie);
	}

	T GetAt(uint32_t const index) const
	{
		if (index >= m_values.size())
		{
			throw winrt::hresult_out_of_bounds();
		}

		return m_values[index];
	}

	uint32_t Size() const noexcept
	{
		return static_cast<uint32_t>(m_values.size());
	}

	winrt::Windows::Foundation::Collections::IVectorView<T> GetView()
	{
		return *this;
	}

	bool IndexOf(T const& value, uint32_t& index) const noexcept
	{
		index = static_cast<uint32_t>(std::find(m_values.begin(), m_values.end(), value) - m_values.begin());
		return index < m_values.size();
	}

	void SetAt(uint32_t const index, T const& value)
	{
		if (index >= m_values.size())
		{
			throw winrt::hresult_out_of_bounds();
		}

		++m_version;
		m_values[index] = value;
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::ItemChanged, index));
	}

	void InsertAt(uint32_t const index, T const& value)
	{
		if (index > m_values.size())
		{
			throw winrt::hresult_out_of_bounds();
		}

		++m_version;
		m_values.insert(m_values.begin() + index, value);
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted, index));
	}

	void RemoveAt(uint32_t const index)
	{
		if (index >= m_values.size())
		{
			throw winrt::hresult_out_of_bounds();
		}

		++m_version;
		m_values.erase(m_values.begin() + index);
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::ItemRemoved, index));
	}

	void Append(T const& value)
	{
		++m_version;
		m_values.push_back(value);
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted, Size() - 1));
	}

	void RemoveAtEnd()
	{
		if (m_values.empty())
		{
			throw winrt::hresult_out_of_bounds();
		}

		++m_version;
		m_values.pop_back();
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::ItemRemoved, Size()));
	}

	void Clear() noexcept
	{
		++m_version;
		m_values.clear();
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::Reset, 0));
	}

	uint32_t GetMany(uint32_t const startIndex, winrt::array_view<T> values) const
	{
		if (startIndex >= m_values.size())
		{
			return 0;
		}

		uint32_t actual = static_cast<uint32_t>(m_values.size() - startIndex);

		if (actual > values.size())
		{
			actual = values.size();
		}

		std::copy_n(m_values.begin() + startIndex, actual, values.begin());
		return actual;
	}

	void ReplaceAll(winrt::array_view<T const> value)
	{
		++m_version;
		m_values.assign(value.begin(), value.end());
		m_changed(*this, winrt::make<args>(winrt::Windows::Foundation::Collections::CollectionChange::Reset, 0));
	}

	winrt::Windows::Foundation::Collections::IIterator<T> First()
	{
		return winrt::make<iterator>(this);
	}

private:

	std::vector<T> m_values;
	winrt::event<winrt::Windows::Foundation::Collections::VectorChangedEventHandler<T>> m_changed;
	uint32_t m_version{};

	struct args : winrt::implements<args, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs>
	{
		args(winrt::Windows::Foundation::Collections::CollectionChange const change, uint32_t const index) :
			m_change(change),
			m_index(index)
		{
		}

		winrt::Windows::Foundation::Collections::CollectionChange CollectionChange() const
		{
			return m_change;
		}

		uint32_t Index() const
		{
			return m_index;
		}

	private:

		winrt::Windows::Foundation::Collections::CollectionChange const m_change{};
		uint32_t const m_index{};
	};

	struct iterator : winrt::implements<iterator, winrt::Windows::Foundation::Collections::IIterator<T>>
	{
		explicit iterator(single_threaded_observable_vector<T>* owner) noexcept :
		m_version(owner->m_version),
			m_current(owner->m_values.begin()),
			m_end(owner->m_values.end())
		{
			m_owner.copy_from(owner);
		}

		void abi_enter() const
		{
			if (m_version != m_owner->m_version)
			{
				throw winrt::hresult_changed_state();
			}
		}

		T Current() const
		{
			if (m_current == m_end)
			{
				throw winrt::hresult_out_of_bounds();
			}

			return *m_current;
		}

		bool HasCurrent() const noexcept
		{
			return m_current != m_end;
		}

		bool MoveNext() noexcept
		{
			if (m_current != m_end)
			{
				++m_current;
			}

			return HasCurrent();
		}

		uint32_t GetMany(winrt::array_view<T> values)
		{
			uint32_t actual = static_cast<uint32_t>(std::distance(m_current, m_end));

			if (actual > values.size())
			{
				actual = values.size();
			}

			std::copy_n(m_current, actual, values.begin());
			std::advance(m_current, actual);
			return actual;
		}

	private:

		winrt::com_ptr<single_threaded_observable_vector<T>> m_owner;
		uint32_t const m_version;
		typename std::vector<T>::const_iterator m_current;
		typename std::vector<T>::const_iterator const m_end;
	};
};
	

#include "OpenSSL\crypto.h"
#include "OpenSSL\bio.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
