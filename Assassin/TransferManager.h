#pragma once

#include "TransferTask.h"

namespace Assassin
{
	using Assassin::TransferTask;
	using Platform::String;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::Uri;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
	using Windows::Storage::IStorageFile;

	public interface class ITransferManager
	{
		property String^ Version
		{
			String^ get();
		}

		IVectorView<ITransferTask^>^ GetTasks();
		IAsyncOperation<IVectorView<ITransferTask^>^>^ GetTasksAsync();

		void AddTask(Uri^ SourceUri, IStorageFile^ DestinationFile);
	};
	
	public ref class TransferManager sealed : public ITransferManager
	{
	private:
		BackgroundDownloader ^ m_Downloader = nullptr;

	public:
		TransferManager();

		virtual property String^ Version
		{
			String^ get();
		}

		virtual IVectorView<ITransferTask^>^ GetTasks();
		virtual IAsyncOperation<IVectorView<ITransferTask^>^>^ GetTasksAsync();

		virtual void AddTask(Uri^ SourceUri, IStorageFile^ DestinationFile);

	};
}
