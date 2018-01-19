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
	using Windows::Storage::IStorageFile;
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
		//   DestinationFile: The destination file object of task.
		// Return value:
		//   The function does not return a value.
		void AddTask(
			Uri^ SourceUri,
			IStorageFile^ DestinationFile);
	};
	
	public ref class TransferManager sealed : public ITransferManager
	{
	private:
		BackgroundDownloader^ m_Downloader = nullptr;
		DispatcherTimer^ m_UINotifyTimer = nullptr;

		CRITICAL_SECTION m_TaskListUpdateCS;
		std::vector<ITransferTask^> m_TaskList;

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
		//   DestinationFile: The destination file object of task.
		// Return value:
		//   The function does not return a value.
		virtual void AddTask(
			Uri^ SourceUri, 
			IStorageFile^ DestinationFile);

	};
}
