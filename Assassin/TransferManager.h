/******************************************************************************
Project: Assassin
Description: Definition for the TransferManager.
File Name: TransferManager.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferManager.g.h"

#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt
{
	using Assassin::ITransferTask;
	using Assassin::TransferTaskStatus;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::IInspectable;
	using Windows::Foundation::Uri;
	using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;
	using Windows::Storage::AccessCache::IStorageItemAccessList;
	using Windows::Storage::ApplicationDataCompositeValue;
	using Windows::Storage::ApplicationDataContainer;
	using Windows::Storage::IStorageFile;
	using Windows::Storage::IStorageFolder;
	using Windows::UI::Xaml::DispatcherTimer;
}

bool NAIsFinalTransferTaskStatus(
	winrt::TransferTaskStatus Status) noexcept;

namespace winrt::Assassin::implementation
{
	struct TransferTask : winrt::implements<
		TransferTask, winrt::ITransferTask, M2::NotifyPropertyChangedBase>
	{
	private:
		winrt::DownloadOperation m_Operation = nullptr;

		winrt::ApplicationDataCompositeValue m_TaskConfig = nullptr;

		ULONGLONG m_TickCount = 0;
		winrt::hstring m_Guid;
		winrt::Uri m_SourceUri = nullptr;
		winrt::hstring m_FileName;
		winrt::IStorageFile m_SaveFile = nullptr;
		winrt::IStorageFolder m_SaveFolder = nullptr;
		winrt::TransferTaskStatus m_Status = winrt::TransferTaskStatus::Canceled;
		uint64_t m_BytesReceived = 0;
		uint64_t m_BytesReceivedSpeed = 0;
		uint64_t m_RemainTime = 0;
		uint64_t m_TotalBytesToReceive = 0;

	public:
		TransferTask() = default;

		winrt::IAsyncAction Initialize(
			winrt::hstring Guid,
			winrt::ApplicationDataCompositeValue TaskConfig,
			winrt::IStorageItemAccessList FutureAccessList,
			std::map<winrt::hstring, winrt::DownloadOperation>& DownloadOperationMap);

		void UpdateChangedProperties();
		void NotifyPropertyChanged();
		winrt::ApplicationDataCompositeValue GetTaskConfig();

	public:
		// Gets the Guid string of the task.
		winrt::hstring Guid() const;

		// Gets the URI which to download the file.
		winrt::Uri SourceUri() const;

		// Gets the file name which to download the file.
		winrt::hstring FileName() const;

		// Gets the save file object which to download the file.
		winrt::IStorageFile SaveFile() const;

		// Gets the save folder object which to download the file.
		winrt::IStorageFolder SaveFolder() const;

		// The current status of the task.
		winrt::TransferTaskStatus Status() const;

		// The total number of bytes received. This value does not include bytes 
		// received as response headers. If the task has restarted, this value may
		// be smaller than in the previous progress report.
		uint64_t BytesReceived() const;

		// The speed of bytes received in one second.
		uint64_t BytesReceivedSpeed() const;

		// The remain time, in seconds.
		uint64_t RemainTime() const;

		// The total number of bytes of data to download. If this number is 
		// unknown, this value is set to 0.
		uint64_t TotalBytesToReceive() const;

		// Pauses a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Pause();

		// Resumes a paused download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Resume();

		// Cancels a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Cancel();
	};

	struct TransferManager : TransferManagerT<
		TransferManager, M2::NotifyPropertyChangedBase>
	{
	private:
		winrt::BackgroundDownloader m_Downloader = nullptr;
		winrt::DispatcherTimer m_UINotifyTimer = nullptr;

		M2::CCriticalSection m_TaskListUpdateCS;
		std::vector<winrt::ITransferTask> m_TaskList;

		winrt::ApplicationDataContainer m_RootContainer = nullptr;
		winrt::ApplicationDataContainer m_TasksContainer = nullptr;

		winrt::IStorageItemAccessList m_FutureAccessList = nullptr;

		winrt::IStorageFolder m_LastusedFolder = nullptr;
		winrt::IStorageFolder m_DefaultFolder = nullptr;

		uint64_t m_TotalDownloadBandwidth = 0;
		uint64_t m_TotalUploadBandwidth = 0;

		winrt::hstring m_SearchFilter;

		winrt::IAsyncAction Initialize(
			bool EnableUINotify);

		void UpdateTransferTaskStatusWithoutLock(
			bool NotifyUI);

		void UINotifyTimerTick(
			const winrt::IInspectable sender,
			const winrt::IInspectable args);

		void CreateBackgroundWorker();

	public:
		// Creates a new TransferManager object.
		// Parameters:
		//   EnableUINotify: Enable the UI notify timer if true. 
		// Return value:
		//   The function does not return a value.
		TransferManager(
			bool EnableUINotify);

		// Destroys a TransferManager object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		~TransferManager();

		// Destroys a TransferManager object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Close();

		// Gets the version of Nagisa.
		winrt::hstring Version() const;

		// Gets the filter to use for searching the task list.
		winrt::hstring SearchFilter() const;

		// Sets the filter to use for searching the task list.
		void SearchFilter(
			winrt::hstring const& value);

		// Gets the last used folder.
		winrt::IStorageFolder LastusedFolder();

		// Gets the default download folder.
		winrt::IStorageFolder DefaultFolder();

		// Sets the default download folder.
		void DefaultFolder(
			winrt::IStorageFolder const& value);

		// Gets the total download bandwidth.
		uint64_t TotalDownloadBandwidth() const;

		// Gets the total upload bandwidth.
		uint64_t TotalUploadBandwidth() const;

		// Gets the task list.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns an object which represents the task list.
		winrt::IAsyncOperation<winrt::IVectorView<winrt::ITransferTask>>
			GetTasksAsync();

		// Add a task to the task list.
		// Parameters:
		//   SourceUri: The source uri object of task.
		//   DesiredFileName: The file name you desire.
		//   SaveFolder: The object of the folder which you want to save.
		// Return value:
		//   Returns an asynchronous object used to wait.
		winrt::IAsyncAction AddTaskAsync(
			winrt::Uri const SourceUri,
			winrt::hstring const DesiredFileName,
			winrt::IStorageFolder const SaveFolder);

		// Removes a task to the task list.
		// Parameters:
		//   Task: The task object. 
		// Return value:
		//   Returns an asynchronous object used to wait.
		winrt::IAsyncAction RemoveTaskAsync(
			winrt::ITransferTask const Task);

		// Start all tasks.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void StartAllTasks();

		// Pause all tasks.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void PauseAllTasks();

		// Clears the task list.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void ClearTaskList();
	};
}

namespace winrt::Assassin::factory_implementation
{
	struct TransferManager : TransferManagerT<
		TransferManager, implementation::TransferManager>
	{
	};
}
