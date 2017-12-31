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

MainPage::MainPage()
{
	InitializeComponent();
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

	M2AsyncSetCompletedHandler(
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





					M2::CThread([this]()
					{
						auto Tasks = M2AsyncWait(this->m_TransferManager->GetTasksAsync());

						if (nullptr != Tasks)
						{
							M2ExecuteOnUIThread([this, Tasks]()
							{
								this->TaskList->ItemsSource = Tasks;
							});
						}
					});
				}		
			}
		}
	});
}


void MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
	/*GUID guid = { 0 };
	HRESULT hr = CoCreateGuid(&guid);
	if (FAILED(hr))
	throw ref new Platform::Exception(hr);

	String^ x = (ref new Platform::Guid(guid))->ToString();*/

	this->m_TransferManager = ref new TransferManager();

	M2::CThread([this]()
	{
		auto Tasks = M2AsyncWait(this->m_TransferManager->GetTasksAsync());

		if (nullptr != Tasks)
		{
			M2ExecuteOnUIThread([this, Tasks]()
			{
				this->TaskList->ItemsSource = Tasks;
			});		
		}
	});
}


void MainPage::CopyLinkMenuItem_Click(Object^ sender, RoutedEventArgs^ e)
{
	try
	{
		ITransferTask^ Task = dynamic_cast<ITransferTask^>(
			dynamic_cast<FrameworkElement^>(sender)->DataContext);

		using Windows::ApplicationModel::DataTransfer::Clipboard;
		using Windows::ApplicationModel::DataTransfer::DataPackage;

		DataPackage^ data = ref new DataPackage();
		data->SetText(Task->RequestedUri->RawUri);

		Clipboard::SetContent(data);
	}
	catch (Exception^ ex)
	{
		
	}
}
