//
// CustomConverters.xaml.cpp
// CustomConverters 类的实现
//

#include "pch.h"
#include "CustomConverters.xaml.h"

using namespace Nagisa;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

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

Object^ Uint64ToByteSizeStringConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<uint64>^ status = dynamic_cast<IBox<uint64>^>(value);

	if (status != nullptr)
	{
		double result = static_cast<double>(status->Value);

		if (0.0 == result)
		{
			return L"0 Byte";
		}

		StringReference Systems[] = { L"Bytes", L"KiB", L"MiB", L"GiB", L"TiB" };
		size_t i = 0;
		for (; i < sizeof(Systems) / sizeof(*Systems); ++i)
		{
			if (1024.0 > result)
				break;
			
			result /= 1024.0;	
		}
		
		uint64 temp = static_cast<uint64>(result * 100);
		return (temp / 100.0).ToString() + Systems[i];
	}
	else
	{
		return L"0 Byte";
	}
}

Object^ Uint64ToByteSizeStringConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ Uint64ToByteSpeedStringConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	IBox<uint64>^ status = dynamic_cast<IBox<uint64>^>(value);

	if (status != nullptr)
	{
		double result = static_cast<double>(status->Value);

		if (0.0 == result)
		{
			return L"0 Byte/s";
		}

		StringReference Systems[] = { L"Bytes/s", L"KiB/s", L"MiB/s", L"GiB/s", L"TiB/s" };
		size_t i = 0;
		for (; i < sizeof(Systems) / sizeof(*Systems); ++i)
		{
			if (1024.0 > result)
				break;

			result /= 1024.0;
		}

		uint64 temp = static_cast<uint64>(result * 100);
		return (temp / 100.0).ToString() + Systems[i];
	}
	else
	{
		return L"0 Byte/s";
	}
}

Object^ Uint64ToByteSpeedStringConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ StorageFileToFileNameConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Windows::Storage::IStorageFile;	
	IStorageFile^ result = dynamic_cast<IStorageFile^>(value);
	return ((result != nullptr) ? result->Name : L"N/A");
}

Object^ StorageFileToFileNameConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ StatusErrorToVisibleConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Assassin::TransferTaskStatus;
	IBox<TransferTaskStatus>^ status = 
		dynamic_cast<IBox<TransferTaskStatus>^>(value);

	if (status != nullptr && TransferTaskStatus::Error == status->Value)
	{
		return Visibility::Visible;
	}

	return Visibility::Collapsed;
}

Object^ StatusErrorToVisibleConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ StatusPausedToVisibleConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Assassin::TransferTaskStatus;
	IBox<TransferTaskStatus>^ status =
		dynamic_cast<IBox<TransferTaskStatus>^>(value);

	if (status != nullptr && TransferTaskStatus::Paused == status->Value)
	{
		return Visibility::Visible;
	}

	return Visibility::Collapsed;
}

Object^ StatusPausedToVisibleConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ StatusRunningToVisibleConverter::Convert(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	using Assassin::TransferTaskStatus;
	IBox<TransferTaskStatus>^ status =
		dynamic_cast<IBox<TransferTaskStatus>^>(value);

	if (status != nullptr && TransferTaskStatus::Running == status->Value)
	{
		return Visibility::Visible;
	}

	return Visibility::Collapsed;
}

Object^ StatusRunningToVisibleConverter::ConvertBack(
	Object^ value,
	TypeName targetType,
	Object^ parameter,
	String^ language)
{
	M2ThrowPlatformException(E_NOTIMPL);
}

Object^ RemainTimeToTimeStringConverter::Convert(
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

Object^ RemainTimeToTimeStringConverter::ConvertBack(
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

