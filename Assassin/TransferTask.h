/******************************************************************************
Project: Assassin
Description: Definition for the TransferTask.
File Name: TransferTask.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferManager.g.h"

#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>
#include <winrt\Windows.UI.Xaml.Data.h>

namespace winrt
{
	using Assassin::ITransferManager;
	using Assassin::ITransferTask;
	using Assassin::TransferTaskStatus;
	using Windows::Foundation::Collections::IKeyValuePair;
	using Windows::Foundation::Collections::IVector;
	using Windows::Foundation::Collections::IVectorView;
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::IClosable;
	using Windows::Foundation::IInspectable;
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
	using Windows::UI::Xaml::Data::INotifyPropertyChanged;
	using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
	using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
	using Windows::UI::Xaml::DispatcherTimer;
}

bool NAIsFinalTransferTaskStatus(
	winrt::TransferTaskStatus Status) noexcept;

namespace winrt::Assassin::implementation
{
	struct TransferTask : winrt::implements<
		TransferTask, winrt::ITransferTask, winrt::INotifyPropertyChanged>
	{
	private:
		winrt::DownloadOperation m_Operation = nullptr;

		winrt::ApplicationDataCompositeValue m_TaskConfig = nullptr;

		ULONGLONG m_TickCount = 0;
		winrt::hstring m_Guid;
		winrt::Uri m_SourceUri = nullptr;
		winrt::hstring m_FileName;
		winrt::IStorageFile m_SaveFile = nullptr;
		winrt::IStorageFolder m_SaveFolder = nullptr;
		winrt::TransferTaskStatus m_Status = winrt::TransferTaskStatus::Canceled;
		uint64_t m_BytesReceived = 0;
		uint64_t m_BytesReceivedSpeed = 0;
		uint64_t m_RemainTime = 0;
		uint64_t m_TotalBytesToReceive = 0;

		void RaisePropertyChanged(
			winrt::hstring PropertyName);

		winrt::event<winrt::PropertyChangedEventHandler> m_PropertyChanged;

	public:
		TransferTask() = default;

		winrt::IAsyncAction Initialize(
			winrt::hstring Guid,
			winrt::ApplicationDataCompositeValue TaskConfig,
			winrt::IStorageItemAccessList FutureAccessList,
			std::map<winrt::hstring, winrt::DownloadOperation>& DownloadOperationMap);

		void UpdateChangedProperties();
		void NotifyPropertyChanged();
		winrt::ApplicationDataCompositeValue GetTaskConfig();

	public:
		winrt::event_token PropertyChanged(
			winrt::PropertyChangedEventHandler const& value);
		void PropertyChanged(
			winrt::event_token const& token);

		// Gets the Guid string of the task.
		winrt::hstring Guid() const;

		// Gets the URI which to download the file.
		winrt::Uri SourceUri() const;

		// Gets the file name which to download the file.
		winrt::hstring FileName() const;

		// Gets the save file object which to download the file.
		winrt::IStorageFile SaveFile() const;

		// Gets the save folder object which to download the file.
		winrt::IStorageFolder SaveFolder() const;

		// The current status of the task.
		winrt::TransferTaskStatus Status() const;

		// The total number of bytes received. This value does not include bytes 
		// received as response headers. If the task has restarted, this value may
		// be smaller than in the previous progress report.
		uint64_t BytesReceived() const;

		// The speed of bytes received in one second.
		uint64_t BytesReceivedSpeed() const;

		// The remain time, in seconds.
		uint64_t RemainTime() const;

		// The total number of bytes of data to download. If this number is 
		// unknown, this value is set to 0.
		uint64_t TotalBytesToReceive() const;

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

}
