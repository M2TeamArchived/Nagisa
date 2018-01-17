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

void MainPage::RefreshTaskList()
{
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

void Nagisa::MainPage::SearchTaskList(String^ SearchFilter)
{
	this->m_TransferManager->SearchFilter = SearchFilter;

	this->RefreshTaskList();
}

void MainPage::AboutButton_Click(
	Object^ sender, 
	RoutedEventArgs^ e)
{
	AboutDialog^ dialog = ref new AboutDialog();
	dialog->m_TransferManager = this->m_TransferManager;
	dialog->ShowAsync();
}

void MainPage::NewTaskButton_Click(
	Object^ sender, 
	RoutedEventArgs^ e)
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

					this->RefreshTaskList();
				}		
			}
		}
	});
}

void MainPage::Page_Loaded(
	Object^ sender, 
	RoutedEventArgs^ e)
{	
	this->m_TransferManager = ref new TransferManager(true);

	this->RefreshTaskList();
}

void MainPage::CopyLinkMenuItem_Click(
	Object^ sender,
	RoutedEventArgs^ e)
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
	catch (...)
	{
		
	}
}

void MainPage::SearchAutoSuggestBox_QuerySubmitted(
	AutoSuggestBox^ sender, 
	AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	this->SearchTaskList(sender->Text);
}

void MainPage::SearchAutoSuggestBox_TextChanged(
	AutoSuggestBox^ sender, 
	AutoSuggestBoxTextChangedEventArgs^ args)
{
	using Windows::UI::Xaml::DispatcherTimer;
	using Windows::Foundation::EventHandler;
	using Windows::Foundation::TimeSpan;

	if (nullptr == sender->DataContext)
	{
		DispatcherTimer^ Timer = ref new DispatcherTimer();
		AutoSuggestBox^ SearchAutoSuggestBox = sender;

		TimeSpan Interval;
		Interval.Duration = 250 * 10000; // 10,000 ticks per millisecond.

		Timer->Interval = Interval;

		Timer->Tick += ref new EventHandler<Object^>(
			[this, SearchAutoSuggestBox, Timer](Object^ sender, Object^ args)
		{
			this->SearchTaskList(SearchAutoSuggestBox->Text);

			dynamic_cast<DispatcherTimer^>(sender)->Stop();
		});
		
		sender->DataContext = Timer;
	}

	DispatcherTimer^ Timer = dynamic_cast<DispatcherTimer^>(
		sender->DataContext);

	Timer->Stop();
	Timer->Start();
}

void MainPage::RetryButton_Click(
	Object^ sender, 
	RoutedEventArgs^ e)
{
	ITransferTask^ Task = dynamic_cast<ITransferTask^>(
		dynamic_cast<FrameworkElement^>(sender)->DataContext);

	M2::CThread([this, Task]()
	{
		using Windows::Storage::IStorageFile;

		Uri^ SourceUri = Task->RequestedUri;
		IStorageFile^ DestinationFile = Task->ResultFile;
		
		M2AsyncWait(Task->CancelAsync());
		
		M2ExecuteOnUIThread([this, SourceUri, DestinationFile]()
		{
			this->m_TransferManager->AddTask(SourceUri, DestinationFile);
			
			this->RefreshTaskList();
		});
	});
}

void MainPage::ResumeButton_Click(
	Object^ sender,
	RoutedEventArgs^ e)
{
	ITransferTask^ Task = dynamic_cast<ITransferTask^>(
		dynamic_cast<FrameworkElement^>(sender)->DataContext);

	Task->Resume();

	this->RefreshTaskList();
}

void MainPage::PauseButton_Click(
	Object^ sender,
	RoutedEventArgs^ e)
{
	ITransferTask^ Task = dynamic_cast<ITransferTask^>(
		dynamic_cast<FrameworkElement^>(sender)->DataContext);

	Task->Pause();

	this->RefreshTaskList();
}

void MainPage::TaskItemRemoveMenuItem_Click(
	Object^ sender,
	RoutedEventArgs^ e)
{
	ITransferTask^ Task = dynamic_cast<ITransferTask^>(
		dynamic_cast<FrameworkElement^>(sender)->DataContext);

	M2::CThread([this, Task]()
	{
		M2AsyncWait(Task->CancelAsync());

		M2ExecuteOnUIThread([this]()
		{
			this->RefreshTaskList();
		});
	});
}
