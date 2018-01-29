/******************************************************************************
Project: Assassin
Description: Definition for the TransferManager.
File Name: TransferManager.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferTask.h"

#include <vector>

namespace Assassin
{
	using Platform::String;
	using Platform::Collections::Vector;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::Uri;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::Networking::BackgroundTransfer::BackgroundDownloader;	
	using Windows::Storage::ApplicationDataContainer;
	using Windows::Storage::IStorageFile;
	using Windows::Storage::IStorageFolder;
	using Windows::UI::Xaml::Data::INotifyPropertyChanged;
	using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
	using Windows::UI::Xaml::DispatcherTimer;

	using ITransferTaskVector = IVectorView<ITransferTask^>;

	public interface class ITransferManager : INotifyPropertyChanged
	{
		// Gets the version of Nagisa.
		property String^ Version
		{
			String^ get();
		}

		// Gets or sets the filter to use for searching the task list.
		property String^ SearchFilter;

		// Gets the last used folder.
		property IStorageFolder^ LastusedFolder
		{
			IStorageFolder^ get();
		}

		// Gets or sets the default download folder.
		property IStorageFolder^ DefaultFolder;

		// Gets the total download bandwidth.
		property uint64 TotalDownloadBandwidth
		{
			uint64 get();
		}

		// Gets the total upload bandwidth.
		property uint64 TotalUploadBandwidth
		{
			uint64 get();
		}

		// Gets the task list.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns an object which represents the task list.
		IAsyncOperation<ITransferTaskVector^>^ GetTasksAsync();

		// Add a task to the task list.
		// Parameters:
		//   SourceUri: The source uri object of task.
		//   DesiredFileName: The file name you desire.
		//   SaveFolder: The object of the folder which you want to save.
		// Return value:
		//   Returns an asynchronous object used to wait.
		IAsyncAction^ AddTaskAsync(
			Uri^ SourceUri,
			String^ DesiredFileName,
			IStorageFolder^ SaveFolder);

		// Removes a task to the task list.
		// Parameters:
		//   Task: The task object. 
		// Return value:
		//   Returns an asynchronous object used to wait.
		IAsyncAction^ RemoveTaskAsync(
			ITransferTask^ Task);

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

	ref class TransferManager sealed : public ITransferManager
	{
	private:
		BackgroundDownloader^ m_Downloader = nullptr;
		DispatcherTimer^ m_UINotifyTimer = nullptr;

		CRITICAL_SECTION m_TaskListUpdateCS;
		std::vector<ITransferTask^> m_TaskList;

		ApplicationDataContainer^ m_RootContainer = nullptr;
		ApplicationDataContainer^ m_TasksContainer = nullptr;

		M2::CFutureAccessList m_FutureAccessList;

		IStorageFolder^ m_LastusedFolder = nullptr;
		IStorageFolder^ m_DefaultFolder = nullptr;

		uint64 m_TotalDownloadBandwidth = 0;
		uint64 m_TotalUploadBandwidth = 0;

	internal:
		void RaisePropertyChanged(
			String^ PropertyName);

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
		virtual ~TransferManager();

		virtual event PropertyChangedEventHandler^ PropertyChanged;

		// Gets the version of Nagisa.
		virtual property String^ Version
		{
			String^ get();
		}

		// Gets or sets the filter to use for searching the task list.
		virtual property String^ SearchFilter;

		// Gets the last used folder.
		virtual property IStorageFolder^ LastusedFolder
		{
			IStorageFolder^ get();
		}

		// Gets or sets the default download folder.
		virtual property IStorageFolder^ DefaultFolder
		{
			IStorageFolder^ get();
			void set(IStorageFolder^ value);
		}

		// Gets the total download bandwidth.
		virtual property uint64 TotalDownloadBandwidth
		{
			uint64 get();
		}

		// Gets the total upload bandwidth.
		virtual property uint64 TotalUploadBandwidth
		{
			uint64 get();
		}

		// Gets the task list.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns an object which represents the task list.
		virtual IAsyncOperation<ITransferTaskVector^>^ GetTasksAsync();

		// Add a task to the task list.
		// Parameters:
		//   SourceUri: The source uri object of task.
		//   DesiredFileName: The file name you desire.
		//   SaveFolder: The object of the folder which you want to save.
		// Return value:
		//   Returns an asynchronous object used to wait.
		virtual IAsyncAction^ AddTaskAsync(
			Uri^ SourceUri,
			String^ DesiredFileName,
			IStorageFolder^ SaveFolder);

		// Removes a task to the task list.
		// Parameters:
		//   Task: The task object. 
		// Return value:
		//   Returns an asynchronous object used to wait.
		virtual IAsyncAction^ RemoveTaskAsync(
			ITransferTask^ Task);

		// Start all tasks.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void StartAllTasks();

		// Pause all tasks.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void PauseAllTasks();

		// Clears the task list.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void ClearTaskList();
	};

	public ref class TransferManagerFactory sealed
	{
	public:
		// Creates a new TransferManager object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns a new TransferManager object.
		static ITransferManager^ CreateInstance();
	};
}
