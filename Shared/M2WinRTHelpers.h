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

#include <winrt\Windows.ApplicationModel.Core.h>
#include <winrt\Windows.UI.Core.h>
#include <winrt\Windows.UI.Xaml.Data.h>

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace winrt
{
    using Windows::Foundation::IAsyncAction;
    using Windows::UI::Core::DispatchedHandler;
    using Windows::UI::Xaml::Data::INotifyPropertyChanged;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
}

/// <summary>
/// Creates a GUID, a unique 128-bit integer used for CLSIDs and interface
/// identifiers.
/// </summary>
/// <returns>
/// The function will return GUID struct.
/// </returns>
GUID M2CreateGuid();

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
    bool IgnoreCase);

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
    uint64_t ByteSize);

namespace M2
{
    struct NotifyPropertyChangedBase : winrt::implements<
        NotifyPropertyChangedBase, winrt::INotifyPropertyChanged>
    {
    private:
        winrt::event<winrt::PropertyChangedEventHandler> m_PropertyChanged;

    protected:
        void RaisePropertyChanged(
            std::wstring_view const& PropertyName);

    public:
        NotifyPropertyChangedBase() = default;

        winrt::event_token PropertyChanged(
            winrt::PropertyChangedEventHandler const& value);

        void PropertyChanged(
            winrt::event_token const& token);
    };
}

// Execute function on the UI thread with normal priority.
// Parameters:
//   agileCallback: The function you want to execute.
// Return value:
//   The return value is Windows::Foundation::IAsyncAction^.
winrt::IAsyncAction M2ExecuteOnUIThread(
    winrt::DispatchedHandler const& agileCallback);

#endif // _M2_WINRT_HELPERS_
