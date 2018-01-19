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

void TransferTask::RaisePropertyChanged(
	String ^ PropertyName)
{
	using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
	this->PropertyChanged(
		this, ref new PropertyChangedEventArgs(PropertyName));
}

TransferTask::TransferTask(
	DownloadOperation^ Operations)
	: m_Operation(Operations)
{
	if (TransferTaskStatus::Running == this->Status)
	{
		this->m_Operation->AttachAsync();
	}
}

// Gets the URI from which to download the file.
Uri^ TransferTask::RequestedUri::get()
{
	return this->m_Operation->RequestedUri;
}

// Returns the IStorageFile object provided by the caller when creating
// the task.
IStorageFile^ TransferTask::ResultFile::get()
{
	return this->m_Operation->ResultFile;
}

// The current status of the task.
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

// The total number of bytes received. This value does not include 
// bytes received as response headers. If the task has restarted, 
// this value may be smaller than in the previous progress report.
uint64 TransferTask::BytesReceived::get()
{
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
	
	if (0 == ReceivedSpeed)
	{
		return static_cast<uint64>(-1);
	}
	else
	{
		return (this->TotalBytesToReceive - this->BytesReceived) / ReceivedSpeed;
	}
}

// The total number of bytes of data to download. If this number is
// unknown, this value is set to 0.
uint64 TransferTask::TotalBytesToReceive::get()
{
	return this->m_Operation->Progress.TotalBytesToReceive;
}

// Pauses a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Pause()
{
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
//   Returns an object used to wait.
IAsyncAction^ TransferTask::CancelAsync()
{
	return M2AsyncCreate(
		[this](IM2AsyncController^ AsyncController) -> void
	{
		if (TransferTaskStatus::Canceled != this->Status)
		{
			auto Operation = this->m_Operation->AttachAsync();		
			Operation->Cancel();

			try
			{
				M2AsyncWait(Operation);
			}
			catch (...)
			{

			}

			// Wait for the BackgroundTransfer to update the task list.
			SleepEx(2000, FALSE);
		}
	});
}

// Send property changed event to the UI.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void Assassin::TransferTask::NotifyPropertyChanged()
{
	this->RaisePropertyChanged(L"Status");
	this->RaisePropertyChanged(L"BytesReceived");
	this->RaisePropertyChanged(L"BytesReceivedSpeed");
	this->RaisePropertyChanged(L"RemainTime");
	this->RaisePropertyChanged(L"TotalBytesToReceive");
}