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

	public ref class TransferTask sealed
	{
	private:
		DownloadOperation^ m_Operation;

	internal:
		TransferTask(DownloadOperation^ Operation);

	public:
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
}
