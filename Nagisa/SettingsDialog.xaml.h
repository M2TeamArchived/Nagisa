//
// SettingsDialog.xaml.h
// SettingsDialog 类的声明
//

#pragma once

#include "SettingsDialog.g.h"

namespace Nagisa
{
	using Assassin::ITransferManager;
	using Platform::Object;
	using Windows::Storage::IStorageFolder;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	using Windows::UI::Xaml::RoutedEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsDialog sealed
	{
	public:
		SettingsDialog(
			ITransferManager^ TransferManager);
	private:
		ITransferManager ^ m_TransferManager = nullptr;

		IStorageFolder^ m_LastusedFolder = nullptr;

		void ContentDialog_Loaded(
			Object^ sender, 
			RoutedEventArgs^ e);
		void CustomDownloadFolderBrowseButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void UseCustomFolder_Click(
			Object^ sender,
			RoutedEventArgs^ e);
	};
}
