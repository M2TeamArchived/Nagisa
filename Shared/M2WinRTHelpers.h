/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the C++/WinRT helper functions.
File Name: M2WinRTHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_WINRT_HELPERS_
#define _M2_WINRT_HELPERS_

#include <Windows.h>
#include "M2BaseHelpers.h"

#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include <map>
#include <string>
#include <vector>

// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface 
// identifiers. 
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function will return GUID struct.
GUID M2CreateGuid();

// Finds a sub string from a source string. 
// Parameters:
//   SourceString: The source string.
//   SubString: The sub string.
//   IgnoreCase: Determines whether to ignore case.
// Return value:
//   Returns true if successful, or false otherwise.
bool M2FindSubString(
	winrt::hstring SourceString,
	winrt::hstring SubString,
	bool IgnoreCase);

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

		bool IndexOf(
			winrt::Windows::Foundation::IInspectable const& value, 
			uint32_t& index) const
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
winrt::hstring M2ConvertByteSizeToString(uint64_t ByteSize);

#endif // _M2_WINRT_HELPERS_
