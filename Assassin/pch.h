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
#include <winrt\Windows.UI.Xaml.Interop.h>

#include <map>
#include <string>
#include <vector>

//#include <ppltasks.h>

#include "M2BaseHelpers.h"
//#include "M2WinRTHelpers.h"

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
inline GUID M2CreateGuid()
{
	GUID guid = { 0 };
	winrt::check_hresult(CoCreateGuid(&guid));
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
	winrt::check_hresult(hr);

	// Return the result of asynchronous call.
	return Async.GetResults();
}

namespace M2
{
	template <typename Type>
	struct BindableVectorView : winrt::implements<
		BindableVectorView<Type>,
		winrt::Windows::Foundation::Collections::IVectorView<Type>,
		winrt::Windows::Foundation::Collections::IIterable<Type>,
		winrt::Windows::UI::Xaml::Interop::IBindableVectorView,
		winrt::Windows::UI::Xaml::Interop::IBindableIterable>
	{
	private:
		std::vector<Type> m_values;

		class WinRTObject
		{
		private:
			Type m_value;

		public:
			WinRTObject(Type value) :
				m_value(value)
			{

			}

			operator Type()
			{
				return this->m_value;
			}

			operator winrt::Windows::Foundation::IInspectable()
			{
				return winrt::box_value(this->m_value);
			}
		};

		struct iterator : winrt::implements<
			iterator,
			winrt::Windows::Foundation::Collections::IIterator<Type>,
			winrt::Windows::UI::Xaml::Interop::IBindableIterator>
		{
		private:
			winrt::com_ptr<BindableVectorView<Type>> m_owner;
			typename std::vector<Type>::const_iterator m_current;
			typename std::vector<Type>::const_iterator const m_end;

		public:	
			explicit iterator(BindableVectorView<Type>* owner) noexcept :
				m_current(owner->m_values.begin()),
				m_end(owner->m_values.end())
			{
				m_owner.copy_from(owner);
			}

			WinRTObject Current() const
			{
				if (m_current == m_end)
				{
					throw winrt::hresult_out_of_bounds();
				}

				return WinRTObject(*m_current);
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

			uint32_t GetMany(winrt::array_view<Type> values)
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
		};

	public:
		explicit BindableVectorView(std::vector<Type>& values) :
			m_values(values)
		{
		}

		uint32_t Size() const noexcept
		{
			return static_cast<uint32_t>(m_values.size());
		}

		WinRTObject GetAt(uint32_t const index) const
		{
			if (index >= m_values.size())
			{
				throw winrt::hresult_out_of_bounds();
			}

			return WinRTObject(m_values[index]);
		}

		uint32_t GetMany(uint32_t const startIndex, winrt::array_view<Type> values) const
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

		bool IndexOf(Type const& value, uint32_t& index) const noexcept
		{
			index = static_cast<uint32_t>(std::find(m_values.begin(), m_values.end(), value) - m_values.begin());
			return index < m_values.size();
		}


		class IIteratorCreator
		{
		private:
			BindableVectorView<Type>* m_owner;

		public:
			IIteratorCreator(BindableVectorView<Type>* owner) :
				m_owner(owner)
			{

			}

			operator winrt::Windows::Foundation::Collections::IIterator<Type>()
			{
				return winrt::make<iterator>(this->m_owner);
			}

			operator winrt::Windows::UI::Xaml::Interop::IBindableIterator()
			{
				return winrt::make<iterator>(this->m_owner).try_as<winrt::Windows::UI::Xaml::Interop::IBindableIterator>();
			}
		};

		IIteratorCreator First()
		{
			return IIteratorCreator(this);
		}

		bool IndexOf(winrt::Windows::Foundation::IInspectable const& value, uint32_t& index) const
		{
			return this->IndexOf(winrt::unbox_value<Type>(value), index);
		}
	};
}

// Converts a numeric value into a string that represents the number expressed 
// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
// pebibytes or exbibytes, depending on the size.
// Parameters:
//   ByteSize: The numeric byte size value to be converted.
// Return value:
//   Returns a winrt::hstring object which represents the converted string.
inline winrt::hstring M2ConvertByteSizeToString(uint64_t ByteSize)
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
		static_cast<uint64_t>(result * 100) / 100.0);

	return ByteSizeString + L" " + Systems[nSystem];
}

#include "OpenSSL\crypto.h"
#include "OpenSSL\bio.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
