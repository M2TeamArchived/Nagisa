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
	throw ref new Platform::NotImplementedException();
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

		StringReference Systems[] = { L"Bytes", L"KiB", L"MiB", L"GiB" };

		for (size_t i = 0; i < sizeof(Systems) / sizeof(*Systems); ++i)
		{
			if (1024.0 > result)
			{				
				uint64 temp = static_cast<uint64>(result * 100);
				
				return (temp / 100.0).ToString() + Systems[i];
			}
			else
			{
				result /= 1024.0;
			}	
		}
		
		return result.ToString() + L"TiB";
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
	throw ref new Platform::NotImplementedException();
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
	throw ref new Platform::NotImplementedException();
}
