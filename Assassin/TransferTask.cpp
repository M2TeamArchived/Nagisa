#include "pch.h"
#include "TransferManager.h"
#include "TransferTask.h"

using namespace Assassin;
using namespace Platform;
using namespace Windows::Networking::BackgroundTransfer;

TransferTask::TransferTask(DownloadOperation^ Operations)
{
	this->m_Operation = Operations;
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

uint64 TransferTask::TotalBytesToReceive::get()
{
	return this->m_Operation->Progress.TotalBytesToReceive;
}