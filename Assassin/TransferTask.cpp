/******************************************************************************
Project: Assassin
Description: Implementation for TransferTask.
File Name: TransferTask.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"
#include "TransferTask.h"

TransferTask::TransferTask(
	winrt::hstring Guid,
	winrt::ApplicationDataCompositeValue TaskConfig,
	winrt::IStorageItemAccessList FutureAccessList,
	std::map<winrt::hstring, winrt::DownloadOperation>& DownloadOperationMap) :
	m_Guid(Guid),
	m_TaskConfig(TaskConfig)
{
	this->m_SourceUri = winrt::Uri(
		winrt::unbox_value<winrt::hstring>(TaskConfig.Lookup(L"SourceUri")));
	this->m_FileName =
		winrt::unbox_value<winrt::hstring>(TaskConfig.Lookup(L"FileName"));
	this->m_Status = static_cast<winrt::TransferTaskStatus>(
		winrt::unbox_value<uint8_t>(TaskConfig.Lookup(L"Status")));
	try
	{
		this->m_SaveFolder = M2AsyncWait(FutureAccessList.GetFolderAsync(
			winrt::unbox_value<winrt::hstring>(
				TaskConfig.Lookup(L"SaveFolder"))));
		if (nullptr != this->m_SaveFolder)
		{
			this->m_SaveFile = M2AsyncWait(this->m_SaveFolder.GetFileAsync(
				this->m_FileName));
		}

		std::map<winrt::hstring, winrt::DownloadOperation>::iterator iterator =
			DownloadOperationMap.find(winrt::unbox_value<winrt::hstring>(
				TaskConfig.Lookup(L"BackgroundTransferGuid")));
		this->m_Operation = (DownloadOperationMap.end() != iterator)
			? iterator->second : nullptr;
		if (nullptr == this->m_Operation)
		{
			M2ThrowPlatformException(E_FAIL);
		}

		winrt::BackgroundDownloadProgress Progress = this->m_Operation.Progress();
		this->m_BytesReceived = Progress.BytesReceived;
		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		if (winrt::TransferTaskStatus::Running == this->m_Status)
		{
			this->m_Operation.AttachAsync();
		}
	}
	catch (...)
	{
		switch (this->m_Status)
		{
		case winrt::TransferTaskStatus::Paused:
		case winrt::TransferTaskStatus::Queued:
		case winrt::TransferTaskStatus::Running:
			this->m_Status = winrt::TransferTaskStatus::Error;
			break;
		default:
			break;
		}
	}
}

void TransferTask::UpdateChangedProperties()
{
	if (nullptr != this->m_Operation)
	{
		winrt::BackgroundDownloadProgress Progress = this->m_Operation.Progress();

		switch (Progress.Status)
		{
		case winrt::BackgroundTransferStatus::Idle:
			this->m_Status = winrt::TransferTaskStatus::Queued;
			break;
		case winrt::BackgroundTransferStatus::Running:
		case winrt::BackgroundTransferStatus::PausedCostedNetwork:
		case winrt::BackgroundTransferStatus::PausedNoNetwork:
		case winrt::BackgroundTransferStatus::PausedSystemPolicy:
			this->m_Status = winrt::TransferTaskStatus::Running;
			break;
		case winrt::BackgroundTransferStatus::PausedByApplication:
			this->m_Status = winrt::TransferTaskStatus::Paused;
			break;
		case winrt::BackgroundTransferStatus::Completed:
			this->m_Status = winrt::TransferTaskStatus::Completed;
			break;
		case winrt::BackgroundTransferStatus::Canceled:
			this->m_Status = winrt::TransferTaskStatus::Canceled;
			break;
		case winrt::BackgroundTransferStatus::Error:
		default:
			this->m_Status = winrt::TransferTaskStatus::Error;
			break;
		}
		if (winrt::TransferTaskStatus::Running != this->m_Status)
			return;

		ULONGLONG LastTickCount = this->m_TickCount;
		this->m_TickCount = M2GetTickCount();

		uint64_t LastBytesReceived = this->m_BytesReceived;
		this->m_BytesReceived = Progress.BytesReceived;

		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		uint64_t DeltaBytesReceived = this->m_BytesReceived - LastBytesReceived;
		ULONGLONG DeltaPassedTime = this->m_TickCount - LastTickCount;
		this->m_BytesReceivedSpeed =
			DeltaBytesReceived * 1000 / DeltaPassedTime;

		if (0 == this->m_BytesReceivedSpeed ||
			0 == this->m_TotalBytesToReceive)
		{
			this->m_RemainTime = static_cast<uint64_t>(-1);
		}
		else
		{
			uint64_t RemainBytesToReceive =
				this->m_TotalBytesToReceive - this->m_BytesReceived;
			this->m_RemainTime =
				RemainBytesToReceive / this->m_BytesReceivedSpeed;
		}
	}
}

void TransferTask::RaisePropertyChanged(
	winrt::hstring PropertyName)
{
	this->m_PropertyChanged(
		*this, winrt::PropertyChangedEventArgs(PropertyName));
}

winrt::event_token TransferTask::PropertyChanged(
	winrt::PropertyChangedEventHandler const& value)
{
	return this->m_PropertyChanged.add(value);
}

void TransferTask::PropertyChanged(
	winrt::event_token const& token)
{
	this->m_PropertyChanged.remove(token);
}

void TransferTask::NotifyPropertyChanged()
{
	if (nullptr != this->m_Operation)
	{
		this->RaisePropertyChanged(L"Status");
		if (winrt::TransferTaskStatus::Running == this->m_Status)
		{
			this->RaisePropertyChanged(L"BytesReceived");
			this->RaisePropertyChanged(L"TotalBytesToReceive");
			this->RaisePropertyChanged(L"BytesReceivedSpeed");
			this->RaisePropertyChanged(L"RemainTime");
		}
	}
}

winrt::ApplicationDataCompositeValue TransferTask::GetTaskConfig()
{
	this->m_TaskConfig.Insert(
		L"Status",
		winrt::box_value(static_cast<uint32_t>(this->m_Status)));

	return this->m_TaskConfig;
}

// Gets the Guid string of the task.
winrt::hstring TransferTask::Guid() const
{
	return this->m_Guid;
}

// Gets the URI which to download the file.
winrt::Uri TransferTask::SourceUri() const
{
	return this->m_SourceUri;
}

// Gets the file name which to download the file.
winrt::hstring TransferTask::FileName() const
{
	return this->m_FileName.data();
}

// Gets the save file object which to download the file.
winrt::IStorageFile TransferTask::SaveFile() const
{
	return this->m_SaveFile;
}

// Gets the save folder object which to download the file.
winrt::IStorageFolder TransferTask::SaveFolder() const
{
	return this->m_SaveFolder;
}

// The current status of the task.
winrt::TransferTaskStatus TransferTask::Status() const
{
	return this->m_Status;
}

// The total number of bytes received. This value does not include bytes 
// received as response headers. If the task has restarted, this value may
// be smaller than in the previous progress report.
uint64_t TransferTask::BytesReceived() const
{
	return this->m_BytesReceived;
}

// The speed of bytes received in one second.
uint64_t TransferTask::BytesReceivedSpeed() const
{
	return this->m_BytesReceivedSpeed;
}

// The remain time, in seconds.
uint64_t TransferTask::RemainTime() const
{
	return this->m_RemainTime;
}

// The total number of bytes of data to download. If this number is
// unknown, this value is set to 0.
uint64_t TransferTask::TotalBytesToReceive() const
{
	return this->m_TotalBytesToReceive;
}

// Pauses a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Pause()
{
	if (winrt::TransferTaskStatus::Running == this->m_Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.Pause();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

// Resumes a paused download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Resume()
{
	if (winrt::TransferTaskStatus::Paused == this->m_Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.Resume();
			this->m_Operation.AttachAsync();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
	}
}

// Cancels a download operation.
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
void TransferTask::Cancel()
{
	switch (this->m_Status)
	{
	case winrt::TransferTaskStatus::Paused:
	case winrt::TransferTaskStatus::Queued:
	case winrt::TransferTaskStatus::Running:
		if (nullptr != this->m_Operation)
		{
			this->m_Operation.AttachAsync().Cancel();
		}
		else
		{
			this->m_Status = winrt::TransferTaskStatus::Error;
		}
		break;
	default:
		break;
	}
}
