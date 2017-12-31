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

IVectorView<ITransferTask^>^ TransferManager::GetTasks()
{
	using Platform::Collections::Vector;

	Vector<ITransferTask^>^ TaskList = ref new Vector<ITransferTask^>();

	using Windows::Foundation::Collections::IVectorView;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;

	IVectorView<DownloadOperation^>^ downloads = M2AsyncWait(
		this->m_Downloader->GetCurrentDownloadsAsync());

	for (DownloadOperation^ download : downloads)
	{
		TaskList->Append(ref new TransferTask(download));
	}

	return TaskList->GetView();
}

IAsyncOperation<IVectorView<ITransferTask^>^>^ TransferManager::GetTasksAsync()
{
	return M2AsyncCreate([this](IM2AsyncController^ AsyncController) -> IVectorView<ITransferTask^>^
	{
		return this->GetTasks();
	});
}

void TransferManager::AddTask(
	Uri ^ SourceUri,
	IStorageFile ^ DestinationFile)
{
	this->m_Downloader->CreateDownload(
		SourceUri, DestinationFile)->StartAsync();
}
