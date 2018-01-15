/******************************************************************************
Project: Assassin
Description: Definition for the TransferTask.
File Name: TransferTask.h
License: The MIT License
******************************************************************************/

#pragma once

namespace Assassin
{
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
		property Uri^ RequestedUri
		{
			Uri^ get();
		}

		property IStorageFile^ ResultFile
		{
			IStorageFile^ get();
		}

		property TransferTaskStatus Status
		{
			TransferTaskStatus get();
		}

		property uint64 BytesReceived
		{
			uint64 get();
		}

		property uint64 BytesReceivedSpeed
		{
			uint64 get();
		}

		property uint64 RemainTime
		{
			uint64 get();
		}

		property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		void Pause();

		void Resume();

		IAsyncAction^ CancelAsync();

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
		void RaisePropertyChanged(String^ PropertyName);

	internal:
		TransferTask(DownloadOperation^ Operation);

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

		virtual property Uri^ RequestedUri
		{
			Uri^ get();
		}

		virtual property IStorageFile^ ResultFile
		{
			IStorageFile^ get();
		}

		virtual property TransferTaskStatus Status
		{
			TransferTaskStatus get();
		}

		virtual property uint64 BytesReceived
		{
			uint64 get();
		}

		virtual property uint64 BytesReceivedSpeed
		{
			uint64 get();
		}

		virtual property uint64 RemainTime
		{
			uint64 get();
		}

		virtual property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		virtual void Pause();

		virtual void Resume();

		virtual IAsyncAction^ CancelAsync();

		virtual void NotifyPropertyChanged();
	};
}
