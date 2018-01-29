//
// NewTaskDialog.xaml.h
// NewTaskDialog 类的声明
//

#pragma once

#include "NewTaskDialog.g.h"

namespace Nagisa
{
	using Assassin::ITransferManager;
	using Platform::Object;
	using Platform::String;
	using Windows::Foundation::Uri;
	using Windows::Storage::IStorageFolder;
	using Windows::UI::Colors;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	using Windows::UI::Xaml::Media::SolidColorBrush;
	using Windows::UI::Xaml::RoutedEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class NewTaskDialog sealed
	{
	public:
		NewTaskDialog(
			ITransferManager^ TransferManager);
	private:
		ITransferManager ^ m_TransferManager = nullptr;

		SolidColorBrush^ m_NormalBrush = nullptr;
		SolidColorBrush^ m_NoticeableBrush = nullptr;

		Uri^ m_DownloadSource = nullptr;
		String^ m_FileName = nullptr;
		IStorageFolder^ m_SaveFolder = nullptr;

		void DownloadButtonClick(
			ContentDialog^ sender, 
			ContentDialogButtonClickEventArgs^ args);
		void BrowseButtonClick(
			Object^ sender,
			RoutedEventArgs^ e);
		void DownloadSourceTextBox_LostFocus(
			Object^ sender, 
			RoutedEventArgs^ e);
		void FileNameTextBox_LostFocus(
			Object^ sender,
			RoutedEventArgs^ e);
		void ContentDialog_Loaded(
			Object^ sender,
			RoutedEventArgs^ e);
	};
}
