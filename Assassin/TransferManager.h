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
#include <winrt\Windows.System.Threading.h>

namespace winrt::Assassin::implementation
{
    using Assassin::ITransferTask;
    using Assassin::TransferTaskStatus;
    using Windows::Foundation::Collections::IVectorView;
    using Windows::Foundation::IAsyncAction;
    using Windows::Foundation::IAsyncOperation;
    using Windows::Foundation::IClosable;
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Uri;
    using Windows::Networking::BackgroundTransfer::BackgroundDownloader;
    using Windows::Networking::BackgroundTransfer::DownloadOperation;
    using Windows::Storage::AccessCache::IStorageItemAccessList;
    using Windows::Storage::ApplicationDataCompositeValue;
    using Windows::Storage::ApplicationDataContainer;
    using Windows::Storage::IStorageFile;
    using Windows::Storage::IStorageFolder;
    using Windows::System::Threading::ThreadPoolTimer;

    bool NAIsFinalTransferTaskStatus(
        TransferTaskStatus Status) noexcept;

    struct TransferTask : implements<
        TransferTask, ITransferTask, M2::NotifyPropertyChangedBase>
    {
    private:
        DownloadOperation m_Operation = nullptr;

        ApplicationDataCompositeValue m_TaskConfig = nullptr;

        IStorageItemAccessList m_FutureAccessList = nullptr;

        ULONGLONG m_TickCount = 0;
        hstring m_Guid;

        hstring ReadHString(
            std::wstring_view const& KeyName) const;

        uint64_t ReadUInt64(
            std::wstring_view const& KeyName) const;

        void WriteHString(
            std::wstring_view const& KeyName,
            hstring const& Value);

        void WriteUInt64(
            std::wstring_view const& KeyName,
            uint64_t const& Value);

    public:

        DownloadOperation Operation() const;

        ApplicationDataCompositeValue TaskConfig() const;

        IStorageItemAccessList FutureAccessList() const;

        void Operation(
            DownloadOperation const& value);

        void TaskConfig(
            ApplicationDataCompositeValue const& value);

        void FutureAccessList(
            IStorageItemAccessList const& value);

        void Guid(
            hstring const& value);

        void SourceUri(
            Uri const& value);

        void FileName(
            hstring const& value);

        void SaveFolder(
            IStorageFolder const& value);

        void Status(
            TransferTaskStatus const& value);

        void BytesReceived(
            uint64_t const& value);

        void BytesReceivedSpeed(
            uint64_t const& value);

        void RemainTime(
            uint64_t const& value);

        void TotalBytesToReceive(
            uint64_t const& value);

    public:
        TransferTask() = default;

        void UpdateChangedProperties();
        void NotifyPropertyChanged();
        

    public:
        // Gets the Guid string of the task.
        hstring Guid() const;

        // Gets the URI which to download the file.
        Uri SourceUri() const;

        // Gets the file name which to download the file.
        hstring FileName() const;
      
        // Gets the save file object which to download the file.
        IAsyncOperation<IStorageFile> SaveFile() const;
   
        // Gets the save folder object which to download the file. 
        IAsyncOperation<IStorageFolder> SaveFolder() const;

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
        bool m_EnableUINotify = false;

        BackgroundDownloader m_Downloader = nullptr;
        ThreadPoolTimer m_NotifyTimer = nullptr;

        M2::CSRWLock m_TaskListUpdateLock;
        std::map<hstring, ITransferTask> m_TaskList;

        ApplicationDataContainer m_RootContainer = nullptr;
        ApplicationDataContainer m_TasksContainer = nullptr;

        IStorageItemAccessList m_FutureAccessList = nullptr;

        uint64_t m_TotalDownloadBandwidth = 0;
        uint64_t m_TotalUploadBandwidth = 0;

        hstring m_SearchFilter;

        void UpdateTransferTaskStatusWithoutLock();

        void NotifyTimerTick(
            ThreadPoolTimer const& source);

        void CreateBackgroundWorker();

        void LastusedFolder(
            IStorageFolder const& value);

        void NotifyTaskListUpdated();

        IAsyncOperation<IStorageFolder> GetFolderObjectInternal(
            hstring const& FolderStringKey);

        IAsyncAction RemoveTaskInternalAsync(
            ITransferTask const Task);

        fire_and_forget Initialize();

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
        IAsyncOperation<IStorageFolder> LastusedFolder();

        // Gets the default download folder.
        IAsyncOperation<IStorageFolder> DefaultFolder();

        // Gets the total download bandwidth.
        uint64_t TotalDownloadBandwidth() const;

        // Gets the total upload bandwidth.
        uint64_t TotalUploadBandwidth() const;

        // Gets the task list.
        IVectorView<ITransferTask> Tasks();

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
         * @param Task The task object.
         * @return The asynchronous object used to wait.
         */
        IAsyncAction RemoveTaskAsync(
            ITransferTask const Task);

        /**
         * Resume all tasks.
         */
        void ResumeAllTasks();

        /**
         * Pause all tasks.
         */
        void PauseAllTasks();

        /**
         * Clears the task list.
         * @return The asynchronous object used to wait.
         */
        IAsyncAction ClearTaskListAsync();

        /**
         * Set the default folder.
         * @param DefaultFolder The default folder you want to set.
         */
        void SetDefaultFolder(
            IStorageFolder const& DefaultFolder);
    };
}

namespace winrt::Assassin::factory_implementation
{
    struct TransferManager : TransferManagerT<
        TransferManager, implementation::TransferManager>
    {
    };
}
