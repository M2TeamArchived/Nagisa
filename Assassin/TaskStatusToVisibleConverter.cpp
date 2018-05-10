/******************************************************************************
Project: Assassin
Description: Implementation for TaskStatusToVisibleConverter.
File Name: TaskStatusToVisibleConverter.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"

#include "TaskStatusToVisibleConverter.h"

using namespace winrt::Assassin::implementation;

#include <winrt\Assassin.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt
{
	using Assassin::TransferTaskStatus;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::UI::Xaml::Visibility;
}

winrt::IInspectable TaskStatusToVisibleConverter::Convert(
	winrt::IInspectable const& value,
	winrt::TypeName const& targetType,
	winrt::IInspectable const& parameter,
	winrt::hstring const& language) const
{
	UNREFERENCED_PARAMETER(targetType);
	UNREFERENCED_PARAMETER(language);

	winrt::TransferTaskStatus Status = 
		winrt::unbox_value_or<winrt::TransferTaskStatus>(
			value, winrt::TransferTaskStatus::Error);
	winrt::hstring ParameterObject = 
		winrt::unbox_value_or<winrt::hstring>(parameter, L"");
	
	struct { const wchar_t* Key; winrt::TransferTaskStatus Value; } List[] =
	{
		{ L"Canceled", winrt::TransferTaskStatus::Canceled },
		{ L"Completed", winrt::TransferTaskStatus::Completed },
		{ L"Error", winrt::TransferTaskStatus::Error },
		{ L"Paused", winrt::TransferTaskStatus::Paused },
		{ L"Queued", winrt::TransferTaskStatus::Queued },
		{ L"Running", winrt::TransferTaskStatus::Running }
	};

	const wchar_t* Parameter = ParameterObject.data();

	for (size_t i = 0; i < sizeof(List) / sizeof(*List); ++i)
	{
		if (0 == _wcsicmp(Parameter, List[i].Key))
		{
			return winrt::box_value(
				(List[i].Value == Status)
				? winrt::Visibility::Visible
				: winrt::Visibility::Collapsed);
		}
	}

	winrt::throw_hresult(E_INVALIDARG);
}

winrt::IInspectable TaskStatusToVisibleConverter::ConvertBack(
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
