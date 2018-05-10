//
// AboutDialog.xaml.cpp
// AboutDialog 类的实现
//

#include "pch.h"
#include "AboutDialog.xaml.h"

using namespace Nagisa;
using namespace Assassin;

AboutDialog::AboutDialog(
	ITransferManager^ TransferManager) :
	m_TransferManager(TransferManager)
{
	InitializeComponent();
}

void AboutDialog::ContentDialog_Loaded(
	Object^ sender,
	RoutedEventArgs^ e)
{
	VersionText->Text = m_TransferManager->Version;
}

void AboutDialog::GitHubButtonClick(
	ContentDialog^ sender, 
	ContentDialogButtonClickEventArgs^ args)
{
	using Windows::Foundation::Uri;
	using Windows::System::Launcher;
	
	Launcher::LaunchUriAsync(
		ref new Uri(L"https://github.com/Project-Nagisa/Nagisa"));

	args->Cancel = true;
}
