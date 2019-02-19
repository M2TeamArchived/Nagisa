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

/// <summary>
/// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface
/// identifiers.
/// </summary>
/// <returns>
/// The function will return GUID struct.
/// </returns>
GUID M2CreateGuid()
{
    GUID guid = { 0 };
    winrt::check_hresult(CoCreateGuid(&guid));
    return guid;
}

/// <summary>
/// Finds a sub string from a source string.
/// </summary>
/// <param name="SourceString">
/// The source string.
/// </param>
/// <param name="SubString">
/// The sub string.
/// </param>
/// <param name="IgnoreCase">
/// Determines whether to ignore case.
/// </param>
/// <returns>
/// Returns true if successful, or false otherwise.
/// </returns>
bool M2FindSubString(
    winrt::hstring const& SourceString,
    winrt::hstring const& SubString,
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

/// <summary>
/// Converts a numeric value into a string that represents the number expressed
/// as a size value in byte, bytes, kibibytes, mebibytes, gibibytes, tebibytes,
/// pebibytes or exbibytes, depending on the size.
/// </summary>
/// <param name="ByteSize">
/// The numeric byte size value to be converted.
/// </param>
/// <returns>
/// Returns a winrt::hstring object which represents the converted string.
/// </returns>
winrt::hstring M2ConvertByteSizeToString(
    uint64_t ByteSize)
{
    const wchar_t* Systems[] =
    {
        L"Byte",
        L"Bytes",
        L"KiB",
        L"MiB",
        L"GiB",
        L"TiB",
        L"PiB",
        L"EiB"
    };

    size_t nSystem = 0;
    double result = static_cast<double>(ByteSize);

    if (ByteSize > 1)
    {
        for (
            nSystem = 1;
            nSystem < sizeof(Systems) / sizeof(*Systems);
            ++nSystem)
        {
            if (1024.0 > result)
                break;

            result /= 1024.0;
        }

        result = static_cast<uint64_t>(result * 100) / 100.0;
    }

    return winrt::to_hstring(result) + L" " + Systems[nSystem];
}

namespace M2
{
    void NotifyPropertyChangedBase::RaisePropertyChanged(
        winrt::hstring PropertyName)
    {
        this->m_PropertyChanged(
            *this, winrt::PropertyChangedEventArgs(PropertyName));
    }

    winrt::event_token NotifyPropertyChangedBase::PropertyChanged(
        winrt::PropertyChangedEventHandler const& value)
    {
        return this->m_PropertyChanged.add(value);
    }

    void NotifyPropertyChangedBase::PropertyChanged(
        winrt::event_token const& token)
    {
        this->m_PropertyChanged.remove(token);
    }
}

// Execute function on the UI thread with normal priority.
// Parameters:
//   agileCallback: The function you want to execute.
// Return value:
//   The return value is Windows::Foundation::IAsyncAction^.
winrt::IAsyncAction M2ExecuteOnUIThread(
    winrt::DispatchedHandler const& agileCallback)
{
    using winrt::Windows::ApplicationModel::Core::CoreApplication;
    using winrt::Windows::UI::Core::CoreDispatcherPriority;

    return CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
        CoreDispatcherPriority::Normal, agileCallback);
}

#endif // _M2_WINRT_HELPERS_
