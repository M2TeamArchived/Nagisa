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
	using Windows::Storage::IStorageFile;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;
	using Windows::UI::Xaml::Data::INotifyPropertyChanged;
	using Windows::UI::Xaml::Data::PropertyChangedEventHandler;

	public interface class ITransferTask : INotifyPropertyChanged
	{
		// Gets the URI from which to download the file.
		property Uri^ RequestedUri
		{
			Uri^ get();
		}

		// Returns the IStorageFile object provided by the caller when creating
		// the task.
		property IStorageFile^ ResultFile
		{
			IStorageFile^ get();
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
		//   Returns an object used to wait.
		IAsyncAction^ CancelAsync();

		// Send property changed event to the UI.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		void NotifyPropertyChanged();
	};

	ref class TransferTask sealed : public ITransferTask
	{
	private:
		DownloadOperation^ m_Operation = nullptr;

		ULONGLONG m_LastUpdated = 0;
		uint64 m_LastBytesReceived = 0;
		uint64 m_BytesReceivedSpeed = 0;

	protected:
		void RaisePropertyChanged(
			String^ PropertyName);

	internal:
		TransferTask(
			DownloadOperation^ Operation);

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

		// Gets the URI from which to download the file.
		virtual property Uri^ RequestedUri
		{
			Uri^ get();
		}

		// Returns the IStorageFile object provided by the caller when creating
		// the task.
		virtual property IStorageFile^ ResultFile
		{
			IStorageFile^ get();
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
		//   Returns an object used to wait.
		virtual IAsyncAction^ CancelAsync();

		// Send property changed event to the UI.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   The function does not return a value.
		virtual void NotifyPropertyChanged();
	};
}
