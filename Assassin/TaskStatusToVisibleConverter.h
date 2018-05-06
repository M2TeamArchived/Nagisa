/******************************************************************************
Project: Assassin
Description: Definition for the TaskStatusToVisibleConverter.
File Name: TaskStatusToVisibleConverter.h
License: The MIT License
******************************************************************************/

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.UI.Xaml.Interop.h>

#include "TaskStatusToVisibleConverter.g.h"

namespace winrt
{
	using Windows::Foundation::IInspectable;
	using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::Assassin::implementation
{
    struct TaskStatusToVisibleConverter : 
		TaskStatusToVisibleConverterT<TaskStatusToVisibleConverter>
    {
		TaskStatusToVisibleConverter() = default;

		winrt::IInspectable Convert(
			winrt::IInspectable const& value,
			winrt::TypeName const& targetType,
			winrt::IInspectable const& parameter, 
			winrt::hstring const& language) const;

		winrt::IInspectable ConvertBack(
			winrt::IInspectable const& value, 
			winrt::TypeName const& targetType,
			winrt::IInspectable const& parameter,
			winrt::hstring const& language) const;
    };
}

namespace winrt::Assassin::factory_implementation
{
    struct TaskStatusToVisibleConverter : TaskStatusToVisibleConverterT<
		TaskStatusToVisibleConverter,
		implementation::TaskStatusToVisibleConverter>
    {
    };
}
