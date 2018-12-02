/******************************************************************************
Project: Assassin
Description: Implementation for CustomConverter.
File Name: CustomConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "CustomConverter.h"

using namespace winrt::Assassin::implementation;

#include <winrt\Assassin.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt
{
    using Assassin::ITransferTask;
    using Assassin::TransferTaskStatus;
    using Windows::Foundation::Collections::IVectorView;
    using Windows::UI::Xaml::Visibility;
}

winrt::IInspectable ConvertNullableBooleanToBoolean(
    winrt::IInspectable const& value)
{
    return winrt::box_value(winrt::unbox_value_or<bool>(value, false));
}

winrt::IInspectable ConvertBooleanToNullableBoolean(
    winrt::IInspectable const& value)
{
    return winrt::box_value(winrt::unbox_value_or<bool>(value, false));
}

winrt::IInspectable ConvertUint64ToDouble(
    winrt::IInspectable const& value)
{
    return winrt::box_value(
        static_cast<double>(winrt::unbox_value_or<uint64_t>(value, 0)));
}

winrt::IInspectable ConvertTaskListEmptyToVisibility(
    winrt::IInspectable const& value)
{
    winrt::IVectorView<winrt::ITransferTask> ItemSource =
        winrt::unbox_value_or<winrt::IVectorView<winrt::ITransferTask>>(
            value, winrt::IVectorView<winrt::ITransferTask>(nullptr));

    return winrt::box_value(
        (nullptr == ItemSource || 0 == ItemSource.Size())
        ? winrt::Visibility::Visible
        : winrt::Visibility::Collapsed);
}

winrt::IInspectable ConvertUint64ByteSizeToString(
    winrt::IInspectable const& value)
{
    return winrt::box_value(
        M2ConvertByteSizeToString(winrt::unbox_value_or<uint64_t>(value, 0)));
}

winrt::IInspectable ConvertUint64RemainTimeToString(
    winrt::IInspectable const& value)
{
    uint64_t Seconds = winrt::unbox_value_or<uint64_t>(
        value, static_cast<uint64_t>(-1));

    if (static_cast<uint64_t>(-1) != Seconds)
    {
        int Hour = static_cast<int>(Seconds / 3600);
        int Minute = static_cast<int>(Seconds / 60 % 60);
        int Second = static_cast<int>(Seconds % 60);

        return winrt::box_value(
            M2FormatString(L"%d:%02d:%02d", Hour, Minute, Second));
    }

    return winrt::box_value(L"N/A");
}

template<winrt::TransferTaskStatus TaskStatus>
winrt::IInspectable ConvertTaskStatusToVisible(
    winrt::IInspectable const& value)
{
    winrt::TransferTaskStatus Status =
        winrt::unbox_value_or<winrt::TransferTaskStatus>(
            value, winrt::TransferTaskStatus::Error);

    return winrt::box_value(
        (TaskStatus == Status)
        ? winrt::Visibility::Visible
        : winrt::Visibility::Collapsed);
}

winrt::IInspectable CustomConverter::Convert(
    winrt::IInspectable const& value,
    winrt::TypeName const& targetType,
    winrt::IInspectable const& parameter,
    winrt::hstring const& language) const
{
    UNREFERENCED_PARAMETER(targetType);
    UNREFERENCED_PARAMETER(language);

    winrt::hstring ParameterObject =
        winrt::unbox_value_or<winrt::hstring>(parameter, L"");
    const wchar_t* Parameter = ParameterObject.data();

    if (0 == wcscmp(Parameter, L"NullableBooleanToBoolean"))
    {
        return ConvertNullableBooleanToBoolean(value);
    }

    if (0 == wcscmp(Parameter, L"Uint64ToDouble"))
    {
        return ConvertUint64ToDouble(value);
    }

    if (0 == wcscmp(Parameter, L"TaskListEmptyToVisibility"))
    {
        return ConvertTaskListEmptyToVisibility(value);
    }

    if (0 == wcscmp(Parameter, L"Uint64ByteSizeToString"))
    {
        return ConvertUint64ByteSizeToString(value);
    }

    if (0 == wcscmp(Parameter, L"Uint64RemainTimeToString"))
    {
        return ConvertUint64RemainTimeToString(value);
    }

    if (0 == wcscmp(Parameter, L"TaskCanceledStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Canceled>(value);
    }

    if (0 == wcscmp(Parameter, L"TaskCompletedStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Completed>(value);
    }

    if (0 == wcscmp(Parameter, L"TaskErrorStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Error>(value);
    }

    if (0 == wcscmp(Parameter, L"TaskPausedStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Paused>(value);
    }

    if (0 == wcscmp(Parameter, L"TaskQueuedStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Queued>(value);
    }

    if (0 == wcscmp(Parameter, L"TaskRunningStatusToVisible"))
    {
        return ConvertTaskStatusToVisible<
            winrt::TransferTaskStatus::Running>(value);
    }

    winrt::throw_hresult(E_NOTIMPL);
}

winrt::IInspectable CustomConverter::ConvertBack(
    winrt::IInspectable const& value,
    winrt::TypeName const& targetType,
    winrt::IInspectable const& parameter,
    winrt::hstring const& language) const
{
    UNREFERENCED_PARAMETER(targetType);
    UNREFERENCED_PARAMETER(language);

    winrt::hstring ParameterObject =
        winrt::unbox_value_or<winrt::hstring>(parameter, L"");
    const wchar_t* Parameter = ParameterObject.data();

    if (0 == wcscmp(Parameter, L"NullableBooleanToBoolean"))
    {
        return ConvertBooleanToNullableBoolean(value);
    }

    winrt::throw_hresult(E_NOTIMPL);
}
