//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

#include "MainPage.g.h"

namespace Nagisa
{
	using Assassin::ITransferManager;
	using Platform::Object;
	using Platform::String;
	using Windows::Foundation::IAsyncOperation;
	using Windows::UI::Xaml::RoutedEventArgs;
	using Windows::UI::Xaml::Controls::AutoSuggestBox;
	using Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs;
	using Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogResult;
	using Windows::UI::Xaml::Data::INotifyPropertyChanged;
	using Windows::UI::Xaml::Data::PropertyChangedEventHandler;

	public ref class MainPage sealed
	{
	public:
		MainPage();

		property ITransferManager^ TransferManager
		{
			ITransferManager^ get();
		}

	private:
		ITransferManager ^ m_TransferManager = nullptr;

		void RefreshTaskList();
		void RefreshTaskListAsync();
		void SearchTaskList(
			String^ SearchFilter);
		IAsyncOperation<ContentDialogResult>^ ShowContentDialogAsync(
			ContentDialog^ Dialog);

		void AboutButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void NewTaskButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void Page_Loaded(
			Object^ sender,
			RoutedEventArgs^ e);
		void CopyLinkMenuItem_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void SearchAutoSuggestBox_QuerySubmitted(
			AutoSuggestBox^ sender,
			AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void SearchAutoSuggestBox_TextChanged(
			AutoSuggestBox^ sender, 
			AutoSuggestBoxTextChangedEventArgs^ args);
		void RetryButton_Click(
			Object^ sender, 
			RoutedEventArgs^ e);
		void ResumeButton_Click(
			Object^ sender, 
			RoutedEventArgs^ e);
		void PauseButton_Click(
			Object^ sender, 
			RoutedEventArgs^ e);
		void CancelMenuItem_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void RemoveMenuItem_Click(
			Object^ sender, 
			RoutedEventArgs^ e);
		void OpenFolderMenuItem_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void StartAllAppBarButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void PauseAllAppBarButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void ClearListAppBarButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void OpenDownloadsFolderAppBarButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
		void SettingsAppBarButton_Click(
			Object^ sender,
			RoutedEventArgs^ e);
	};
}
