//
// AboutDialog.xaml.h
// AboutDialog 类的声明
//

#pragma once

#include "AboutDialog.g.h"

namespace Nagisa
{
	using Assassin::TransferManager;
	using Platform::Object;
	using Windows::UI::Xaml::Controls::ContentDialog;
	using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
	using Windows::UI::Xaml::RoutedEventArgs;
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AboutDialog sealed
	{
	public:
		AboutDialog();
	internal:
		TransferManager^ m_TransferManager = nullptr;
	private:
		void ContentDialog_Loaded(
			Object^ sender,
			RoutedEventArgs^ e);
		void GitHubButtonClick(
			ContentDialog^ sender, 
			ContentDialogButtonClickEventArgs^ args);
	};
}
