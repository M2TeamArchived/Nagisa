//
// NewTaskDialog.xaml.h
// NewTaskDialog 类的声明
//

#pragma once

#include "NewTaskDialog.g.h"

namespace Nagisa
{
	using Assassin::TransferManager;
	using Platform::Object;
	using Platform::String;
	using Windows::Foundation::Uri;
	using Windows::Storage::StorageFolder;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	using Windows::UI::Xaml::RoutedEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class NewTaskDialog sealed
	{
	public:
		NewTaskDialog();
	internal:
		TransferManager ^ m_TransferManager = nullptr;
		Uri^ m_DownloadSource = nullptr;
		String^ m_FileName = nullptr;
		StorageFolder^ m_SaveFolder = nullptr;
	private:
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
	};
}
