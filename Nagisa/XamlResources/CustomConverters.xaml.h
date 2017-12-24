//
// CustomConverters.xaml.h
// CustomConverters 类的声明
//

#pragma once

#include "XamlResources\CustomConverters.g.h"

namespace Nagisa
{
	using Platform::Object;
	using Platform::String;
	using Windows::UI::Xaml::Data::IValueConverter;
	using Windows::UI::Xaml::Interop::TypeName;
	
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Uint64ToDoubleConverter sealed : IValueConverter
	{
	public:
		// Inherited via IValueConverter
		virtual Object^ Convert(
			Object^ value, 
			TypeName targetType, 
			Object^ parameter, 
			String^ language);
		virtual Object^ ConvertBack(
			Object^ value, 
			TypeName targetType, 
			Object ^parameter, 
			String^ language);
	};

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Uint64ToByteSizeStringConverter sealed : IValueConverter
	{
	public:
		// Inherited via IValueConverter
		virtual Object^ Convert(
			Object^ value,
			TypeName targetType,
			Object^ parameter,
			String^ language);
		virtual Object^ ConvertBack(
			Object^ value,
			TypeName targetType,
			Object ^parameter,
			String^ language);
	};

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StorageFileToFileNameConverter sealed : IValueConverter
	{
	public:
		// Inherited via IValueConverter
		virtual Object^ Convert(
			Object^ value,
			TypeName targetType,
			Object^ parameter,
			String^ language);
		virtual Object^ ConvertBack(
			Object^ value,
			TypeName targetType,
			Object ^parameter,
			String^ language);
	};
}
