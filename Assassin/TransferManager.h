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
	using Windows::Storage::AccessCache::IStorageItemAccessList;
	using Windows::Storage::IStorageFile;
	using Windows::Storage::IStorageFolder;
	using Windows::UI::Xaml::DispatcherTimer;

	using ITransferTaskVector = IVectorView<ITransferTask^>;

	public interface class ITransferManager
	{
		// Gets the version of Nagisa.
		property String^ Version
		{
			String^ get();
		}

		// Gets or sets the filter to use for searching the task list.
		property String^ SearchFilter;

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
		//   The function does not return a value.
		void RemoveTask(
			ITransferTask^ Task);
	};
	
	public ref class TransferManager sealed : public ITransferManager
	{
	private:
		BackgroundDownloader^ m_Downloader = nullptr;
		DispatcherTimer^ m_UINotifyTimer = nullptr;

		CRITICAL_SECTION m_TaskListUpdateCS;
		std::vector<ITransferTask^> m_TaskList;

		ApplicationDataContainer^ m_RootContainer = nullptr;
		ApplicationDataContainer^ m_TasksContainer = nullptr;

		IStorageItemAccessList^ m_StorageItemAccessList = nullptr;

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

		// Gets the version of Nagisa.
		virtual property String^ Version
		{
			String^ get();
		}

		// Gets or sets the filter to use for searching the task list.
		virtual property String^ SearchFilter;

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
		//   The function does not return a value.
		virtual void RemoveTask(
			ITransferTask^ Task);
	};
}
