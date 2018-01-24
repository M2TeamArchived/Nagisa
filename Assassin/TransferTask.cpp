/******************************************************************************
Project: Assassin
Description: Implemention for TransferTask.
File Name: TransferTask.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"
#include "TransferTask.h"

using namespace Assassin;
using namespace Platform;
using namespace Windows::Networking::BackgroundTransfer;

TransferTask::TransferTask(
	DownloadOperation^ Operations,
	String^ Guid,
	Uri^ SourceUri,
	String^ FileName,
	IStorageFolder^ SaveFolder) :
	m_Operation(Operations),
	m_Guid(Guid),
	m_SourceUri(SourceUri),
	m_FileName(FileName),
	m_SaveFolder(SaveFolder)
{
	this->m_SaveFile = M2AsyncWait(
		this->m_SaveFolder->GetFileAsync(this->m_FileName));
	
	if (nullptr != this->m_Operation)
	{
		if (TransferTaskStatus::Running == this->Status)
		{
			this->m_Operation->AttachAsync();
		}
	}
}

void TransferTask::RaisePropertyChanged(
	String ^ PropertyName)
{
	using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
	this->PropertyChanged(
		this, ref new PropertyChangedEventArgs(PropertyName));
}

// Gets the Guid string of the task.
String^ TransferTask::Guid::get()
{
	return this->m_Guid;
}

// Gets the URI which to download the file.
Uri^ TransferTask::SourceUri::get()
{
	return this->m_SourceUri;
}

// Gets the file name which to download the file.
String^ TransferTask::FileName::get()
{
	return this->m_FileName;
}

// Gets the save file object which to download the file.
IStorageFile^ TransferTask::SaveFile::get()
{
	return this->m_SaveFile;
}

// Gets the save folder object which to download the file.
IStorageFolder^ TransferTask::SaveFolder::get()
{
	return this->m_SaveFolder;
}

// The current status of the task.
TransferTaskStatus TransferTask::Status::get()
{	
	if (nullptr == this->m_Operation)
		return TransferTaskStatus::Completed;
	
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

// The total number of bytes received. This value does not include 
// bytes received as response headers. If the task has restarted, 
// this value may be smaller than in the previous progress report.
uint64 TransferTask::BytesReceived::get()
{
	if (nullptr == this->m_Operation)
		return 0;

	return this->m_Operation->Progress.BytesReceived;
}

// The speed of bytes received in one second.
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

// The remain time, in seconds.
uint64 TransferTask::RemainTime::get()
{
	uint64 ReceivedSpeed = this->m_BytesReceivedSpeed;
	
	if (0 == ReceivedSpeed || 0 == this->TotalBytesToReceive)
		return static_cast<uint64>(-1);

	return (this->TotalBytesToReceive - this->BytesReceived) / ReceivedSpeed;
}

// The total number of bytes of data to download. If this number is
// unknown, this value is set to 0.
uint64 TransferTask::TotalBytesToReceive::get()
{
	if (nullptr == this->m_Operation)
		return 0;

	return this->m_Operation->Progress.TotalBytesToReceive;
}

// Pauses a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Pause()
{
	if (nullptr == this->m_Operation)
		M2ThrowPlatformException(E_FAIL);
	
	if (TransferTaskStatus::Running == this->Status)
	{
		this->m_Operation->Pause();
	}
}

// Resumes a paused download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Resume()
{
	if (nullptr == this->m_Operation)
		M2ThrowPlatformException(E_FAIL);

	if (TransferTaskStatus::Paused == this->Status)
	{
		this->m_Operation->Resume();
		this->m_Operation->AttachAsync();
	}
}

// Cancels a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Cancel()
{
	if (nullptr == this->m_Operation)
		M2ThrowPlatformException(E_FAIL);

	if (TransferTaskStatus::Canceled != this->Status)
	{
		this->m_Operation->AttachAsync()->Cancel();
	}
}
