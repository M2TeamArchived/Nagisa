/******************************************************************************
Project: Assassin
Description: Implementation for TransferTask.
File Name: TransferTask.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManager.h"
#include "TransferTask.h"

using namespace Assassin;
using namespace Platform;

using Windows::Networking::BackgroundTransfer::BackgroundDownloadProgress;
using Windows::Networking::BackgroundTransfer::BackgroundTransferStatus;

TransferTask::TransferTask(
	String^ Guid,
	ApplicationDataCompositeValue^ TaskConfig,
	IStorageItemAccessList^ FutureAccessList,
	std::map<String^, DownloadOperation^>& DownloadOperationMap) :
	m_Guid(Guid),
	m_TaskConfig(TaskConfig)
{
	this->m_SourceUri = ref new Uri(dynamic_cast<String^>(
		TaskConfig->Lookup(L"SourceUri")));
	this->m_FileName = dynamic_cast<String^>(
		TaskConfig->Lookup(L"FileName"));
	this->m_Status = static_cast<TransferTaskStatus>(
		dynamic_cast<Windows::Foundation::IPropertyValue^>(
			TaskConfig->Lookup(L"Status"))->GetUInt8());
	try
	{
		this->m_SaveFolder = dynamic_cast<IStorageFolder^>(M2AsyncWait(
			FutureAccessList->GetItemAsync(dynamic_cast<String^>(
				TaskConfig->Lookup(L"SaveFolder")))));
		if (nullptr != this->m_SaveFolder)
		{
			this->m_SaveFile = M2AsyncWait(
				this->m_SaveFolder->GetFileAsync(this->m_FileName));
		}

		std::map<String^, DownloadOperation^>::iterator iterator =
			DownloadOperationMap.find(dynamic_cast<String^>(
				TaskConfig->Lookup(L"BackgroundTransferGuid")));
		this->m_Operation = (DownloadOperationMap.end() != iterator)
			? iterator->second : nullptr;
		if (nullptr == this->m_Operation)
		{
			M2ThrowPlatformException(E_FAIL);
		}

		BackgroundDownloadProgress Progress = this->m_Operation->Progress;
		this->m_BytesReceived = Progress.BytesReceived;
		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		if (TransferTaskStatus::Running == this->Status)
		{
			this->m_Operation->AttachAsync();
		}
	}
	catch (...)
	{
		switch (this->m_Status)
		{
		case TransferTaskStatus::Paused:
		case TransferTaskStatus::Queued:
		case TransferTaskStatus::Running:
			this->m_Status = TransferTaskStatus::Error;
			break;
		default:
			break;
		}
	}
}

void TransferTask::UpdateChangedProperties()
{
	using Windows::Networking::BackgroundTransfer::BackgroundDownloadProgress;

	if (nullptr != this->m_Operation)
	{
		switch (this->m_Operation->Progress.Status)
		{
		case BackgroundTransferStatus::Idle:
			this->m_Status = TransferTaskStatus::Queued;
			break;
		case BackgroundTransferStatus::Running:
		case BackgroundTransferStatus::PausedCostedNetwork:
		case BackgroundTransferStatus::PausedNoNetwork:
		case BackgroundTransferStatus::PausedSystemPolicy:
			this->m_Status = TransferTaskStatus::Running;
			break;
		case BackgroundTransferStatus::PausedByApplication:
			this->m_Status = TransferTaskStatus::Paused;
			break;
		case BackgroundTransferStatus::Completed:
			this->m_Status = TransferTaskStatus::Completed;
			break;
		case BackgroundTransferStatus::Canceled:
			this->m_Status = TransferTaskStatus::Canceled;
			break;
		case BackgroundTransferStatus::Error:
		default:
			this->m_Status = TransferTaskStatus::Error;
			break;
		}
		if (TransferTaskStatus::Running != this->m_Status)
			return;

		BackgroundDownloadProgress Progress = this->m_Operation->Progress;

		ULONGLONG LastTickCount = this->m_TickCount;
		this->m_TickCount = M2GetTickCount();

		uint64 LastBytesReceived = this->m_BytesReceived;
		this->m_BytesReceived = Progress.BytesReceived;

		this->m_TotalBytesToReceive = Progress.TotalBytesToReceive;

		uint64 DeltaBytesReceived = this->m_BytesReceived - LastBytesReceived;
		ULONGLONG DeltaPassedTime = this->m_TickCount - LastTickCount;
		this->m_BytesReceivedSpeed =
			DeltaBytesReceived * 1000 / DeltaPassedTime;

		if (0 == this->m_BytesReceivedSpeed ||
			0 == this->m_TotalBytesToReceive)
		{
			this->m_RemainTime = static_cast<uint64>(-1);
		}
		else
		{
			uint64 RemainBytesToReceive =
				this->m_TotalBytesToReceive - this->m_BytesReceived;
			this->m_RemainTime =
				RemainBytesToReceive / this->m_BytesReceivedSpeed;
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

void TransferTask::NotifyPropertyChanged()
{
	using Windows::Networking::BackgroundTransfer::BackgroundDownloadProgress;
	
	if (nullptr != this->m_Operation)
	{
		this->RaisePropertyChanged(L"Status");
		if (TransferTaskStatus::Running == this->m_Status)
		{
			this->RaisePropertyChanged(L"BytesReceived");
			this->RaisePropertyChanged(L"TotalBytesToReceive");
			this->RaisePropertyChanged(L"BytesReceivedSpeed");
			this->RaisePropertyChanged(L"RemainTime");
		}	
	}
}

ApplicationDataCompositeValue^ TransferTask::GetTaskConfig()
{
	this->m_TaskConfig->Insert(
		L"Status", 
		Windows::Foundation::PropertyValue::CreateUInt8(
			static_cast<uint8>(this->Status)));

	return this->m_TaskConfig;
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
	return this->m_Status;
}

// The total number of bytes received. This value does not include 
// bytes received as response headers. If the task has restarted, 
// this value may be smaller than in the previous progress report.
uint64 TransferTask::BytesReceived::get()
{
	return this->m_BytesReceived;
}

// The speed of bytes received in one second.
uint64 TransferTask::BytesReceivedSpeed::get()
{	
	return this->m_BytesReceivedSpeed;
}

// The remain time, in seconds.
uint64 TransferTask::RemainTime::get()
{
	return this->m_RemainTime;
}

// The total number of bytes of data to download. If this number is
// unknown, this value is set to 0.
uint64 TransferTask::TotalBytesToReceive::get()
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
	if (TransferTaskStatus::Running == this->Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation->Pause();
		}
		else
		{
			this->m_Status = TransferTaskStatus::Error;
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
	if (TransferTaskStatus::Paused == this->Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation->Resume();
			this->m_Operation->AttachAsync();
		}
		else
		{
			this->m_Status = TransferTaskStatus::Error;
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
	switch (this->Status)
	{
	case TransferTaskStatus::Paused:
	case TransferTaskStatus::Queued:
	case TransferTaskStatus::Running:
		if (nullptr != this->m_Operation)
		{
			this->m_Operation->AttachAsync()->Cancel();
		}
		else
		{
			this->m_Status = TransferTaskStatus::Error;
		}
		break;
	default:
		break;
	}
}
