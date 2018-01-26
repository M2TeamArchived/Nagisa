//
// SettingsDialog.xaml.h
// SettingsDialog 类的声明
//

#pragma once

#include "SettingsDialog.g.h"

namespace Nagisa
{
	using Assassin::TransferManager;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsDialog sealed
	{
	public:
		SettingsDialog();
	internal:
		TransferManager^ m_TransferManager = nullptr;
	private:

	};
}
