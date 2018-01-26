/******************************************************************************
Project: Assassin
Description: Definition for the TransferTask.
File Name: TransferTask.h
License: The MIT License
******************************************************************************/

#pragma once

namespace Assassin
{
	// The status of task.
	public enum class TransferTaskStatus
	{
		Canceled,
		Completed,
		Error,
		Paused,
		Queued,
		Running
	};

	using Platform::String;
	using Windows::Foundation::Uri;
	using Windows::Foundation::IAsyncAction;
	using Windows::Storage::ApplicationDataCompositeValue;
	using Windows::Storage::IStorageFile;
	using Windows::Storage::IStorageFolder;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;
	using Windows::UI::Xaml::Data::INotifyPropertyChanged;
	using Windows::UI::Xaml::Data::PropertyChangedEventHandler;

	public interface class ITransferTask : INotifyPropertyChanged
	{
		// Gets the Guid string of the task.
		property String^ Guid
		{
			String^ get();
		}
			
		// Gets the URI which to download the file.
		property Uri^ SourceUri
		{
			Uri^ get();
		}

		// Gets the file name which to download the file.
		property String^ FileName
		{
			String^ get();
		}

		// Gets the save file object which to download the file.
		property IStorageFile^ SaveFile
		{
			IStorageFile^ get();
		}

		// Gets the save folder object which to download the file.
		property IStorageFolder^ SaveFolder
		{
			IStorageFolder^ get();
		}

		// The current status of the task.
		property TransferTaskStatus Status
		{
			TransferTaskStatus get();
		}

		// The total number of bytes received. This value does not include 
		// bytes received as response headers. If the task has restarted, 
		// this value may be smaller than in the previous progress report.
		property uint64 BytesReceived
		{
			uint64 get();
		}

		// The speed of bytes received in one second.
		property uint64 BytesReceivedSpeed
		{
			uint64 get();
		}

		// The remain time, in seconds.
		property uint64 RemainTime
		{
			uint64 get();
		}

		// The total number of bytes of data to download. If this number is
		// unknown, this value is set to 0.
		property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		// Pauses a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Pause();

		// Resumes a paused download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Resume();

		// Cancels a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void Cancel();
	};

	ref class TransferTask sealed : public ITransferTask
	{
	private:
		DownloadOperation^ m_Operation = nullptr;

		ULONGLONG m_LastUpdated = 0;
		uint64 m_LastBytesReceived = 0;
		uint64 m_BytesReceivedSpeed = 0;

		String^ m_Guid = nullptr;
		Uri^ m_SourceUri = nullptr;
		String^ m_FileName = nullptr;
		IStorageFile^ m_SaveFile = nullptr;
		IStorageFolder^ m_SaveFolder = nullptr;
		TransferTaskStatus m_LastStatus = TransferTaskStatus::Canceled;

		ApplicationDataCompositeValue^ m_TaskConfig = nullptr;

	internal:
		TransferTask(
			String^ Guid,
			ApplicationDataCompositeValue^ TaskConfig,
			M2::CFutureAccessList& FutureAccessList,
			std::map<String^, DownloadOperation^>& DownloadOperationMap);

		void RaisePropertyChanged(
			String^ PropertyName);

		ApplicationDataCompositeValue^ GetTaskConfig();

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

		// Gets the Guid string of the task.
		virtual property String^ Guid
		{
			String^ get();
		}

		// Gets the URI which to download the file.
		virtual property Uri^ SourceUri
		{
			Uri^ get();
		}

		// Gets the file name which to download the file.
		virtual property String^ FileName
		{
			String^ get();
		}

		// Gets the save file object which to download the file.
		virtual property IStorageFile^ SaveFile
		{
			IStorageFile^ get();
		}

		// Gets the save folder object which to download the file.
		virtual property IStorageFolder^ SaveFolder
		{
			IStorageFolder^ get();
		}

		// The current status of the task.
		virtual property TransferTaskStatus Status
		{
			TransferTaskStatus get();
		}

		// The total number of bytes received. This value does not include 
		// bytes received as response headers. If the task has restarted, 
		// this value may be smaller than in the previous progress report.
		virtual property uint64 BytesReceived
		{
			uint64 get();
		}

		// The speed of bytes received in one second.
		virtual property uint64 BytesReceivedSpeed
		{
			uint64 get();
		}

		// The remain time, in seconds.
		virtual property uint64 RemainTime
		{
			uint64 get();
		}

		// The total number of bytes of data to download. If this number is
		// unknown, this value is set to 0.
		virtual property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		// Pauses a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void Pause();

		// Resumes a paused download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void Resume();

		// Cancels a download operation.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void Cancel();
	};
}
