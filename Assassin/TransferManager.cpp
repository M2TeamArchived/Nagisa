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

	using Windows::Storage::AccessCache::StorageApplicationPermissions;
	this->m_StorageItemAccessList =
		StorageApplicationPermissions::FutureAccessList;

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

			for (ITransferTask^ Task : this->m_TaskList)
			{
				TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);
					
				TaskInternal->RaisePropertyChanged(L"Status");
				
				if (TransferTaskStatus::Running == Task->Status)
				{		
					TaskInternal->RaisePropertyChanged(L"BytesReceived");
					TaskInternal->RaisePropertyChanged(L"BytesReceivedSpeed");
					TaskInternal->RaisePropertyChanged(L"RemainTime");
					TaskInternal->RaisePropertyChanged(L"TotalBytesToReceive");
				}	
			}

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
	delete this->m_Downloader;
	DeleteCriticalSection(&this->m_TaskListUpdateCS);
	
	if (nullptr != this->m_UINotifyTimer)
	{
		this->m_UINotifyTimer->Stop();
		delete this->m_UINotifyTimer;
	}
}

// Gets the version of Nagisa.
String^ TransferManager::Version::get()
{
	return NAGISA_VERSION_STRING;
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
		using Windows::Storage::AccessCache::AccessListEntry;
		using Windows::Storage::ApplicationDataCompositeValue;

		VectorView<ITransferTask^>^ Result = nullptr;
		
		EnterCriticalSection(&this->m_TaskListUpdateCS);

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
			String^ TaskGuid = Download->Key;
			ApplicationDataCompositeValue^ TaskConfig =
				dynamic_cast<ApplicationDataCompositeValue^>(Download->Value);

			String^ BackgroundTransferGuid = dynamic_cast<String^>(
				TaskConfig->Lookup(L"BackgroundTransferGuid"));

			Uri^ SourceUri = ref new Uri(dynamic_cast<String^>(
				TaskConfig->Lookup(L"SourceUri")));
			String^ FileName = dynamic_cast<String^>(
				TaskConfig->Lookup(L"FileName"));
			String^ SaveFolderPath = dynamic_cast<String^>(
				TaskConfig->Lookup(L"SaveFolderPath"));

			IStorageFolder^ SaveFolder = nullptr;
			for (AccessListEntry Entry : this->m_StorageItemAccessList->Entries)
			{
				if (SaveFolderPath == Entry.Metadata)
				{
					SaveFolder = M2AsyncWait(
						this->m_StorageItemAccessList->GetFolderAsync(
							Entry.Token));
					break;
				}
			}

			std::map<String^, DownloadOperation^>::iterator iterator =
				DownloadsList.find(BackgroundTransferGuid);

			DownloadOperation^ Operation =
				(DownloadsList.end() != iterator) ? iterator->second : nullptr;

			if (NeedSearchFilter)
			{
				if (Operation && 
					!M2FindSubString(
						Operation->ResultFile->Name,
						CurrentSearchFilter,
						true))
				{
					continue;
				}
			}

			this->m_TaskList.push_back(ref new TransferTask(
				Operation, TaskGuid, SourceUri, FileName, SaveFolder));
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
		using Windows::Storage::AccessCache::AccessListEntry;
		using Windows::Storage::ApplicationDataCompositeValue;
		using Windows::Storage::CreationCollisionOption;
		using Windows::Storage::StorageFile;

		StorageFile^ SaveFile = M2AsyncWait(SaveFolder->CreateFileAsync(
			DesiredFileName, CreationCollisionOption::GenerateUniqueName));

		String^ SaveFolderPath = SaveFolder->Path;
		bool NeedAddToFutureAccessList = true;
		for (AccessListEntry Entry : this->m_StorageItemAccessList->Entries)
		{
			if (SaveFolderPath == Entry.Metadata)
			{
				NeedAddToFutureAccessList = false;
				break;
			}
		}
		if (NeedAddToFutureAccessList)
		{
			this->m_StorageItemAccessList->Add(SaveFolder, SaveFolderPath);
		}
		
		DownloadOperation^ Operation = this->m_Downloader->CreateDownload(
			SourceUri, SaveFile);

		ApplicationDataCompositeValue^ TaskConfig =
			ref new ApplicationDataCompositeValue();

		TaskConfig->Insert(
			L"BackgroundTransferGuid",
			Operation->Guid.ToString());

		TaskConfig->Insert(
			L"SourceUri",
			SourceUri->RawUri);
		TaskConfig->Insert(
			L"FileName",
			SaveFile->Name);
		TaskConfig->Insert(
			L"SaveFolderPath",
			SaveFolderPath);

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
//   The function does not return a value.
void TransferManager::RemoveTask(
	ITransferTask^ Task)
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
		Task->SaveFile->DeleteAsync(StorageDeleteOption::PermanentDelete);
	}

	this->m_TasksContainer->Values->Remove(Task->Guid);

	LeaveCriticalSection(&this->m_TaskListUpdateCS);
}
