/******************************************************************************
Project: Assassin
Description: Implemention for TransferManager.
File Name: TransferManager.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"

using namespace Assassin;
using namespace Platform;

TransferManager::TransferManager()
{
	this->m_Downloader = ref new BackgroundDownloader();
	
	this->m_UpdateThread = M2::CThread([this]()
	{		
		for (;;)
		{
			ULONGLONG StartTime = M2GetTickCount();

			std::vector<ITransferTask^> TaskList = this->m_TaskList;

			if (this->ExitSignal)
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
	this->ExitSignal = true;
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
		this->m_TaskList.clear();

		using Windows::Foundation::Collections::IVectorView;
		using Windows::Networking::BackgroundTransfer::DownloadOperation;

		String^ CurrentSearchFilter = this->SearchFilter;

		bool NeedSearchFilter =(
			nullptr != CurrentSearchFilter && 
			!CurrentSearchFilter->IsEmpty());

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

			this->m_TaskList.push_back(Task);
		}

		using Platform::Collections::VectorView;
		return ref new VectorView<ITransferTask^>(this->m_TaskList);
	});
}

void TransferManager::AddTask(
	Uri ^ SourceUri,
	IStorageFile ^ DestinationFile)
{
	this->m_Downloader->CreateDownload(
		SourceUri, DestinationFile)->StartAsync();
}
