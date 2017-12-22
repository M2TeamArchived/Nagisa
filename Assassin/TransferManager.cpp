#include "pch.h"
#include "TransferManager.h"

using namespace Assassin;
using namespace Platform;

TransferManager::TransferManager()
{
	using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
	this->m_Downloader = ref new BackgroundDownloader();
}

String^ TransferManager::Version::get()
{
	return NAGISA_VERSION_STRING;
}

IAsyncOperation<IVectorView<TransferTask^>^>^ TransferManager::GetTasksAsync()
{
	using concurrency::create_async;

	return create_async([this]() ->IVectorView<TransferTask^>^
	{
		using Platform::Collections::Vector;

		Vector<TransferTask^>^ result = ref new Vector<TransferTask^>();

		using Windows::Foundation::Collections::IVectorView;
		using Windows::Networking::BackgroundTransfer::DownloadOperation;

		IVectorView<DownloadOperation^>^ downloads = M2AsyncWait(
			this->m_Downloader->GetCurrentDownloadsAsync());

		for (DownloadOperation^ download : downloads)
		{
			result->Append(ref new TransferTask(download));
		}

		return result->GetView();
	});
}

void Assassin::TransferManager::AddTask(Uri ^ SourceUri, IStorageFile ^ DestinationFile)
{
	this->m_Downloader->CreateDownload(SourceUri, DestinationFile);
		//->Pause()
		//->Resume()
		//->AttachAsync()
		//->StartAsync()
}
