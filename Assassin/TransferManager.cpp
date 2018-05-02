/******************************************************************************
Project: Assassin
Description: Implementation for TransferManager.
File Name: TransferManager.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"

void TransferManager::RaisePropertyChanged(
	winrt::hstring PropertyName)
{
	this->m_PropertyChanged(
		*this, winrt::PropertyChangedEventArgs(PropertyName));
}

winrt::event_token TransferManager::PropertyChanged(
	winrt::PropertyChangedEventHandler const& value)
{
	return this->m_PropertyChanged.add(value);
}

void TransferManager::PropertyChanged(
	winrt::event_token const& token)
{
	this->m_PropertyChanged.remove(token);
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
			this->m_LastusedFolder =
				M2AsyncWait(this->m_FutureAccessList.GetFolderAsync(
					winrt::unbox_value<winrt::hstring>(
						this->m_RootContainer.Values().Lookup(
							L"LastusedFolder"))));
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
			this->m_DefaultFolder =
				M2AsyncWait(this->m_FutureAccessList.GetFolderAsync(
					winrt::unbox_value<winrt::hstring>(
						this->m_RootContainer.Values().Lookup(
							L"DefaultFolder"))));
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

			for (winrt::ITransferTask Task : this->m_TaskList)
			{
				if (nullptr == Task) continue;

				TransferTask& TaskInternal = *Task.try_as<TransferTask>();

				this->m_TasksContainer.Values().Insert(
					TaskInternal.Guid(),
					TaskInternal.GetTaskConfig());

				TaskInternal.UpdateChangedProperties();
				TaskInternal.NotifyPropertyChanged();

				this->m_TotalDownloadBandwidth += TaskInternal.BytesReceivedSpeed();
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
winrt::hstring TransferManager::Version() const
{
	return NAGISA_VERSION_STRING;
}

// Gets the filter to use for searching the task list.
winrt::hstring TransferManager::SearchFilter() const
{
	return this->m_SearchFilter;
}

// Sets the filter to use for searching the task list.
void TransferManager::SearchFilter(
	winrt::hstring const& value)
{
	this->m_SearchFilter = value;
}

// Gets the last used folder.
winrt::IStorageFolder TransferManager::LastusedFolder()
{	
	return this->m_LastusedFolder;
}

// Gets the default download folder.
winrt::IStorageFolder TransferManager::DefaultFolder()
{
	return this->m_DefaultFolder;
}

// Sets the default download folder.
void TransferManager::DefaultFolder(
	winrt::IStorageFolder const& value)
{
	this->m_DefaultFolder = value;

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
uint64_t TransferManager::TotalDownloadBandwidth() const
{
	return this->m_TotalDownloadBandwidth;
}

// Gets the total upload bandwidth.
uint64_t TransferManager::TotalUploadBandwidth() const
{
	return this->m_TotalUploadBandwidth;
}

std::vector<winrt::ITransferTask> x{ nullptr,nullptr };

// Gets the task list.
// Parameters:
//   The function does not have parameters.
// Return value:
//   Returns an object which represents the task list.
winrt::IAsyncOperation<winrt::IVectorView<winrt::ITransferTask>>
	TransferManager::GetTasksAsync()
{
	winrt::IVectorView<winrt::ITransferTask> Result = nullptr;

	EnterCriticalSection(&this->m_TaskListUpdateCS);

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		TransferTask& TaskInternal = *Task.try_as<TransferTask>();

		this->m_TasksContainer.Values().Insert(
			TaskInternal.Guid(),
			TaskInternal.GetTaskConfig());
	}

	this->m_TaskList.clear();

	winrt::hstring CurrentSearchFilter = this->SearchFilter();

	bool NeedSearchFilter = !CurrentSearchFilter.empty();

	std::map<winrt::hstring, winrt::DownloadOperation> DownloadsList;

	for (winrt::DownloadOperation Item
		: M2AsyncWait(this->m_Downloader.GetCurrentDownloadsAsync()))
	{
		DownloadsList.insert(std::pair<winrt::hstring, winrt::DownloadOperation>(
			winrt::to_hstring(Item.Guid()), Item));
	}

	for (winrt::IKeyValuePair<winrt::hstring, winrt::IInspectable> Download
		: this->m_TasksContainer.Values())
	{
		winrt::ITransferTask Task = winrt::make<TransferTask>(
			Download.Key(),
			Download.Value().try_as<winrt::ApplicationDataCompositeValue>(),
			this->m_FutureAccessList,
			DownloadsList);

		if (NeedSearchFilter)
		{
			if (!M2FindSubString(
				Task.FileName(), CurrentSearchFilter, true))
			{
				continue;
			}
		}

		this->m_TaskList.push_back(Task);
	}

	/*winrt::IVector<winrt::ITransferTask> x = winrt::IVector<winrt::ITransferTask>();
	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		x.Append(Task);
	}*/
	Result = winrt::com_array<winrt::ITransferTask>(this->m_TaskList);


	Result = winrt::single_threaded_vector(std::move(this->m_TaskList)).GetView();
		
		//winrt::make<winrt::impl::input_vector<std::vector<winrt::ITransferTask>, std::vector<std::vector<winrt::ITransferTask>, std::allocator<std::vector<winrt::ITransferTask>>>>>(this->m_TaskList).GetView();

	//((IUnknown*)winrt::get_abi(Result))->AddRef();

	LeaveCriticalSection(&this->m_TaskListUpdateCS);

	// Asynchronous call return.
	co_return Result;
}

// Add a task to the task list.
// Parameters:
//   SourceUri: The source uri object of task.
//   DesiredFileName: The file name you desire.
//   SaveFolder: The object of the folder which you want to save.
// Return value:
//   Returns an asynchronous object used to wait.
winrt::IAsyncAction TransferManager::AddTaskAsync(
	winrt::Uri const& SourceUri,
	winrt::param::hstring const& DesiredFileName,
	winrt::IStorageFolder const& SaveFolder)
{
	winrt::IStorageFile SaveFile = M2AsyncWait(SaveFolder.CreateFileAsync(
		DesiredFileName,
		winrt::CreationCollisionOption::GenerateUniqueName));

	winrt::hstring Token = this->m_FutureAccessList.Add(SaveFolder);

	this->m_LastusedFolder = SaveFolder;
	this->m_RootContainer.Values().Insert(
		L"LastusedFolder",
		winrt::box_value(Token));

	winrt::DownloadOperation Operation = this->m_Downloader.CreateDownload(
		SourceUri, SaveFile);

	winrt::ApplicationDataCompositeValue TaskConfig =
		winrt::ApplicationDataCompositeValue();

	TaskConfig.Insert(
		L"SourceUri",
		winrt::box_value(SourceUri.RawUri()));
	TaskConfig.Insert(
		L"FileName",
		winrt::box_value(SaveFile.Name()));
	TaskConfig.Insert(
		L"SaveFolder",
		winrt::box_value(Token));
	TaskConfig.Insert(
		L"Status",
		winrt::box_value(
			static_cast<uint8_t>(winrt::TransferTaskStatus::Queued)));

	TaskConfig.Insert(
		L"BackgroundTransferGuid",
		winrt::box_value(winrt::to_hstring(Operation.Guid())));

	this->m_TasksContainer.Values().Insert(
		winrt::to_hstring(M2CreateGuid()),
		TaskConfig);

	Operation.StartAsync();

	// Asynchronous call return.
	co_return;
}

// Removes a task to the task list.
// Parameters:
//   Task: The task object. 
// Return value:
//   Returns an asynchronous object used to wait.
winrt::IAsyncAction TransferManager::RemoveTaskAsync(
	winrt::ITransferTask const& Task)
{
	EnterCriticalSection(&this->m_TaskListUpdateCS);

	switch (Task.Status())
	{
	case winrt::TransferTaskStatus::Paused:
	case winrt::TransferTaskStatus::Queued:
	case winrt::TransferTaskStatus::Running:
		Task.Cancel();
		break;
	default:
		break;
	}

	if (winrt::TransferTaskStatus::Completed != Task.Status())
	{
		try
		{
			winrt::IStorageFile SaveFile = Task.SaveFile();
			if (nullptr != SaveFile)
			{
				M2AsyncWait(SaveFile.DeleteAsync(
					winrt::StorageDeleteOption::PermanentDelete));
			}
		}
		catch (...)
		{

		}
	}

	for (size_t i = 0; i < this->m_TaskList.size(); ++i)
	{
		if (nullptr == this->m_TaskList[i]) continue;

		if (Task.Guid() == this->m_TaskList[i].Guid())
		{
			this->m_TaskList[i] = nullptr;
		}
	}
	this->m_TasksContainer.Values().Remove(Task.Guid());

	LeaveCriticalSection(&this->m_TaskListUpdateCS);

	// Asynchronous call return.
	co_return;
}

// Start all tasks.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::StartAllTasks()
{
	EnterCriticalSection(&this->m_TaskListUpdateCS);

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task.Resume();
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

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task.Pause();
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

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		switch (Task.Status())
		{
		case winrt::TransferTaskStatus::Canceled:
		case winrt::TransferTaskStatus::Completed:
		case winrt::TransferTaskStatus::Error:
			this->RemoveTaskAsync(Task);
			break;
		default:
			break;
		}
	}

	LeaveCriticalSection(&this->m_TaskListUpdateCS);
}
