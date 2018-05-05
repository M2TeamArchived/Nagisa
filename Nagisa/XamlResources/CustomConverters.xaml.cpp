//
// CustomConverters.xaml.cpp
// CustomConverters 类的实现
//

#include "pch.h"
#include "CustomConverters.xaml.h"

#include <map>
#include <string>

using namespace Nagisa;

using Platform::IBox;

Object^ TaskStatusToVisibleConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Assassin::TransferTaskStatus;
	using Windows::UI::Xaml::Visibility;

	IBox<TransferTaskStatus>^ StatusObject =
		dynamic_cast<IBox<TransferTaskStatus>^>(value);
	String^ ParameterObject = dynamic_cast<String^>(parameter);

	if (StatusObject != nullptr && ParameterObject != nullptr)
	{
		struct { const wchar_t* Key; TransferTaskStatus Value; } List[] =
		{
			{ L"Canceled", TransferTaskStatus::Canceled },
			{ L"Completed", TransferTaskStatus::Completed },
			{ L"Error", TransferTaskStatus::Error },
			{ L"Paused", TransferTaskStatus::Paused },
			{ L"Queued", TransferTaskStatus::Queued },
			{ L"Running", TransferTaskStatus::Running }
		};

		const wchar_t* Parameter = ParameterObject->Data();
		TransferTaskStatus Status = StatusObject->Value;

		for (size_t i = 0; i < sizeof(List) / sizeof(*List); ++i)
		{
			if (0 == _wcsicmp(Parameter, List[i].Key))
			{			
				if (List[i].Value == Status)
					return Visibility::Visible;

				return Visibility::Collapsed;
			}

		}
	}

	M2ThrowPlatformException(E_INVALIDARG);
}

Object^ TaskStatusToVisibleConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ NullableBooleanToBooleanConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<bool>^ status = dynamic_cast<IBox<bool>^>(value);
	return (status != nullptr) ? status->Value : false;
}

Object^ NullableBooleanToBooleanConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<bool>^ status = dynamic_cast<IBox<bool>^>(value);
	return (status != nullptr) ? status->Value : false;
}
