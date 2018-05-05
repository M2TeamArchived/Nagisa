/******************************************************************************
Project: Assassin
Description: Implementation for TaskListEmptyToVisibilityConverter.
File Name: TaskListEmptyToVisibilityConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "TaskListEmptyToVisibilityConverter.h"

using namespace winrt::Assassin::implementation;

#include <winrt\Assassin.h>

namespace winrt
{
	using Assassin::ITransferTask;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::UI::Xaml::Visibility;
}

winrt::IInspectable TaskListEmptyToVisibilityConverter::Convert(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);

	winrt::IVectorView<winrt::ITransferTask> ItemSource = 
		winrt::unbox_value_or<winrt::IVectorView<winrt::ITransferTask>>(
			value, winrt::IVectorView<winrt::ITransferTask>(nullptr));

	return winrt::box_value(
		(nullptr == ItemSource || 0 == ItemSource.Size())
		? winrt::Visibility::Visible
		: winrt::Visibility::Collapsed);
}

winrt::IInspectable TaskListEmptyToVisibilityConverter::ConvertBack(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(value);
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(parameter);
	UNREFERENCED_PARAMETER(language);
	
	winrt::throw_hresult(E_NOTIMPL);
}
