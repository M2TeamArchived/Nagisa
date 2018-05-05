/******************************************************************************
Project: Assassin
Description: Definition for the TransferManager.
File Name: TransferManager.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferTask.h"

namespace winrt::Assassin::implementation
{
	struct TransferManager : TransferManagerT<TransferManager>
	{
	private:
		winrt::BackgroundDownloader m_Downloader = nullptr;
		winrt::DispatcherTimer m_UINotifyTimer = nullptr;

		CRITICAL_SECTION m_TaskListUpdateCS;
		std::vector<winrt::ITransferTask> m_TaskList;

		winrt::ApplicationDataContainer m_RootContainer = nullptr;
		winrt::ApplicationDataContainer m_TasksContainer = nullptr;

		winrt::IStorageItemAccessList m_FutureAccessList = nullptr;

		winrt::IStorageFolder m_LastusedFolder = nullptr;
		winrt::IStorageFolder m_DefaultFolder = nullptr;

		uint64_t m_TotalDownloadBandwidth = 0;
		uint64_t m_TotalUploadBandwidth = 0;

		winrt::hstring m_SearchFilter;

		winrt::event<winrt::PropertyChangedEventHandler> m_PropertyChanged;

		void RaisePropertyChanged(
			winrt::hstring PropertyName);

	public:
		winrt::event_token PropertyChanged(
			winrt::PropertyChangedEventHandler const& value);
		void PropertyChanged(
			winrt::event_token const& token);

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
		virtual ~TransferManager();

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
			winrt::Uri const& SourceUri,
			winrt::param::hstring const& DesiredFileName,
			winrt::IStorageFolder const& SaveFolder);

		// Removes a task to the task list.
		// Parameters:
		//   Task: The task object. 
		// Return value:
		//   Returns an asynchronous object used to wait.
		winrt::IAsyncAction RemoveTaskAsync(
			winrt::ITransferTask const& Task);

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
