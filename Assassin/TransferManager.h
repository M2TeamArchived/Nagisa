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
		property String^ Version
		{
			String^ get();
		}

		property String^ SearchFilter;

		IAsyncOperation<ITransferTaskVector^>^ GetTasksAsync();

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
		TransferManager(
			bool EnableUINotify);

		virtual ~TransferManager();

		virtual property String^ Version
		{
			String^ get();
		}

		virtual property String^ SearchFilter;

		virtual IAsyncOperation<ITransferTaskVector^>^ GetTasksAsync();

		virtual void AddTask(
			Uri^ SourceUri, 
			IStorageFile^ DestinationFile);

	};
}
