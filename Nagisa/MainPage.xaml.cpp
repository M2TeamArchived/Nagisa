//
// MainPage.xaml.cpp
// MainPage 类的实现。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "AboutDialog.xaml.h"
#include "NewTaskDialog.xaml.h"

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

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

MainPage::MainPage()
{
	InitializeComponent();

	m_TransferManager = ref new TransferManager();
}


void MainPage::AboutButtonClick(Object^ sender, RoutedEventArgs^ e)
{
	AboutDialog^ dialog = ref new AboutDialog();
	dialog->m_TransferManager = this->m_TransferManager;
	dialog->ShowAsync();
}

void MainPage::NewTaskButtonClick(Object^ sender, RoutedEventArgs^ e)
{
	NewTaskDialog^ dialog = ref new NewTaskDialog();
	dialog->m_TransferManager = this->m_TransferManager;

	M2SetAsyncCompletedHandler(
		dialog->ShowAsync(),
		[this, dialog](
			IAsyncOperation<ContentDialogResult>^ asyncInfo,
			AsyncStatus asyncStatus)
	{
		if (AsyncStatus::Completed == asyncStatus)
		{
			if (ContentDialogResult::Primary == asyncInfo->GetResults())
			{
				using Windows::Storage::CreationCollisionOption;
				using Windows::Storage::StorageFile;

				StorageFile^ SaveFile = M2AsyncWait(
					dialog->m_SaveFolder->CreateFileAsync(
						dialog->m_FileName,
						CreationCollisionOption::GenerateUniqueName));

				if (nullptr != SaveFile)
				{
					this->m_TransferManager->AddTask(
						dialog->m_DownloadSource, 
						SaveFile);
				}		
			}
		}
	});
}
