/******************************************************************************
Project: Assassin
Description: Implementation for TransferManager.
File Name: TransferManager.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"

#include <map>

using namespace Assassin;
using namespace Platform;

void TransferManager::RaisePropertyChanged(String^ PropertyName)
{
	using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
	this->PropertyChanged(
		this, ref new PropertyChangedEventArgs(PropertyName));
}

// Creates a new TransferManager object.
// Parameters:
//   EnableUINotify: Enable the UI notify timer if true. 
// Return value:
//   The function does not return a value.
TransferManager::TransferManager(
	bool EnableUINotify)
{
	this->m_Downloader = ref new BackgroundDownloader();

	InitializeCriticalSection(&this->m_TaskListUpdateCS);

	using Windows::Storage::AccessCache::StorageApplicationPermissions;
	this->m_FutureAccessList =
		StorageApplicationPermissions::FutureAccessList;

	using Windows::Storage::ApplicationData;
	using Windows::Storage::ApplicationDataCreateDisposition;

	this->m_RootContainer =
		ApplicationData::Current->LocalSettings->CreateContainer(
			L"Nagisa",
			ApplicationDataCreateDisposition::Always);
	this->m_TasksContainer =
		this->m_RootContainer->CreateContainer(
			L"Tasks",
			ApplicationDataCreateDisposition::Always);

	if (this->m_RootContainer->Values->HasKey(L"LastusedFolder"))
	{
		try
		{
			this->m_LastusedFolder = dynamic_cast<IStorageFolder^>(M2AsyncWait(
				this->m_FutureAccessList->GetItemAsync(dynamic_cast<String^>(
					this->m_RootContainer->Values->Lookup(
						L"LastusedFolder")))));
		}
		catch (...)
		{
			this->m_LastusedFolder = nullptr;
			this->m_RootContainer->Values->Remove(L"LastusedFolder");
		}
	}

	if (this->m_RootContainer->Values->HasKey(L"DefaultFolder"))
	{
		try
		{
			this->m_DefaultFolder = dynamic_cast<IStorageFolder^>(M2AsyncWait(
				this->m_FutureAccessList->GetItemAsync(dynamic_cast<String^>(
					this->m_RootContainer->Values->Lookup(
						L"DefaultFolder")))));
		}
		catch (...)
		{
			this->m_DefaultFolder = nullptr;
			this->m_RootContainer->Values->Remove(L"DefaultFolder");
		}	
	}

	if (EnableUINotify)
	{
		using Windows::Foundation::EventHandler;
		using Windows::Foundation::TimeSpan;
		
		this->m_UINotifyTimer = ref new DispatcherTimer();

		TimeSpan Interval;
		Interval.Duration = 1000 * 10000; // 10,000 ticks per millisecond.

		this->m_UINotifyTimer->Interval = Interval;

		this->m_UINotifyTimer->Tick += ref new EventHandler<Object^>(
			[this](Object^ sender, Object^ args)
		{
			EnterCriticalSection(&this->m_TaskListUpdateCS);

			this->m_TotalDownloadBandwidth = 0;
			this->m_TotalUploadBandwidth = 0;

			for (ITransferTask^ Task : this->m_TaskList)
			{
				if (nullptr == Task) continue;
				
				TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);

				this->m_TasksContainer->Values->Insert(
					TaskInternal->Guid, TaskInternal->GetTaskConfig());

				TaskInternal->UpdateChangedProperties();
				TaskInternal->NotifyPropertyChanged();

				this->m_TotalDownloadBandwidth += TaskInternal->BytesReceivedSpeed;
				this->m_TotalUploadBandwidth += 0;
			}

			this->RaisePropertyChanged(L"TotalDownloadBandwidth");
			this->RaisePropertyChanged(L"TotalUploadBandwidth");

			LeaveCriticalSection(&this->m_TaskListUpdateCS);
		});

		this->m_UINotifyTimer->Start();
	}
}

// Destroys a TransferManager object.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
TransferManager::~TransferManager()
{
	DeleteCriticalSection(&this->m_TaskListUpdateCS);
	
	if (nullptr != this->m_UINotifyTimer)
	{
		this->m_UINotifyTimer->Stop();
	}
}

// Gets the version of Nagisa.
String^ TransferManager::Version::get()
{
	return NAGISA_VERSION_STRING;
}

// Gets the last used folder.
IStorageFolder^ TransferManager::LastusedFolder::get()
{
	return this->m_LastusedFolder;
}

// Gets the default download folder.
IStorageFolder^ TransferManager::DefaultFolder::get()
{
	return this->m_DefaultFolder;
}

// Sets the default download folder.
void TransferManager::DefaultFolder::set(
	IStorageFolder^ value)
{
	this->m_DefaultFolder = value;
	
	if (nullptr != value)
	{
		this->m_RootContainer->Values->Insert(
			L"DefaultFolder", 
			this->m_FutureAccessList->Add(value));
	}
	else
	{
		if (this->m_RootContainer->Values->HasKey(L"DefaultFolder"))
		{
			this->m_RootContainer->Values->Remove(L"DefaultFolder");
		}	
	}
}

// Gets the total download bandwidth.
uint64 TransferManager::TotalDownloadBandwidth::get()
{
	return this->m_TotalDownloadBandwidth;
}

// Gets the total upload bandwidth.
uint64 TransferManager::TotalUploadBandwidth::get()
{
	return this->m_TotalUploadBandwidth;
}

// Gets the task list.
// Parameters:
//   The function does not have parameters.
// Return value:
//   Returns an object which represents the task list.
IAsyncOperation<ITransferTaskVector^>^ TransferManager::GetTasksAsync()
{
	return M2AsyncCreate(
		[this](IM2AsyncController^ AsyncController)
			-> ITransferTaskVector^
	{
		using Platform::Collections::VectorView;
		using Windows::Foundation::Collections::IKeyValuePair;
		using Windows::Foundation::Collections::IVectorView;
		using Windows::Networking::BackgroundTransfer::DownloadOperation;
		using Windows::Storage::ApplicationDataCompositeValue;

		VectorView<ITransferTask^>^ Result = nullptr;
		
		EnterCriticalSection(&this->m_TaskListUpdateCS);

		for (ITransferTask^ Task : this->m_TaskList)
		{
			if (nullptr == Task) continue;

			TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);

			this->m_TasksContainer->Values->Insert(
				TaskInternal->Guid, TaskInternal->GetTaskConfig());
		}

		this->m_TaskList.clear();

		String^ CurrentSearchFilter = this->SearchFilter;

		bool NeedSearchFilter = (
			nullptr != CurrentSearchFilter &&
			!CurrentSearchFilter->IsEmpty());

		std::map<String^, DownloadOperation^> DownloadsList;

		for (DownloadOperation^ Item 
			: M2AsyncWait(this->m_Downloader->GetCurrentDownloadsAsync()))
		{
			DownloadsList.insert(std::pair<String^, DownloadOperation^>(
				Item->Guid.ToString(), Item));
		}	

		for (IKeyValuePair<String^, Object^>^ Download
			: this->m_TasksContainer->Values)
		{
			TransferTask^ Task = ref new TransferTask(
				Download->Key,
				dynamic_cast<ApplicationDataCompositeValue^>(Download->Value),
				this->m_FutureAccessList,
				DownloadsList);

			if (NeedSearchFilter)
			{
				if (!M2FindSubString(
					Task->FileName, CurrentSearchFilter, true))
				{
					continue;
				}
			}	
					
			this->m_TaskList.push_back(Task);
		}

		Result = ref new VectorView<ITransferTask^>(this->m_TaskList);

		LeaveCriticalSection(&this->m_TaskListUpdateCS);
	
		return Result;
	});
}

// Add a task to the task list.
// Parameters:
//   SourceUri: The source uri object of task.
//   DesiredFileName: The file name you desire.
//   SaveFolder: The object of the folder which you want to save.
// Return value:
//   Returns an asynchronous object used to wait.
IAsyncAction^ TransferManager::AddTaskAsync(
	Uri^ SourceUri,
	String^ DesiredFileName,
	IStorageFolder^ SaveFolder)
{
	return M2AsyncCreate(
		[this, SourceUri, DesiredFileName, SaveFolder](
			IM2AsyncController^ AsyncController) -> void
	{
		using Windows::Networking::BackgroundTransfer::DownloadOperation;
		using Windows::Storage::ApplicationDataCompositeValue;
		using Windows::Storage::CreationCollisionOption;
		using Windows::Storage::StorageFile;

		StorageFile^ SaveFile = M2AsyncWait(SaveFolder->CreateFileAsync(
			DesiredFileName, CreationCollisionOption::GenerateUniqueName));

		String^ Token = this->m_FutureAccessList->Add(SaveFolder);

		this->m_LastusedFolder = SaveFolder;
		this->m_RootContainer->Values->Insert(
			L"LastusedFolder", 
			Token);
		
		DownloadOperation^ Operation = this->m_Downloader->CreateDownload(
			SourceUri, SaveFile);

		ApplicationDataCompositeValue^ TaskConfig =
			ref new ApplicationDataCompositeValue();

		TaskConfig->Insert(
			L"SourceUri",
			SourceUri->RawUri);
		TaskConfig->Insert(
			L"FileName",
			SaveFile->Name);
		TaskConfig->Insert(
			L"SaveFolder",
			Token);
		TaskConfig->Insert(
			L"Status",
			Windows::Foundation::PropertyValue::CreateUInt8(
				static_cast<uint8>(TransferTaskStatus::Queued)));

		TaskConfig->Insert(
			L"BackgroundTransferGuid",
			Operation->Guid.ToString());

		this->m_TasksContainer->Values->Insert(
			M2CreateGuid().ToString(),
			TaskConfig);

		Operation->StartAsync();
	});
}

// Removes a task to the task list.
// Parameters:
//   Task: The task object. 
// Return value:
//   Returns an asynchronous object used to wait.
IAsyncAction^ TransferManager::RemoveTaskAsync(
	ITransferTask^ Task)
{
	return M2AsyncCreate(
		[this, Task](
			IM2AsyncController^ AsyncController) -> void
	{
		EnterCriticalSection(&this->m_TaskListUpdateCS);

		switch (Task->Status)
		{
		case TransferTaskStatus::Paused:
		case TransferTaskStatus::Queued:
		case TransferTaskStatus::Running:
			Task->Cancel();
			break;
		default:
			break;
		}

		if (TransferTaskStatus::Completed != Task->Status)
		{
			using Windows::Storage::StorageDeleteOption;
			try
			{
				IStorageFile^ SaveFile = Task->SaveFile;
				if (nullptr != SaveFile)
				{
					M2AsyncWait(SaveFile->DeleteAsync(
						StorageDeleteOption::PermanentDelete));
				}
			}
			catch (...)
			{

			}
		}

		for (size_t i = 0; i < this->m_TaskList.size(); ++i)
		{
			if (nullptr == this->m_TaskList[i]) continue;
			
			if (Task->Guid == this->m_TaskList[i]->Guid)
			{
				this->m_TaskList[i] = nullptr;
			}
		}
		this->m_TasksContainer->Values->Remove(Task->Guid);

		LeaveCriticalSection(&this->m_TaskListUpdateCS);
	});
}

// Start all tasks.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::StartAllTasks()
{
	EnterCriticalSection(&this->m_TaskListUpdateCS);

	for (ITransferTask^ Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task->Resume();
	}

	LeaveCriticalSection(&this->m_TaskListUpdateCS);
}

// Pause all tasks.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::PauseAllTasks()
{
	EnterCriticalSection(&this->m_TaskListUpdateCS);

	for (ITransferTask^ Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task->Pause();
	}

	LeaveCriticalSection(&this->m_TaskListUpdateCS);
}

// Clears the task list.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::ClearTaskList()
{
	EnterCriticalSection(&this->m_TaskListUpdateCS);

	for (ITransferTask^ Task : this->m_TaskList)
	{
		switch (Task->Status)
		{
		case TransferTaskStatus::Canceled:
		case TransferTaskStatus::Completed:
		case TransferTaskStatus::Error:
			this->RemoveTaskAsync(Task);
			break;
		default:
			break;
		}
	}

	LeaveCriticalSection(&this->m_TaskListUpdateCS);
}

// Creates a new TransferManager object.
// Parameters:
//   The function does not have parameters.
// Return value:
//   Returns a new TransferManager object.
ITransferManager^ TransferManagerFactory::CreateInstance()
{
	return ref new TransferManager(true);
}
