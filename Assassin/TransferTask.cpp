#include "pch.h"
#include "TransferManager.h"
#include "TransferTask.h"

using namespace Assassin;
using namespace Platform;
using namespace Windows::Networking::BackgroundTransfer;

void Assassin::TransferTask::RaisePropertyChanged(String ^ PropertyName)
{
	using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
	this->PropertyChanged(
		this, ref new PropertyChangedEventArgs(PropertyName));
}

TransferTask::TransferTask(DownloadOperation^ Operations)
	: m_Operation(Operations)
{
	if (TransferTaskStatus::Running == this->Status)
	{
		this->m_Operation->AttachAsync();
	}
}

Uri^ TransferTask::RequestedUri::get()
{
	return this->m_Operation->RequestedUri;
}

IStorageFile^ TransferTask::ResultFile::get()
{
	return this->m_Operation->ResultFile;
}

TransferTaskStatus TransferTask::Status::get()
{	
	switch (this->m_Operation->Progress.Status)
	{
	case BackgroundTransferStatus::Idle:
		return TransferTaskStatus::Queued;

	case BackgroundTransferStatus::Running:
		return TransferTaskStatus::Running;

	case BackgroundTransferStatus::PausedByApplication:
	case BackgroundTransferStatus::PausedCostedNetwork:
	case BackgroundTransferStatus::PausedNoNetwork:
	case BackgroundTransferStatus::PausedSystemPolicy:
		return TransferTaskStatus::Paused;

	case BackgroundTransferStatus::Completed:
		return TransferTaskStatus::Completed;

	case BackgroundTransferStatus::Canceled:
		return TransferTaskStatus::Canceled;

	case BackgroundTransferStatus::Error:		
	default:
		return TransferTaskStatus::Error;
	}
}

uint64 TransferTask::BytesReceived::get()
{
	return this->m_Operation->Progress.BytesReceived;
}

uint64 TransferTask::BytesReceivedSpeed::get()
{	
	if (0 != this->m_LastUpdated)
	{	
		uint64 DeltaBytesReceived =
			this->BytesReceived - this->m_LastBytesReceived;
		ULONGLONG DeltaPassedTime = 
			M2GetTickCount() - this->m_LastUpdated;

		this->m_BytesReceivedSpeed = 
			DeltaBytesReceived * 1000 / DeltaPassedTime;
	}
	
	this->m_LastUpdated = M2GetTickCount();
	this->m_LastBytesReceived = this->BytesReceived;

	return this->m_BytesReceivedSpeed;
}

uint64 TransferTask::RemainTime::get()
{
	uint64 ReceivedSpeed = this->m_BytesReceivedSpeed;
	
	if (0 == ReceivedSpeed)
	{
		return static_cast<uint64>(-1);
	}
	else
	{
		return (this->TotalBytesToReceive - this->BytesReceived) / ReceivedSpeed;
	}
}

uint64 TransferTask::TotalBytesToReceive::get()
{
	return this->m_Operation->Progress.TotalBytesToReceive;
}

void TransferTask::Pause()
{
	if (TransferTaskStatus::Running == this->Status)
	{
		this->m_Operation->Pause();
	}
}

void TransferTask::Resume()
{
	if (TransferTaskStatus::Paused == this->Status)
	{
		this->m_Operation->Resume();
		this->m_Operation->AttachAsync();
	}
}

void TransferTask::Cancel()
{
	if (TransferTaskStatus::Canceled != this->Status)
	{
		this->m_Operation->AttachAsync()->Cancel();
	}
}

void Assassin::TransferTask::NotifyPropertyChanged()
{
	this->RaisePropertyChanged(L"BytesReceived");
	this->RaisePropertyChanged(L"BytesReceivedSpeed");
	this->RaisePropertyChanged(L"RemainTime");
	this->RaisePropertyChanged(L"TotalBytesToReceive");
}