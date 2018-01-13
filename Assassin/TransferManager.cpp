#include "pch.h"
#include "TransferManager.h"

using namespace Assassin;
using namespace Platform;

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

IAsyncOperation<ITransferTaskVector^>^ TransferManager::GetTasksAsync()
{
	return M2AsyncCreate(
		[this](IM2AsyncController^ AsyncController)
			-> ITransferTaskVector^
	{
		this->m_TaskList->Clear();

		using Windows::Foundation::Collections::IVectorView;
		using Windows::Networking::BackgroundTransfer::DownloadOperation;

		String^ CurrentSearchFilter = this->SearchFilter;

		bool NeedSearchFilter =
			(CurrentSearchFilter == nullptr || CurrentSearchFilter->IsEmpty());

		IVectorView<DownloadOperation^>^ downloads = M2AsyncWait(
			this->m_Downloader->GetCurrentDownloadsAsync());

		for (DownloadOperation^ download : downloads)
		{
			ITransferTask^ Task = ref new TransferTask(download);

			if (NeedSearchFilter)
			{
				if (!M2FindSubString(
					Task->ResultFile->Name,
					CurrentSearchFilter,
					true))
				{
					continue;
				}
			}

			this->m_TaskList->Append(Task);
		}

		return this->m_TaskList->GetView();
	});
}

void TransferManager::AddTask(
	Uri ^ SourceUri,
	IStorageFile ^ DestinationFile)
{
	this->m_Downloader->CreateDownload(
		SourceUri, DestinationFile)->StartAsync();
}
