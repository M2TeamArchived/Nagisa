﻿/******************************************************************************
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
	this->m_Downloader = winrt::BackgroundDownloader();

	InitializeCriticalSection(&this->m_TaskListUpdateCS);
	
	this->m_FutureAccessList =
		winrt::StorageApplicationPermissions::FutureAccessList();

	this->m_RootContainer =
		winrt::ApplicationData::Current().LocalSettings().CreateContainer(
			L"Nagisa",
			winrt::ApplicationDataCreateDisposition::Always);
	this->m_TasksContainer =
		this->m_RootContainer.CreateContainer(
			L"Tasks",
			winrt::ApplicationDataCreateDisposition::Always);

	if (this->m_RootContainer.Values().HasKey(L"LastusedFolder"))
	{
		try
		{		
			this->m_LastusedFolder = this->m_FutureAccessList.GetFolderAsync(
				winrt::unbox_value<winrt::hstring>(
					this->m_RootContainer.Values().Lookup(
						L"LastusedFolder"))).get();
		}
		catch (...)
		{
			this->m_LastusedFolder = nullptr;
			this->m_RootContainer.Values().Remove(L"LastusedFolder");
		}
	}

	if (this->m_RootContainer.Values().HasKey(L"DefaultFolder"))
	{
		try
		{
			this->m_DefaultFolder = this->m_FutureAccessList.GetFolderAsync(
				winrt::unbox_value<winrt::hstring>(
					this->m_RootContainer.Values().Lookup(
						L"DefaultFolder"))).get();
		}
		catch (...)
		{
			this->m_DefaultFolder = nullptr;
			this->m_RootContainer.Values().Remove(L"DefaultFolder");
		}	
	}

	if (EnableUINotify)
	{
		this->m_UINotifyTimer = winrt::DispatcherTimer();

		// 10,000 ticks per millisecond.
		this->m_UINotifyTimer.Interval(winrt::TimeSpan(1000 * 10000));

		this->m_UINotifyTimer.Tick(
			[this](const winrt::IInspectable sender, const winrt::IInspectable args)
		{
			EnterCriticalSection(&this->m_TaskListUpdateCS);

			this->m_TotalDownloadBandwidth = 0;
			this->m_TotalUploadBandwidth = 0;

			for (ITransferTask^ Task : this->m_TaskList)
			{
				if (nullptr == Task) continue;

				TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);

				this->m_TasksContainer.Values().Insert(
					TaskInternal->GuidInternal,
					TaskInternal->GetTaskConfig());

				TaskInternal->UpdateChangedProperties();
				TaskInternal->NotifyPropertyChanged();

				this->m_TotalDownloadBandwidth += TaskInternal->BytesReceivedSpeed;
				this->m_TotalUploadBandwidth += 0;
			}

			this->RaisePropertyChanged(L"TotalDownloadBandwidth");
			this->RaisePropertyChanged(L"TotalUploadBandwidth");

			LeaveCriticalSection(&this->m_TaskListUpdateCS);
		});

		this->m_UINotifyTimer.Start();
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
		this->m_UINotifyTimer.Stop();
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
	return winrt::to_cx<IStorageFolder>(this->m_LastusedFolder);
}

// Gets the default download folder.
IStorageFolder^ TransferManager::DefaultFolder::get()
{
	return winrt::to_cx<IStorageFolder>(this->m_DefaultFolder);
}

// Sets the default download folder.
void TransferManager::DefaultFolder::set(
	IStorageFolder^ value)
{
	this->m_DefaultFolder = winrt::from_cx<winrt::IStorageFolder>(value);
	
	if (nullptr != this->m_DefaultFolder)
	{
		this->m_RootContainer.Values().Insert(
			L"DefaultFolder",
			winrt::box_value(this->m_FutureAccessList.Add(
				this->m_DefaultFolder)));
	}
	else
	{
		if (this->m_RootContainer.Values().HasKey(L"DefaultFolder"))
		{
			this->m_RootContainer.Values().Remove(L"DefaultFolder");
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

		VectorView<ITransferTask^>^ Result = nullptr;
		
		EnterCriticalSection(&this->m_TaskListUpdateCS);

		for (ITransferTask^ Task : this->m_TaskList)
		{
			if (nullptr == Task) continue;

			TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);

			this->m_TasksContainer.Values().Insert(
				TaskInternal->GuidInternal,
				TaskInternal->GetTaskConfig());
		}

		this->m_TaskList.clear();

		String^ CurrentSearchFilter = this->SearchFilter;

		bool NeedSearchFilter = (
			nullptr != CurrentSearchFilter &&
			!CurrentSearchFilter->IsEmpty());

		std::map<winrt::hstring, winrt::DownloadOperation> DownloadsList;

		for (winrt::DownloadOperation Item 
			: this->m_Downloader.GetCurrentDownloadsAsync().get())
		{
			DownloadsList.insert(std::pair<winrt::hstring, winrt::DownloadOperation>(
				winrt::to_hstring(Item.Guid()), Item));
		}	

		for (winrt::IKeyValuePair<winrt::hstring, winrt::IInspectable> Download
			: this->m_TasksContainer.Values())
		{		
			TransferTask^ Task = ref new TransferTask(
				Download.Key(),
				Download.Value().try_as<winrt::ApplicationDataCompositeValue>(),
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
	winrt::Uri SourceUriInternal =
		winrt::from_cx<winrt::Uri>(SourceUri);
	winrt::hstring DesiredFileNameInternal(
		DesiredFileName->Data(), DesiredFileName->Length());
	winrt::IStorageFolder SaveFolderInternal =
		winrt::from_cx<winrt::IStorageFolder>(SaveFolder);
	
	return M2AsyncCreate(
		[this, SourceUriInternal, DesiredFileNameInternal, SaveFolderInternal](
			IM2AsyncController^ AsyncController) -> void
	{	
		winrt::IStorageFile SaveFile = 
			SaveFolderInternal.CreateFileAsync(
				DesiredFileNameInternal,
				winrt::CreationCollisionOption::GenerateUniqueName).get();

		winrt::hstring Token = this->m_FutureAccessList.Add(
			SaveFolderInternal);

		this->m_LastusedFolder = SaveFolderInternal;
		this->m_RootContainer.Values().Insert(
			L"LastusedFolder",
			winrt::box_value(Token));
		
		winrt::DownloadOperation Operation = this->m_Downloader.CreateDownload(
			SourceUriInternal, SaveFile);

		winrt::ApplicationDataCompositeValue TaskConfig =
			winrt::ApplicationDataCompositeValue();

		TaskConfig.Insert(
			L"SourceUri",
			winrt::box_value(SourceUriInternal.RawUri()));
		TaskConfig.Insert(
			L"FileName",
			winrt::box_value(SaveFile.Name()));
		TaskConfig.Insert(
			L"SaveFolder",
			winrt::box_value(Token));
		TaskConfig.Insert(
			L"Status",
			winrt::box_value(
				static_cast<uint8_t>(TransferTaskStatus::Queued)));

		TaskConfig.Insert(
			L"BackgroundTransferGuid",
			winrt::box_value(winrt::to_hstring(Operation.Guid())));

		this->m_TasksContainer.Values().Insert(
			winrt::to_hstring(M2CreateGuid()),
			TaskConfig);

		Operation.StartAsync();
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
	TransferTask^ TaskInternal = dynamic_cast<TransferTask^>(Task);

	return M2AsyncCreate(
		[this, TaskInternal](
			IM2AsyncController^ AsyncController) -> void
	{
		EnterCriticalSection(&this->m_TaskListUpdateCS);

		switch (TaskInternal->Status)
		{
		case TransferTaskStatus::Paused:
		case TransferTaskStatus::Queued:
		case TransferTaskStatus::Running:
			TaskInternal->Cancel();
			break;
		default:
			break;
		}

		if (TransferTaskStatus::Completed != TaskInternal->Status)
		{
			try
			{
				winrt::IStorageFile SaveFile = TaskInternal->SaveFileInternal;
				if (nullptr != SaveFile)
				{
					SaveFile.DeleteAsync(
						winrt::StorageDeleteOption::PermanentDelete).get();
				}
			}
			catch (...)
			{

			}
		}

		for (size_t i = 0; i < this->m_TaskList.size(); ++i)
		{
			if (nullptr == this->m_TaskList[i]) continue;

			if (TaskInternal->Guid == this->m_TaskList[i]->Guid)
			{
				this->m_TaskList[i] = nullptr;
			}
		}
		this->m_TasksContainer.Values().Remove(TaskInternal->GuidInternal);

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
