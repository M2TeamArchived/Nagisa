/*
 * PROJECT:   Nagisa
 * FILE:      CustomConverter.cpp
 * PURPOSE:   Implementation for the CustomConverter.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"

#include "CustomConverter.h"

#include <winrt\Assassin.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt::Nagisa::implementation
{
    using Assassin::ITransferTask;
    using Assassin::TransferTaskStatus;
    using Windows::Foundation::Collections::IVectorView;
    using Windows::UI::Xaml::Visibility;

    IInspectable ConvertBooleanToNullableBoolean(
        IInspectable const& value)
    {
        return box_value(unbox_value_or<bool>(value, false));
    }

    IInspectable ConvertUint64ToDouble(
        IInspectable const& value)
    {
        return box_value(
            static_cast<double>(unbox_value_or<uint64_t>(value, 0)));
    }

    IInspectable ConvertUint64ByteSizeToString(
        IInspectable const& value)
    {
        return box_value(
            M2ConvertByteSizeToString(unbox_value_or<uint64_t>(value, 0)));
    }

    IInspectable ConvertUint64RemainTimeToString(
        IInspectable const& value)
    {
        uint64_t Seconds = unbox_value_or<uint64_t>(
            value, static_cast<uint64_t>(-1));

        if (static_cast<uint64_t>(-1) != Seconds)
        {
            int Hour = static_cast<int>(Seconds / 3600);
            int Minute = static_cast<int>(Seconds / 60 % 60);
            int Second = static_cast<int>(Seconds % 60);

            return box_value(
                M2FormatString(L"%d:%02d:%02d", Hour, Minute, Second));
        }

        return box_value(L"N/A");
    }

    IInspectable ConvertTaskStatusToProgressBarForegroundColor(
        IInspectable const& value)
    {
        using Windows::UI::Colors;
        using Windows::UI::Xaml::Media::SolidColorBrush;

        SolidColorBrush ProgressBarForegroundColorBrush;

        ProgressBarForegroundColorBrush.Opacity(0.8);

        switch (unbox_value_or<TransferTaskStatus>(
            value, TransferTaskStatus::Error))
        {
        case TransferTaskStatus::Running:
            ProgressBarForegroundColorBrush.Color(Colors::Green());
            break;
        case TransferTaskStatus::Paused:
            ProgressBarForegroundColorBrush.Color(Colors::Orange());
            break;
        case TransferTaskStatus::Error:
            ProgressBarForegroundColorBrush.Color(Colors::Red());
            break;
        default:
            ProgressBarForegroundColorBrush.Color(Colors::Transparent());
            break;
        }

        return ProgressBarForegroundColorBrush;
    }

    IInspectable ConvertTaskStatusToRetryButtonVisible(
        IInspectable const& value)
    {
        Visibility RetryButtonVisible = Visibility::Collapsed;

        switch (unbox_value_or<TransferTaskStatus>(
            value, TransferTaskStatus::Error))
        {
        case TransferTaskStatus::Canceled:
        case TransferTaskStatus::Error:
            RetryButtonVisible = Visibility::Visible;
            break;
        default:
            break;
        }

        return box_value(RetryButtonVisible);
    }

    IInspectable ConvertTaskStatusToResumeButtonVisible(
        IInspectable const& value)
    {
        TransferTaskStatus Status =
            unbox_value_or<TransferTaskStatus>(
                value, TransferTaskStatus::Error);

        return box_value(
            (TransferTaskStatus::Paused == Status)
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    IInspectable ConvertTaskStatusToPauseButtonVisible(
        IInspectable const& value)
    {
        TransferTaskStatus Status =
            unbox_value_or<TransferTaskStatus>(
                value, TransferTaskStatus::Error);

        return box_value(
            (TransferTaskStatus::Running == Status)
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    IInspectable ConvertTaskStatusToRunningPausedVisible(
        IInspectable const& value)
    {
        Visibility RetryButtonVisible = Visibility::Collapsed;

        switch (unbox_value_or<TransferTaskStatus>(
            value, TransferTaskStatus::Error))
        {
        case TransferTaskStatus::Paused:
        case TransferTaskStatus::Running:
            RetryButtonVisible = Visibility::Visible;
            break;
        default:
            break;
        }

        return box_value(RetryButtonVisible);
    }

    IInspectable CustomConverter::Convert(
        IInspectable const& value,
        TypeName const& targetType,
        IInspectable const& parameter,
        hstring const& language) const
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(language);

        hstring ParameterObject =
            unbox_value_or<hstring>(parameter, L"");
        const wchar_t* Parameter = ParameterObject.data();

        if (0 == wcscmp(Parameter, L"Uint64ToDouble"))
        {
            return ConvertUint64ToDouble(value);
        }

        if (0 == wcscmp(Parameter, L"Uint64ByteSizeToString"))
        {
            return ConvertUint64ByteSizeToString(value);
        }

        if (0 == wcscmp(Parameter, L"Uint64RemainTimeToString"))
        {
            return ConvertUint64RemainTimeToString(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToProgressBarForegroundColor"))
        {
            return ConvertTaskStatusToProgressBarForegroundColor(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToRetryButtonVisible"))
        {
            return ConvertTaskStatusToRetryButtonVisible(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToResumeButtonVisible"))
        {
            return ConvertTaskStatusToResumeButtonVisible(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToPauseButtonVisible"))
        {
            return ConvertTaskStatusToPauseButtonVisible(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToRunningPausedVisible"))
        {
            return ConvertTaskStatusToRunningPausedVisible(value);
        }

        throw_hresult(E_NOTIMPL);
    }

    IInspectable CustomConverter::ConvertBack(
        IInspectable const& value,
        TypeName const& targetType,
        IInspectable const& parameter,
        hstring const& language) const
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(language);

        hstring ParameterObject =
            unbox_value_or<hstring>(parameter, L"");
        const wchar_t* Parameter = ParameterObject.data();

        if (0 == wcscmp(Parameter, L"NullableBooleanToBoolean"))
        {
            return ConvertBooleanToNullableBoolean(value);
        }

        throw_hresult(E_NOTIMPL);
    }
}
