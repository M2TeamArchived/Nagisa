/*
 * PROJECT:   Assassin
 * FILE:      TransferManager.h
 * PURPOSE:   Definition for the TransferManager
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#include "TransferManager.g.h"

#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Networking.BackgroundTransfer.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.AccessCache.h>
#include <winrt\Windows.UI.Xaml.h>

namespace winrt::Assassin::implementation
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

    bool NAIsFinalTransferTaskStatus(
        TransferTaskStatus Status) noexcept;

    struct TransferTask : implements<
        TransferTask, ITransferTask, M2::NotifyPropertyChangedBase>
    {
    private:
        DownloadOperation m_Operation = nullptr;

        ApplicationDataCompositeValue m_TaskConfig = nullptr;

        ULONGLONG m_TickCount = 0;
        hstring m_Guid;
        Uri m_SourceUri = nullptr;
        hstring m_FileName;
        IStorageFile m_SaveFile = nullptr;
        IStorageFolder m_SaveFolder = nullptr;
        TransferTaskStatus m_Status = TransferTaskStatus::Canceled;
        uint64_t m_BytesReceived = 0;
        uint64_t m_BytesReceivedSpeed = 0;
        uint64_t m_RemainTime = 0;
        uint64_t m_TotalBytesToReceive = 0;

    public:
        TransferTask() = default;

        IAsyncAction Initialize(
            hstring Guid,
            ApplicationDataCompositeValue TaskConfig,
            IStorageItemAccessList FutureAccessList,
            std::map<hstring, DownloadOperation>& DownloadOperationMap);

        void UpdateChangedProperties();
        void NotifyPropertyChanged();
        ApplicationDataCompositeValue GetTaskConfig();

    public:
        // Gets the Guid string of the task.
        hstring Guid() const;

        // Gets the URI which to download the file.
        Uri SourceUri() const;

        // Gets the file name which to download the file.
        hstring FileName() const;
      
        // Gets the save file object which to download the file.
        IStorageFile SaveFile() const;
   
        // Gets the save folder object which to download the file. 
        IStorageFolder SaveFolder() const;

        // Gets the current status of the task.
        TransferTaskStatus Status() const;

        // Gets the total number of bytes received. This value does not include
        // bytes received as response headers. If the task has restarted, this
        // value may be smaller than in the previous progress report.
        uint64_t BytesReceived() const;

        // Gets the speed of bytes received in one second.
        uint64_t BytesReceivedSpeed() const;

        // Gets the remain time, in seconds.
        uint64_t RemainTime() const;

        // Gets the total number of bytes of data to download. If this number
        // is unknown, this value is set to 0.
        uint64_t TotalBytesToReceive() const;

        /**
         * Pauses a download operation.
         */
        void Pause();
        
        /**
         * Resumes a paused download operation.
         */
        void Resume();
    
        /**
         * Cancels a download operation.
         */
        void Cancel();
    };

    struct TransferManager : TransferManagerT<
        TransferManager, M2::NotifyPropertyChangedBase>
    {
    private:
        BackgroundDownloader m_Downloader = nullptr;
        DispatcherTimer m_UINotifyTimer = nullptr;

        M2::CCriticalSection m_TaskListUpdateCS;
        std::vector<ITransferTask> m_TaskList;

        ApplicationDataContainer m_RootContainer = nullptr;
        ApplicationDataContainer m_TasksContainer = nullptr;

        IStorageItemAccessList m_FutureAccessList = nullptr;

        IStorageFolder m_LastusedFolder = nullptr;
        IStorageFolder m_DefaultFolder = nullptr;

        uint64_t m_TotalDownloadBandwidth = 0;
        uint64_t m_TotalUploadBandwidth = 0;

        hstring m_SearchFilter;

        IAsyncAction Initialize(
            bool EnableUINotify);

        void UpdateTransferTaskStatusWithoutLock(
            bool NotifyUI);

        void UINotifyTimerTick(
            const IInspectable sender,
            const IInspectable args);

        void CreateBackgroundWorker();

    public:
        /**
         * Creates a new TransferManager object.
         *
         * @param EnableUINotify Enable the UI notify timer if true.
         */
        TransferManager(
            bool EnableUINotify);

        /**
         * Destroys a TransferManager object.
         */
        ~TransferManager();

        /**
         * Destroys a TransferManager object.
         */
        void Close();

        // Gets the version of Nagisa.
        hstring Version() const;

        // Gets the filter to use for searching the task list.
        hstring SearchFilter() const;

        // Sets the filter to use for searching the task list.
        void SearchFilter(
            hstring const& value);

        // Gets the last used folder.
        IStorageFolder LastusedFolder();

        // Gets the default download folder.
        IStorageFolder DefaultFolder();

        // Sets the default download folder.
        void DefaultFolder(
            IStorageFolder const& value);

        // Gets the total download bandwidth.
        uint64_t TotalDownloadBandwidth() const;

        // Gets the total upload bandwidth.
        uint64_t TotalUploadBandwidth() const;

        /**
         * Gets the task list.
         *
         * @return The object which represents the task list.
         */
        IAsyncOperation<IVectorView<ITransferTask>>
            GetTasksAsync();

        /**
         * Add a task to the task list.
         *
         * @param SourceUri The source uri object of task.
         * @param DesiredFileName The file name you desire.
         * @param SaveFolder The object of the folder which you want to save.
         * @return The asynchronous object used to wait.
         */
        IAsyncAction AddTaskAsync(
            Uri const SourceUri,
            hstring const DesiredFileName,
            IStorageFolder const SaveFolder);

        /**
         * Removes a task to the task list.
         *
         * @param The task object.
         * @return The asynchronous object used to wait.
         */
        IAsyncAction RemoveTaskAsync(
            ITransferTask const Task);

        /**
         * Start all tasks.
         */
        void StartAllTasks();

        /**
         * Pause all tasks.
         */
        void PauseAllTasks();

        /**
         * Clears the task list.
         */
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
