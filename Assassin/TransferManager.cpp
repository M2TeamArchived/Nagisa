/******************************************************************************
Project: Assassin
Description: Implementation for TransferManager.
File Name: TransferManager.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"

#include "M2BindableVectorView.h"

using namespace winrt::Assassin::implementation;

bool NAIsFinalTransferTaskStatus(
	winrt::TransferTaskStatus Status) noexcept
{
	return (
		winrt::TransferTaskStatus::Canceled == Status ||
		winrt::TransferTaskStatus::Completed == Status ||
		winrt::TransferTaskStatus::Error == Status);
}

winrt::IAsyncAction TransferTask::Initialize(
	winrt::hstring Guid,
	winrt::ApplicationDataCompositeValue TaskConfig,
	winrt::IStorageItemAccessList FutureAccessList,
	std::map<winrt::hstring, winrt::DownloadOperation>& DownloadOperationMap)
{
	this->m_Guid = Guid;
	this->m_TaskConfig = TaskConfig;

	this->m_SourceUri = winrt::Uri(winrt::unbox_value<winrt::hstring>(
		TaskConfig.Lookup(L"SourceUri")));
	this->m_FileName = winrt::unbox_value<winrt::hstring>(
		TaskConfig.Lookup(L"FileName"));
	this->m_Status = static_cast<winrt::TransferTaskStatus>(
		winrt::unbox_value<uint32_t>(TaskConfig.Lookup(L"Status")));
	try
	{
		this->m_SaveFolder = co_await FutureAccessList.GetFolderAsync(
			winrt::unbox_value<winrt::hstring>(
				TaskConfig.Lookup(L"SaveFolder")));
		if (nullptr != this->m_SaveFolder)
		{
			this->m_SaveFile = co_await this->m_SaveFolder.GetFileAsync(
				this->m_FileName);
		}

		std::map<winrt::hstring, winrt::DownloadOperation>::iterator iterator =
			DownloadOperationMap.find(winrt::unbox_value<winrt::hstring>(
				TaskConfig.Lookup(L"BackgroundTransferGuid")));
		if (DownloadOperationMap.end() != iterator)
		{
			this->m_Operation = iterator->second;
		}
		else
		{
			winrt::throw_hresult(E_FAIL);
		}

		winrt::BackgroundDownloadProgress Progress = this->m_Operation.Progress();
		this->m_BytesReceived = Progress.BytesReceived;
		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		if (winrt::TransferTaskStatus::Running == this->m_Status)
		{
			this->m_Operation.AttachAsync();
		}
	}
	catch (...)
	{
		if (!NAIsFinalTransferTaskStatus(this->m_Status))
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

void TransferTask::UpdateChangedProperties()
{
	if (nullptr != this->m_Operation)
	{
		winrt::BackgroundDownloadProgress Progress = this->m_Operation.Progress();

		switch (Progress.Status)
		{
		case winrt::BackgroundTransferStatus::Idle:
			this->m_Status = winrt::TransferTaskStatus::Queued;
			break;
		case winrt::BackgroundTransferStatus::Running:
		case winrt::BackgroundTransferStatus::PausedCostedNetwork:
		case winrt::BackgroundTransferStatus::PausedNoNetwork:
		case winrt::BackgroundTransferStatus::PausedSystemPolicy:
			this->m_Status = winrt::TransferTaskStatus::Running;
			break;
		case winrt::BackgroundTransferStatus::PausedByApplication:
			this->m_Status = winrt::TransferTaskStatus::Paused;
			break;
		case winrt::BackgroundTransferStatus::Completed:
			this->m_Status = winrt::TransferTaskStatus::Completed;
			break;
		case winrt::BackgroundTransferStatus::Canceled:
			this->m_Status = winrt::TransferTaskStatus::Canceled;
			break;
		case winrt::BackgroundTransferStatus::Error:
		default:
			this->m_Status = winrt::TransferTaskStatus::Error;
			break;
		}
		if (winrt::TransferTaskStatus::Running != this->m_Status)
			return;

		ULONGLONG LastTickCount = this->m_TickCount;
		this->m_TickCount = M2GetTickCount();

		uint64_t LastBytesReceived = this->m_BytesReceived;
		this->m_BytesReceived = Progress.BytesReceived;

		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		uint64_t DeltaBytesReceived = this->m_BytesReceived - LastBytesReceived;
		ULONGLONG DeltaPassedTime = this->m_TickCount - LastTickCount;
		this->m_BytesReceivedSpeed =
			DeltaBytesReceived * 1000 / DeltaPassedTime;

		if (0 == this->m_BytesReceivedSpeed ||
			0 == this->m_TotalBytesToReceive)
		{
			this->m_RemainTime = static_cast<uint64_t>(-1);
		}
		else
		{
			uint64_t RemainBytesToReceive =
				this->m_TotalBytesToReceive - this->m_BytesReceived;
			this->m_RemainTime =
				RemainBytesToReceive / this->m_BytesReceivedSpeed;
		}
	}
}

void TransferTask::NotifyPropertyChanged()
{
	if (nullptr != this->m_Operation)
	{
		this->RaisePropertyChanged(L"Status");
		if (winrt::TransferTaskStatus::Running == this->m_Status)
		{
			this->RaisePropertyChanged(L"BytesReceived");
			this->RaisePropertyChanged(L"TotalBytesToReceive");
			this->RaisePropertyChanged(L"BytesReceivedSpeed");
			this->RaisePropertyChanged(L"RemainTime");
		}
	}
}

winrt::ApplicationDataCompositeValue TransferTask::GetTaskConfig()
{
	this->m_TaskConfig.Insert(
		L"Status",
		winrt::box_value(static_cast<uint32_t>(this->m_Status)));

	return this->m_TaskConfig;
}

// Gets the Guid string of the task.
winrt::hstring TransferTask::Guid() const
{
	return this->m_Guid;
}

// Gets the URI which to download the file.
winrt::Uri TransferTask::SourceUri() const
{
	return this->m_SourceUri;
}

// Gets the file name which to download the file.
winrt::hstring TransferTask::FileName() const
{
	return this->m_FileName;
}

// Gets the save file object which to download the file.
winrt::IStorageFile TransferTask::SaveFile() const
{
	return this->m_SaveFile;
}

// Gets the save folder object which to download the file.
winrt::IStorageFolder TransferTask::SaveFolder() const
{
	return this->m_SaveFolder;
}

// The current status of the task.
winrt::TransferTaskStatus TransferTask::Status() const
{
	return this->m_Status;
}

// The total number of bytes received. This value does not include bytes 
// received as response headers. If the task has restarted, this value may
// be smaller than in the previous progress report.
uint64_t TransferTask::BytesReceived() const
{
	return this->m_BytesReceived;
}

// The speed of bytes received in one second.
uint64_t TransferTask::BytesReceivedSpeed() const
{
	return this->m_BytesReceivedSpeed;
}

// The remain time, in seconds.
uint64_t TransferTask::RemainTime() const
{
	return this->m_RemainTime;
}

// The total number of bytes of data to download. If this number is
// unknown, this value is set to 0.
uint64_t TransferTask::TotalBytesToReceive() const
{
	return this->m_TotalBytesToReceive;
}

// Pauses a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Pause()
{
	if (winrt::TransferTaskStatus::Running == this->m_Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.Pause();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

// Resumes a paused download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Resume()
{
	if (winrt::TransferTaskStatus::Paused == this->m_Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.Resume();
			this->m_Operation.AttachAsync();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

// Cancels a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Cancel()
{
	if (!NAIsFinalTransferTaskStatus(this->m_Status))
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.AttachAsync().Cancel();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

winrt::IAsyncAction TransferManager::Initialize(
	bool EnableUINotify)
{
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	this->m_Downloader = winrt::BackgroundDownloader();

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
				co_await this->m_FutureAccessList.GetFolderAsync(
					winrt::unbox_value<winrt::hstring>(
						this->m_RootContainer.Values().Lookup(
							L"LastusedFolder")));
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
				co_await this->m_FutureAccessList.GetFolderAsync(
					winrt::unbox_value<winrt::hstring>(
						this->m_RootContainer.Values().Lookup(
							L"DefaultFolder")));
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
			{ this, &TransferManager::UINotifyTimerTick });

		this->m_UINotifyTimer.Start();
	}
}

void TransferManager::UpdateTransferTaskStatusWithoutLock(
	bool NotifyUI)
{
	if (NotifyUI)
	{
		this->m_TotalDownloadBandwidth = 0;
		this->m_TotalUploadBandwidth = 0;
	}
	
	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		TransferTask& TaskInternal = *Task.try_as<TransferTask>();

		this->m_TasksContainer.Values().Insert(
			TaskInternal.Guid(),
			TaskInternal.GetTaskConfig());

		if (NotifyUI)
		{
			TaskInternal.UpdateChangedProperties();
			TaskInternal.NotifyPropertyChanged();

			this->m_TotalDownloadBandwidth +=
				TaskInternal.BytesReceivedSpeed();
			this->m_TotalUploadBandwidth += 0;
		}
	}

	if (NotifyUI)
	{
		this->RaisePropertyChanged(L"TotalDownloadBandwidth");
		this->RaisePropertyChanged(L"TotalUploadBandwidth");
	}	
}

void TransferManager::UINotifyTimerTick(
	const winrt::IInspectable sender,
	const winrt::IInspectable args)
{
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	this->UpdateTransferTaskStatusWithoutLock(true);
}

// Creates a new TransferManager object.
// Parameters:
//   EnableUINotify: Enable the UI notify timer if true. 
// Return value:
//   The function does not return a value.
TransferManager::TransferManager(
	bool EnableUINotify)
{
	this->Initialize(EnableUINotify);
}

// Destroys a TransferManager object.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
TransferManager::~TransferManager()
{
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	if (nullptr != this->m_UINotifyTimer)
	{
		this->m_UINotifyTimer.Stop();
	}
}

// Destroys a TransferManager object.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::Close()
{
	delete this;
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

// Gets the task list.
// Parameters:
//   The function does not have parameters.
// Return value:
//   Returns an object which represents the task list.
winrt::IAsyncOperation<winrt::IVectorView<winrt::ITransferTask>>
	TransferManager::GetTasksAsync()
{
	co_await winrt::resume_background();

	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	this->UpdateTransferTaskStatusWithoutLock(false);

	this->m_TaskList.clear();

	winrt::hstring CurrentSearchFilter = this->SearchFilter();

	bool NeedSearchFilter = !CurrentSearchFilter.empty();

	std::map<winrt::hstring, winrt::DownloadOperation> DownloadsList;

	for (winrt::DownloadOperation Item
		: co_await this->m_Downloader.GetCurrentDownloadsAsync())
	{
		DownloadsList.insert(std::pair<winrt::hstring, winrt::DownloadOperation>(
			winrt::to_hstring(Item.Guid()), Item));
	}

	for (winrt::IKeyValuePair<winrt::hstring, winrt::IInspectable> Download
		: this->m_TasksContainer.Values())
	{
		winrt::ITransferTask Task = winrt::make<TransferTask>();

		co_await Task.try_as<TransferTask>()->Initialize(
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

	// Asynchronous call return.
	co_return winrt::make<M2::BindableVectorView<winrt::ITransferTask>>(
		this->m_TaskList);
}

// Add a task to the task list.
// Parameters:
//   SourceUri: The source uri object of task.
//   DesiredFileName: The file name you desire.
//   SaveFolder: The object of the folder which you want to save.
// Return value:
//   Returns an asynchronous object used to wait.
winrt::IAsyncAction TransferManager::AddTaskAsync(
	winrt::Uri const SourceUri,
	winrt::hstring const DesiredFileName,
	winrt::IStorageFolder const SaveFolder)
{
	co_await winrt::resume_background();
	
	winrt::IStorageFile SaveFile = co_await SaveFolder.CreateFileAsync(
		DesiredFileName,
		winrt::CreationCollisionOption::GenerateUniqueName);

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
			static_cast<uint32_t>(winrt::TransferTaskStatus::Queued)));

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
	winrt::ITransferTask const Task)
{
	co_await winrt::resume_background();
	
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	if (!NAIsFinalTransferTaskStatus(Task.Status()))
	{
		Task.Cancel();
	}

	if (winrt::TransferTaskStatus::Completed != Task.Status())
	{
		try
		{
			winrt::IStorageFile SaveFile = Task.SaveFile();
			if (nullptr != SaveFile)
			{
				co_await SaveFile.DeleteAsync(
					winrt::StorageDeleteOption::PermanentDelete);
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
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task.Resume();
	}
}

// Pause all tasks.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::PauseAllTasks()
{
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (nullptr == Task) continue;

		Task.Pause();
	}
}

// Clears the task list.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferManager::ClearTaskList()
{
	M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

	for (winrt::ITransferTask Task : this->m_TaskList)
	{
		if (NAIsFinalTransferTaskStatus(Task.Status()))
		{
			this->RemoveTaskAsync(Task);
		}
	}
}
