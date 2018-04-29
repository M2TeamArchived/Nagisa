/******************************************************************************
Project: Assassin
Description: Definition for the TransferTask.
File Name: TransferTask.h
License: The MIT License
******************************************************************************/

#pragma once

#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt
{
	using Windows::Foundation::Collections::IKeyValuePair;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::TimeSpan;
	using Windows::Foundation::Uri;
	using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
	using Windows::Networking::BackgroundTransfer::BackgroundDownloadProgress;
	using Windows::Networking::BackgroundTransfer::BackgroundTransferStatus;
	using Windows::Networking::BackgroundTransfer::DownloadOperation;
	using Windows::Storage::AccessCache::IStorageItemAccessList;
	using Windows::Storage::AccessCache::StorageApplicationPermissions;
	using Windows::Storage::ApplicationData;
	using Windows::Storage::ApplicationDataContainer;
	using Windows::Storage::ApplicationDataCompositeValue;
	using Windows::Storage::ApplicationDataCreateDisposition;
	using Windows::Storage::CreationCollisionOption;
	using Windows::Storage::IStorageFile;
	using Windows::Storage::IStorageFolder;
	using Windows::Storage::StorageDeleteOption;
	using Windows::UI::Xaml::DispatcherTimer;
}

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
	using Windows::Storage::IStorageFolder;
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
		winrt::DownloadOperation m_Operation = nullptr;

		winrt::ApplicationDataCompositeValue m_TaskConfig = nullptr;

		ULONGLONG m_TickCount = 0;

		winrt::Uri m_SourceUri = nullptr;
		winrt::hstring m_FileName;
		
		winrt::IStorageFolder m_SaveFolder = nullptr;
		TransferTaskStatus m_Status = TransferTaskStatus::Canceled;
		uint64_t m_BytesReceived = 0;
		uint64_t m_BytesReceivedSpeed = 0;
		uint64_t m_RemainTime = 0;
		uint64_t m_TotalBytesToReceive = 0;

		void RaisePropertyChanged(
			String^ PropertyName);

	internal:
		winrt::hstring GuidInternal;


		winrt::IStorageFile SaveFileInternal = nullptr;

	internal:
		TransferTask(
			winrt::hstring Guid,
			winrt::ApplicationDataCompositeValue TaskConfig,
			winrt::IStorageItemAccessList FutureAccessList,
			std::map<winrt::hstring, winrt::DownloadOperation>& DownloadOperationMap);

		void UpdateChangedProperties();

		void NotifyPropertyChanged();

		winrt::ApplicationDataCompositeValue GetTaskConfig();

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
