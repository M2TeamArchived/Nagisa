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
	
	public ref class TransferManager sealed
	{
	private:
		BackgroundDownloader ^ m_Downloader = nullptr;

	public:
		TransferManager();

		property String^ Version
		{
			String^ get();
		}

		IAsyncOperation<IVectorView<TransferTask^>^>^ GetTasksAsync();

		void AddTask(Uri^ SourceUri, IStorageFile^ DestinationFile);
		
	};
}
