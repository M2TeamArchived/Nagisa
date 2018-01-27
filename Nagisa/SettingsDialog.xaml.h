//
// SettingsDialog.xaml.h
// SettingsDialog 类的声明
//

#pragma once

#include "SettingsDialog.g.h"

namespace Nagisa
{
	using Assassin::TransferManager;
	using Platform::Object;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	using Windows::UI::Xaml::RoutedEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsDialog sealed
	{
	public:
		SettingsDialog();
	internal:
		TransferManager^ m_TransferManager = nullptr;
	private:

		void ContentDialog_Loaded(
			Object^ sender, 
			RoutedEventArgs^ e);
	};
}
