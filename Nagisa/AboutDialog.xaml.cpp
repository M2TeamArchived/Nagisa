//
// AboutDialog.xaml.cpp
// AboutDialog 类的实现
//

#include "pch.h"
#include "AboutDialog.xaml.h"

using namespace Nagisa;
using namespace Assassin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


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
	using Windows::System::Launcher;

	Launcher::LaunchUriAsync(
		ref new Uri("https://github.com/Project-Nagisa/Nagisa"));

	args->Cancel = true;
}
