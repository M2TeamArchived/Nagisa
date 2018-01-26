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
using namespace Windows::Networking::BackgroundTransfer;

TransferTask::TransferTask(
	String^ Guid,
	ApplicationDataCompositeValue^ TaskConfig,
	M2::CFutureAccessList& FutureAccessList,
	std::map<String^, DownloadOperation^>& DownloadOperationMap) :
	m_Guid(Guid),
	m_TaskConfig(TaskConfig)
{
	this->m_SourceUri = ref new Uri(dynamic_cast<String^>(
		TaskConfig->Lookup(L"SourceUri")));
	this->m_FileName = dynamic_cast<String^>(
		TaskConfig->Lookup(L"FileName"));
	try
	{
		this->m_SaveFolder = dynamic_cast<IStorageFolder^>(M2AsyncWait(
			FutureAccessList.GetItemAsync(dynamic_cast<String^>(
				TaskConfig->Lookup(L"SaveFolderPath")))));
		if (nullptr != this->m_SaveFolder)
		{
			this->m_SaveFile = M2AsyncWait(
				this->m_SaveFolder->GetFileAsync(this->m_FileName));
		}
		this->m_LastStatus = static_cast<TransferTaskStatus>(
			dynamic_cast<Windows::Foundation::IPropertyValue^>(
				TaskConfig->Lookup(L"LastStatus"))->GetUInt8());
	}
	catch (...)
	{
		switch (this->m_LastStatus)
		{
		case TransferTaskStatus::Paused:
		case TransferTaskStatus::Queued:
		case TransferTaskStatus::Running:
			this->m_LastStatus = TransferTaskStatus::Error;
			break;
		default:
			break;
		}
	}	

	std::map<String^, DownloadOperation^>::iterator iterator =
		DownloadOperationMap.find(dynamic_cast<String^>(
			TaskConfig->Lookup(L"BackgroundTransferGuid")));
	this->m_Operation =
		(DownloadOperationMap.end() != iterator) ? iterator->second : nullptr;

	if (nullptr != this->m_Operation)
	{
		if (TransferTaskStatus::Running == this->Status)
		{
			this->m_Operation->AttachAsync();
		}
	}
	else
	{
		switch (this->m_LastStatus)
		{
		case TransferTaskStatus::Paused:
		case TransferTaskStatus::Queued:
		case TransferTaskStatus::Running:
			this->m_LastStatus = TransferTaskStatus::Error;
			break;
		default:
			break;
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

ApplicationDataCompositeValue^ TransferTask::GetTaskConfig()
{
	this->m_TaskConfig->Insert(
		L"LastStatus", 
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
	if (nullptr != this->m_Operation)
	{
		switch (this->m_Operation->Progress.Status)
		{
		case BackgroundTransferStatus::Idle:
			this->m_LastStatus = TransferTaskStatus::Queued;
			break;
		case BackgroundTransferStatus::Running:
		case BackgroundTransferStatus::PausedCostedNetwork:
		case BackgroundTransferStatus::PausedNoNetwork:
		case BackgroundTransferStatus::PausedSystemPolicy:
			this->m_LastStatus = TransferTaskStatus::Running;
			break;
		case BackgroundTransferStatus::PausedByApplication:
			this->m_LastStatus = TransferTaskStatus::Paused;
			break;
		case BackgroundTransferStatus::Completed:
			this->m_LastStatus = TransferTaskStatus::Completed;
			break;
		case BackgroundTransferStatus::Canceled:
			this->m_LastStatus = TransferTaskStatus::Canceled;
			break;
		case BackgroundTransferStatus::Error:
		default:
			this->m_LastStatus = TransferTaskStatus::Error;
			break;
		}
	}

	return this->m_LastStatus;
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
	if (TransferTaskStatus::Running == this->Status)
	{
		if (nullptr != this->m_Operation)
		{
			this->m_Operation->Pause();
		}
		else
		{
			this->m_LastStatus = TransferTaskStatus::Error;
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
			this->m_LastStatus = TransferTaskStatus::Error;
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
			this->m_LastStatus = TransferTaskStatus::Error;
		}
		break;
	default:
		break;
	}
}
