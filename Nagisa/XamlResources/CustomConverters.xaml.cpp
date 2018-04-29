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

Object^ Uint64ToDoubleConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<uint64>^ status = dynamic_cast<IBox<uint64>^>(value);
	return static_cast<double>((status != nullptr) ? status->Value : 0);
}

Object^ Uint64ToDoubleConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ Uint64ByteSizeToStringConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<uint64>^ status = dynamic_cast<IBox<uint64>^>(value);

	return winrt::to_cx<Object>(winrt::box_value(M2ConvertByteSizeToString(
		(status != nullptr) ? status->Value : 0)));
}

Object^ Uint64ByteSizeToStringConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ Uint64RemainTimeToStringConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<uint64>^ status = dynamic_cast<IBox<uint64>^>(value);
	if (status != nullptr)
	{
		uint64 Seconds = status->Value;
		if (-1 != Seconds)
		{
			int Hour = static_cast<int>(Seconds / 3600);
			int Minute = static_cast<int>(Seconds / 60 % 60);
			int Second = static_cast<int>(Seconds % 60);

			return ref new String(
				M2FormatString(L"%d:%02d:%02d", Hour, Minute, Second).c_str());
		}
	}

	return L"N/A";
}

Object^ Uint64RemainTimeToStringConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ TaskListEmptyToVisibilityConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Assassin::ITransferTask;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::UI::Xaml::Visibility;

	IVectorView<ITransferTask^>^ ItemSource =
		dynamic_cast<IVectorView<ITransferTask^>^>(value);

	if (ItemSource == nullptr || ItemSource->Size == 0)
	{
		return Visibility::Visible;
	}

	return Visibility::Collapsed;
}

Object^ TaskListEmptyToVisibilityConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

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
