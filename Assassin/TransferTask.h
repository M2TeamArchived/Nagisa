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

	using Windows::Foundation::Uri;
	using Windows::Storage::IStorageFile;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;

	public interface class ITransferTask
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

		property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		void Pause();

		void Resume();

		void Cancel();
	};

	ref class TransferTask sealed : public ITransferTask
	{
	private:
		DownloadOperation^ m_Operation;

	internal:
		TransferTask(DownloadOperation^ Operation);

	public:
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

		virtual property uint64 TotalBytesToReceive
		{
			uint64 get();
		}

		virtual void Pause();

		virtual void Resume();

		virtual void Cancel();

	};
}
