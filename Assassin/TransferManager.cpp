#include "pch.h"
#include "TransferManager.h"

using namespace Assassin;
using namespace Platform;

ULONGLONG M2GetTickCount()
{
	LARGE_INTEGER Frequency = { 0 }, PerformanceCount = { 0 };

	if (QueryPerformanceFrequency(&Frequency))
	{
		if (QueryPerformanceCounter(&PerformanceCount))
		{
			return (PerformanceCount.QuadPart * 1000 / Frequency.QuadPart);
		}
	}

	return GetTickCount64();
}

TransferManager::TransferManager()
{
	this->m_Downloader = ref new BackgroundDownloader();
	this->m_TaskList = ref new Vector<ITransferTask^>();
	
	this->m_UpdateThread = M2::CThread([this]()
	{		
		for (;;)
		{
			ULONGLONG StartTime = M2GetTickCount();

			Vector<ITransferTask^>^ TaskList = this->m_TaskList;

			if (nullptr == TaskList)
				break;

			for (ITransferTask^ Task : TaskList)
			{
				M2ExecuteOnUIThread([Task]()
				{
					Task->NotifyPropertyChanged();
				});
			}

			DWORD RunningTime = static_cast<DWORD>(
				M2GetTickCount() - StartTime);

			if (RunningTime < 1000)
			{
				SleepEx(1000 - RunningTime, FALSE);
			}		
		}
	});

}

TransferManager::~TransferManager()
{
	this->m_TaskList = nullptr;
	this->m_UpdateThread.Wait();
}

String^ TransferManager::Version::get()
{
	return NAGISA_VERSION_STRING;
}

ITransferTaskVector^ TransferManager::GetTasks()
{
	this->m_TaskList->Clear();

	using Windows::Foundation::Collections::IVectorView;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;

	IVectorView<DownloadOperation^>^ downloads = M2AsyncWait(
		this->m_Downloader->GetCurrentDownloadsAsync());

	for (DownloadOperation^ download : downloads)
	{
		this->m_TaskList->Append(ref new TransferTask(download));
	}
	
	return this->m_TaskList->GetView();
}

IAsyncOperation<ITransferTaskVector^>^ TransferManager::GetTasksAsync()
{
	return M2AsyncCreate(
		[this](IM2AsyncController^ AsyncController) -> ITransferTaskVector^
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
