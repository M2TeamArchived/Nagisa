/******************************************************************************
Project: Assassin
Description: Definition for the TransferManager.
File Name: TransferManager.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferManager.g.h"

#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt
{
    using Assassin::ITransferTask;
    using Assassin::TransferTaskStatus;
    using Windows::Foundation::Collections::IVectorView;
    using Windows::Foundation::IAsyncAction;
    using Windows::Foundation::IAsyncOperation;
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Uri;
    using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
    using Windows::Networking::BackgroundTransfer::DownloadOperation;
    using Windows::Storage::AccessCache::IStorageItemAccessList;
    using Windows::Storage::ApplicationDataCompositeValue;
    using Windows::Storage::ApplicationDataContainer;
    using Windows::Storage::IStorageFile;
    using Windows::Storage::IStorageFolder;
    using Windows::UI::Xaml::DispatcherTimer;
}

bool NAIsFinalTransferTaskStatus(
    winrt::TransferTaskStatus Status) noexcept;

namespace winrt::Assassin::implementation
{
    struct TransferTask : winrt::implements<
        TransferTask, winrt::ITransferTask, M2::NotifyPropertyChangedBase>
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
        /// <summary>
        /// Gets the Guid string of the task.
        /// </summary>
        winrt::hstring Guid() const;

        /// <summary>
        /// Gets the URI which to download the file.
        /// </summary>
        winrt::Uri SourceUri() const;

        /// <summary>
        /// Gets the file name which to download the file.
        /// </summary>
        winrt::hstring FileName() const;

        /// <summary>
        /// Gets the save file object which to download the file.
        /// </summary>
        winrt::IStorageFile SaveFile() const;

        /// <summary>
        /// Gets the save folder object which to download the file.
        /// </summary>
        winrt::IStorageFolder SaveFolder() const;

        /// <summary>
        /// Gets the current status of the task.
        /// </summary>
        winrt::TransferTaskStatus Status() const;

        /// <summary>
        /// Gets the total number of bytes received. This value does not include
        /// bytes received as response headers. If the task has restarted, this
        /// value may be smaller than in the previous progress report.
        /// </summary>
        uint64_t BytesReceived() const;

        /// <summary>
        /// Gets the speed of bytes received in one second.
        /// </summary>
        uint64_t BytesReceivedSpeed() const;

        /// <summary>
        /// Gets the remain time, in seconds.
        /// </summary>
        uint64_t RemainTime() const;

        /// <summary>
        /// Gets the total number of bytes of data to download. If this number
        /// is unknown, this value is set to 0.
        /// </summary>
        uint64_t TotalBytesToReceive() const;

        /// <summary>
        /// Pauses a download operation.
        /// </summary>
        void Pause();

        /// <summary>
        /// Resumes a paused download operation.
        /// </summary>
        void Resume();

        /// <summary>
        /// Cancels a download operation.
        /// </summary>
        void Cancel();
    };

    struct TransferManager : TransferManagerT<
        TransferManager, M2::NotifyPropertyChangedBase>
    {
    private:
        winrt::BackgroundDownloader m_Downloader = nullptr;
        winrt::DispatcherTimer m_UINotifyTimer = nullptr;

        M2::CCriticalSection m_TaskListUpdateCS;
        std::vector<winrt::ITransferTask> m_TaskList;

        winrt::ApplicationDataContainer m_RootContainer = nullptr;
        winrt::ApplicationDataContainer m_TasksContainer = nullptr;

        winrt::IStorageItemAccessList m_FutureAccessList = nullptr;

        winrt::IStorageFolder m_LastusedFolder = nullptr;
        winrt::IStorageFolder m_DefaultFolder = nullptr;

        uint64_t m_TotalDownloadBandwidth = 0;
        uint64_t m_TotalUploadBandwidth = 0;

        winrt::hstring m_SearchFilter;

        winrt::IAsyncAction Initialize(
            bool EnableUINotify);

        void UpdateTransferTaskStatusWithoutLock(
            bool NotifyUI);

        void UINotifyTimerTick(
            const winrt::IInspectable sender,
            const winrt::IInspectable args);

        void CreateBackgroundWorker();

    public:
        /// <summary>
        /// Creates a new TransferManager object.
        /// </summary>
        /// <param name="EnableUINotify">
        /// Enable the UI notify timer if true.
        /// </param>
        TransferManager(
            bool EnableUINotify);

        /// <summary>
        /// Destroys a TransferManager object.
        /// </summary>
        ~TransferManager();

        /// <summary>
        /// Destroys a TransferManager object.
        /// </summary>
        void Close();

        /// <summary>
        /// Gets the version of Nagisa.
        /// </summary>
        winrt::hstring Version() const;

        /// <summary>
        /// Gets the filter to use for searching the task list.
        /// </summary>
        winrt::hstring SearchFilter() const;

        /// <summary>
        /// Sets the filter to use for searching the task list.
        /// </summary>
        void SearchFilter(
            winrt::hstring const& value);

        /// <summary>
        /// Gets the last used folder.
        /// </summary>
        winrt::IStorageFolder LastusedFolder();

        /// <summary>
        /// Gets the default download folder.
        /// </summary>
        winrt::IStorageFolder DefaultFolder();

        /// <summary>
        /// Sets the default download folder.
        /// </summary>
        void DefaultFolder(
            winrt::IStorageFolder const& value);

        /// <summary>
        /// Gets the total download bandwidth.
        /// </summary>
        uint64_t TotalDownloadBandwidth() const;

        /// <summary>
        /// Gets the total upload bandwidth.
        /// </summary>
        uint64_t TotalUploadBandwidth() const;

        /// <summary>
        /// Gets the task list.
        /// </summary>
        /// <returns>
        /// Returns an object which represents the task list.
        /// </returns>
        winrt::IAsyncOperation<winrt::IVectorView<winrt::ITransferTask>>
            GetTasksAsync();

        /// <summary>
        /// Add a task to the task list.
        /// </summary>
        /// <param name="SourceUri">
        /// The source uri object of task.
        /// </param>
        /// <param name="DesiredFileName">
        /// The file name you desire.
        /// </param>
        /// <param name="SaveFolder">
        /// The object of the folder which you want to save.
        /// </param>
        /// <returns>
        /// Returns an asynchronous object used to wait.
        /// </returns>
        winrt::IAsyncAction AddTaskAsync(
            winrt::Uri const SourceUri,
            winrt::hstring const DesiredFileName,
            winrt::IStorageFolder const SaveFolder);

        /// <summary>
        /// Removes a task to the task list.
        /// </summary>
        /// <param name="Task">
        /// The task object.
        /// </param>
        /// <returns>
        /// Returns an asynchronous object used to wait.
        /// </returns>
        winrt::IAsyncAction RemoveTaskAsync(
            winrt::ITransferTask const Task);

        /// <summary>
        /// Start all tasks.
        /// </summary>
        void StartAllTasks();

        /// <summary>
        /// Pause all tasks.
        /// </summary>
        void PauseAllTasks();

        /// <summary>
        /// Clears the task list.
        /// </summary>
        void ClearTaskList();
    };
}

namespace winrt::Assassin::factory_implementation
{
    struct TransferManager : TransferManagerT<
        TransferManager, implementation::TransferManager>
    {
    };
}
