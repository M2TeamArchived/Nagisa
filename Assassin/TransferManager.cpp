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

TransferManager::TransferManager(
	bool EnableUINotify)
{
	this->m_Downloader = ref new BackgroundDownloader();

	InitializeCriticalSection(&this->m_TaskListUpdateCS);

	if (EnableUINotify)
	{
		using Windows::Foundation::EventHandler;
		using Windows::Foundation::TimeSpan;
		
		this->m_UINotifyTimer = ref new DispatcherTimer();

		TimeSpan Interval;
		Interval.Duration = 1000 * 10000; // 10,000 ticks per millisecond.

		this->m_UINotifyTimer->Interval = Interval;

		this->m_UINotifyTimer->Tick += ref new EventHandler<Object^>(
			[this](Object^ sender, Object^ args)
		{
			EnterCriticalSection(&this->m_TaskListUpdateCS);

			for (ITransferTask^ Task : this->m_TaskList)
			{
				Task->NotifyPropertyChanged();
			}

			LeaveCriticalSection(&this->m_TaskListUpdateCS);
		});

		this->m_UINotifyTimer->Start();
	}
}

TransferManager::~TransferManager()
{
	delete this->m_Downloader;
	DeleteCriticalSection(&this->m_TaskListUpdateCS);
	
	if (nullptr != this->m_UINotifyTimer)
	{
		this->m_UINotifyTimer->Stop();
		delete this->m_UINotifyTimer;
	}
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
		using Platform::Collections::VectorView;
		using Windows::Foundation::Collections::IVectorView;
		using Windows::Networking::BackgroundTransfer::DownloadOperation;

		VectorView<ITransferTask^>^ Result = nullptr;
		
		EnterCriticalSection(&this->m_TaskListUpdateCS);

		this->m_TaskList.clear();

		String^ CurrentSearchFilter = this->SearchFilter;

		bool NeedSearchFilter = (
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

		Result = ref new VectorView<ITransferTask^>(this->m_TaskList);

		LeaveCriticalSection(&this->m_TaskListUpdateCS);
	
		return Result;
	});
}

void TransferManager::AddTask(
	Uri ^ SourceUri,
	IStorageFile ^ DestinationFile)
{
	this->m_Downloader->CreateDownload(
		SourceUri, DestinationFile)->StartAsync();
}
