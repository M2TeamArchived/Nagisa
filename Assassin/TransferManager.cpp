/*
 * PROJECT:   Assassin
 * FILE:      TransferManager.cpp
 * PURPOSE:   Implementation for the TransferManager
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include "TransferManager.h"

#include "M2BindableVectorView.h"

namespace winrt::Assassin::implementation
{
    using Windows::Foundation::Collections::IKeyValuePair;
    using Windows::Foundation::TimeSpan;
    using Windows::Networking::BackgroundTransfer::BackgroundDownloadProgress;
    using Windows::Networking::BackgroundTransfer::BackgroundTransferStatus;
    using Windows::Storage::AccessCache::StorageApplicationPermissions;
    using Windows::Storage::ApplicationData;
    using Windows::Storage::ApplicationDataCreateDisposition;
    using Windows::Storage::CreationCollisionOption;
    using Windows::Storage::StorageDeleteOption;

    bool NAIsFinalTransferTaskStatus(
        TransferTaskStatus Status) noexcept
    {
        return (
            TransferTaskStatus::Canceled == Status ||
            TransferTaskStatus::Completed == Status ||
            TransferTaskStatus::Error == Status);
    }

    void TransferTask::Status(
        TransferTaskStatus const& value)
    {
        this->m_TaskConfig.Insert(
            L"Status",
            box_value(static_cast<uint32_t>(value)));
    }

    void TransferTask::BytesReceived(
        uint64_t const& value)
    {
        this->m_TaskConfig.Insert(
            L"BytesReceived",
            box_value(value));
    }

    void TransferTask::BytesReceivedSpeed(
        uint64_t const& value)
    {
        this->m_TaskConfig.Insert(
            L"BytesReceivedSpeed",
            box_value(value));
    }

    void TransferTask::RemainTime(
        uint64_t const& value)
    {
        this->m_TaskConfig.Insert(
            L"RemainTime",
            box_value(value));
    }

    void TransferTask::TotalBytesToReceive(
        uint64_t const& value)
    {
        this->m_TaskConfig.Insert(
            L"TotalBytesToReceive",
            box_value(value));
    }

    IAsyncAction TransferTask::Initialize(
        hstring Guid,
        ApplicationDataCompositeValue TaskConfig,
        IStorageItemAccessList FutureAccessList,
        std::map<hstring, DownloadOperation>& DownloadOperationMap)
    {
        this->m_Guid = Guid;
        this->m_TaskConfig = TaskConfig;
        this->m_FutureAccessList = FutureAccessList;

        try
        {
            std::map<hstring, DownloadOperation>::iterator iterator =
                DownloadOperationMap.find(unbox_value_or<hstring>(
                    TaskConfig.Lookup(L"BackgroundTransferGuid"), hstring()));
            if (DownloadOperationMap.end() != iterator)
            {
                this->m_Operation = iterator->second;
            }
            else
            {
                throw_hresult(E_FAIL);
            }

            BackgroundDownloadProgress Progress = this->m_Operation.Progress();

            if (TransferTaskStatus::Running == this->Status())
            {
                this->m_Operation.AttachAsync();
            }
        }
        catch (...)
        {
            if (!NAIsFinalTransferTaskStatus(this->Status()))
            {
                this->Status(TransferTaskStatus::Error);
            }
        }

        co_return;
    }

    void TransferTask::UpdateChangedProperties()
    {
        if (nullptr != this->m_Operation)
        {
            BackgroundDownloadProgress Progress = this->m_Operation.Progress();

            switch (Progress.Status)
            {
            case BackgroundTransferStatus::Idle:
                this->Status(TransferTaskStatus::Queued);
                break;
            case BackgroundTransferStatus::Running:
            case BackgroundTransferStatus::PausedCostedNetwork:
            case BackgroundTransferStatus::PausedNoNetwork:
            case BackgroundTransferStatus::PausedSystemPolicy:
                this->Status(TransferTaskStatus::Running);
                break;
            case BackgroundTransferStatus::PausedByApplication:
                this->Status(TransferTaskStatus::Paused);
                break;
            case BackgroundTransferStatus::Completed:
                this->Status(TransferTaskStatus::Completed);
                break;
            case BackgroundTransferStatus::Canceled:
                this->Status(TransferTaskStatus::Canceled);
                break;
            case BackgroundTransferStatus::Error:
            default:
                this->Status(TransferTaskStatus::Error);
                break;
            }
            if (TransferTaskStatus::Running != this->Status())
                return;

            ULONGLONG LastTickCount = this->m_TickCount;
            this->m_TickCount = M2GetTickCount();

            uint64_t LastBytesReceived = this->BytesReceived();
            this->BytesReceived(Progress.BytesReceived);

            this->TotalBytesToReceive(Progress.TotalBytesToReceive);

            uint64_t DeltaBytesReceived = this->BytesReceived() - LastBytesReceived;
            ULONGLONG DeltaPassedTime = this->m_TickCount - LastTickCount;
            this->BytesReceivedSpeed(DeltaBytesReceived * 1000 / DeltaPassedTime);

            if (0 == this->BytesReceivedSpeed() ||
                0 == this->TotalBytesToReceive())
            {
                this->RemainTime(
                    static_cast<uint64_t>(-1));
            }
            else
            {
                uint64_t RemainBytesToReceive =
                    this->TotalBytesToReceive() - this->BytesReceived();
                this->RemainTime(
                    RemainBytesToReceive / this->BytesReceivedSpeed());
            }
        }
    }

    void TransferTask::NotifyPropertyChanged()
    {
        if (nullptr != this->m_Operation)
        {
            this->RaisePropertyChanged(L"Status");
            if (TransferTaskStatus::Running == this->Status())
            {
                this->RaisePropertyChanged(L"BytesReceived");
                this->RaisePropertyChanged(L"TotalBytesToReceive");
                this->RaisePropertyChanged(L"BytesReceivedSpeed");
                this->RaisePropertyChanged(L"RemainTime");
            }
        }
    }

    ApplicationDataCompositeValue TransferTask::GetTaskConfig()
    {
        this->Status(this->Status());

        return this->m_TaskConfig;
    }

    // Gets the Guid string of the task.
    hstring TransferTask::Guid() const
    {
        return this->m_Guid;
    }

    // Gets the URI which to download the file.
    Uri TransferTask::SourceUri() const
    {
        try
        {
            return Uri(unbox_value_or<hstring>(
                this->m_TaskConfig.Lookup(L"SourceUri"), hstring()));
        }
        catch (...)
        {
            return nullptr;
        }
    }

    // Gets the file name which to download the file.
    hstring TransferTask::FileName() const
    {
        return unbox_value_or<hstring>(
            this->m_TaskConfig.Lookup(L"FileName"), hstring());
    }

    // Gets the save file object which to download the file.
    IAsyncOperation<IStorageFile> TransferTask::SaveFile() const
    {
        try
        {
            co_return co_await (co_await this->SaveFolder()).GetFileAsync(
                this->FileName());
        }
        catch (...)
        {
            co_return nullptr;
        }
    }

    // Gets the save folder object which to download the file. 
    IAsyncOperation<IStorageFolder> TransferTask::SaveFolder() const
    {
        try
        {
            co_return co_await this->m_FutureAccessList.GetFolderAsync(
                unbox_value_or<hstring>(
                    this->m_TaskConfig.Lookup(L"SaveFolder"), hstring()));
        }
        catch (...)
        {
            co_return nullptr;
        }
    }

    // Gets the current status of the task.
    TransferTaskStatus TransferTask::Status() const
    {
        return static_cast<TransferTaskStatus>(unbox_value_or<uint32_t>(
            this->m_TaskConfig.Lookup(L"Status"),
            static_cast<uint32_t>(TransferTaskStatus::Error)));
    }

    // Gets the total number of bytes received. This value does not include
    // bytes received as response headers. If the task has restarted, this
    // value may be smaller than in the previous progress report.
    uint64_t TransferTask::BytesReceived() const
    {
        return unbox_value_or<uint64_t>(
            this->m_TaskConfig.Lookup(L"BytesReceived"), 0);
    }

    // Gets the speed of bytes received in one second.
    uint64_t TransferTask::BytesReceivedSpeed() const
    {
        return unbox_value_or<uint64_t>(
            this->m_TaskConfig.Lookup(L"BytesReceivedSpeed"), 0);
    }

    // Gets the remain time, in seconds.
    uint64_t TransferTask::RemainTime() const
    {
        return unbox_value_or<uint64_t>(
            this->m_TaskConfig.Lookup(L"RemainTime"), 0);
    }

    // Gets the total number of bytes of data to download. If this number
    // is unknown, this value is set to 0.
    uint64_t TransferTask::TotalBytesToReceive() const
    {
        return unbox_value_or<uint64_t>(
            this->m_TaskConfig.Lookup(L"TotalBytesToReceive"), 0);
    }

    /**
     * Pauses a download operation.
     */
    void TransferTask::Pause()
    {
        if (TransferTaskStatus::Running == this->Status())
        {
            if (nullptr != this->m_Operation)
            {
                this->m_Operation.Pause();
            }
            else
            {
                this->Status(TransferTaskStatus::Error);
            }
        }
    }

    /**
     * Resumes a paused download operation.
     */
    void TransferTask::Resume()
    {
        if (TransferTaskStatus::Paused == this->Status())
        {
            if (nullptr != this->m_Operation)
            {
                this->m_Operation.Resume();
                this->m_Operation.AttachAsync();
            }
            else
            {
                this->Status(TransferTaskStatus::Error);
            }
        }
    }

    /**
     * Cancels a download operation.
     */
    void TransferTask::Cancel()
    {
        if (!NAIsFinalTransferTaskStatus(this->Status()))
        {
            if (nullptr != this->m_Operation)
            {
                this->m_Operation.AttachAsync().Cancel();
            }
            else
            {
                this->Status(TransferTaskStatus::Error);
            }
        }
    }

    IAsyncAction TransferManager::Initialize(
        bool EnableUINotify)
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        this->m_Downloader = BackgroundDownloader();

        this->m_FutureAccessList =
            StorageApplicationPermissions::FutureAccessList();

        this->m_RootContainer =
            ApplicationData::Current().LocalSettings().CreateContainer(
                L"Nagisa",
                ApplicationDataCreateDisposition::Always);
        this->m_TasksContainer =
            this->m_RootContainer.CreateContainer(
                L"Tasks",
                ApplicationDataCreateDisposition::Always);

        if (this->m_RootContainer.Values().HasKey(L"LastusedFolder"))
        {
            try
            {
                this->m_LastusedFolder =
                    co_await this->m_FutureAccessList.GetFolderAsync(
                        unbox_value<hstring>(
                            this->m_RootContainer.Values().Lookup(
                                L"LastusedFolder")));
            }
            catch (...)
            {
                this->m_LastusedFolder = nullptr;
                this->m_RootContainer.Values().Remove(L"LastusedFolder");
            }
        }

        if (this->m_RootContainer.Values().HasKey(L"DefaultFolder"))
        {
            try
            {
                this->m_DefaultFolder =
                    co_await this->m_FutureAccessList.GetFolderAsync(
                        unbox_value<hstring>(
                            this->m_RootContainer.Values().Lookup(
                                L"DefaultFolder")));
            }
            catch (...)
            {
                this->m_DefaultFolder = nullptr;
                this->m_RootContainer.Values().Remove(L"DefaultFolder");
            }
        }

        if (EnableUINotify)
        {
            this->m_UINotifyTimer = DispatcherTimer();

            // 10,000 ticks per millisecond.
            this->m_UINotifyTimer.Interval(TimeSpan(1000 * 10000));

            this->m_UINotifyTimer.Tick(
                { this, &TransferManager::UINotifyTimerTick });

            this->m_UINotifyTimer.Start();
        }
    }

    void TransferManager::UpdateTransferTaskStatusWithoutLock(
        bool NotifyUI)
    {
        if (NotifyUI)
        {
            this->m_TotalDownloadBandwidth = 0;
            this->m_TotalUploadBandwidth = 0;
        }

        for (ITransferTask Task : this->m_TaskList)
        {
            if (nullptr == Task) continue;

            TransferTask& TaskInternal = *Task.try_as<TransferTask>();

            this->m_TasksContainer.Values().Insert(
                TaskInternal.Guid(),
                TaskInternal.GetTaskConfig());

            if (NotifyUI)
            {
                TaskInternal.UpdateChangedProperties();
                TaskInternal.NotifyPropertyChanged();

                this->m_TotalDownloadBandwidth +=
                    TaskInternal.BytesReceivedSpeed();
                this->m_TotalUploadBandwidth += 0;
            }
        }

        if (NotifyUI)
        {
            this->RaisePropertyChanged(L"TotalDownloadBandwidth");
            this->RaisePropertyChanged(L"TotalUploadBandwidth");
        }
    }

    void TransferManager::UINotifyTimerTick(
        const IInspectable sender,
        const IInspectable args)
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        this->UpdateTransferTaskStatusWithoutLock(true);
    }

    void TransferManager::CreateBackgroundWorker()
    {
        using Windows::ApplicationModel::Background::BackgroundTaskBuilder;
        using Windows::ApplicationModel::Background::BackgroundTaskRegistration;
        using Windows::ApplicationModel::Background::IBackgroundTaskRegistration;
        using Windows::ApplicationModel::Background::SocketActivityTrigger;

        const wchar_t* BackgroundWorkerTaskName = L"Assassin.BackgroundWorker";

        IBackgroundTaskRegistration Task = nullptr;
        for (auto Current : BackgroundTaskRegistration::AllTasks())
        {
            if (BackgroundWorkerTaskName == Current.Value().Name())
            {
                Task = Current.Value();
                break;
            }
        }

        if (nullptr == Task)
        {
            auto TaskBuilder = BackgroundTaskBuilder();
            TaskBuilder.Name(BackgroundWorkerTaskName);
            TaskBuilder.TaskEntryPoint(L"Assassin.BackgroundWorker");
            TaskBuilder.SetTrigger(SocketActivityTrigger());
            TaskBuilder.IsNetworkRequested(true);
            Task = TaskBuilder.Register();
        }



    }

    /**
     * Creates a new TransferManager object.
     *
     * @param EnableUINotify Enable the UI notify timer if true.
     */
    TransferManager::TransferManager(
        bool EnableUINotify)
    {
        this->Initialize(EnableUINotify);
    }

    /**
     * Destroys a TransferManager object.
     */
    TransferManager::~TransferManager()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        if (nullptr != this->m_UINotifyTimer)
        {
            this->m_UINotifyTimer.Stop();
        }
    }

    /**
     * Destroys a TransferManager object.
     */
    void TransferManager::Close()
    {
        delete this;
    }

    // Gets the version of Nagisa.
    hstring TransferManager::Version() const
    {
        return NAGISA_VERSION_STRING;
    }

    // Gets the filter to use for searching the task list.
    hstring TransferManager::SearchFilter() const
    {
        return this->m_SearchFilter;
    }

    // Sets the filter to use for searching the task list.
    void TransferManager::SearchFilter(
        hstring const& value)
    {
        this->m_SearchFilter = value;
    }

    // Gets the last used folder.
    IStorageFolder TransferManager::LastusedFolder()
    {
        return this->m_LastusedFolder;
    }

    // Gets the default download folder.
    IStorageFolder TransferManager::DefaultFolder()
    {
        return this->m_DefaultFolder;
    }

    // Sets the default download folder.
    void TransferManager::DefaultFolder(
        IStorageFolder const& value)
    {
        this->m_DefaultFolder = value;

        if (nullptr != this->m_DefaultFolder)
        {
            this->m_RootContainer.Values().Insert(
                L"DefaultFolder",
                box_value(this->m_FutureAccessList.Add(
                    this->m_DefaultFolder)));
        }
        else
        {
            if (this->m_RootContainer.Values().HasKey(L"DefaultFolder"))
            {
                this->m_RootContainer.Values().Remove(L"DefaultFolder");
            }
        }
    }

    // Gets the total download bandwidth.
    uint64_t TransferManager::TotalDownloadBandwidth() const
    {
        return this->m_TotalDownloadBandwidth;
    }

    // Gets the total upload bandwidth.
    uint64_t TransferManager::TotalUploadBandwidth() const
    {
        return this->m_TotalUploadBandwidth;
    }

    /**
     * Gets the task list.
     *
     * @return The object which represents the task list.
     */
    IAsyncOperation<IVectorView<ITransferTask>>
        TransferManager::GetTasksAsync()
    {
        co_await resume_background();

        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        this->UpdateTransferTaskStatusWithoutLock(false);

        this->m_TaskList.clear();

        hstring CurrentSearchFilter = this->SearchFilter();

        bool NeedSearchFilter = !CurrentSearchFilter.empty();

        std::map<hstring, DownloadOperation> DownloadsList;

        for (DownloadOperation Item
            : co_await this->m_Downloader.GetCurrentDownloadsAsync())
        {
            DownloadsList.insert(
                std::pair<hstring, DownloadOperation>(to_hstring(Item.Guid()), Item));
        }

        for (IKeyValuePair<hstring, IInspectable> Download
            : this->m_TasksContainer.Values())
        {
            ITransferTask Task = make<TransferTask>();

            co_await Task.try_as<TransferTask>()->Initialize(
                Download.Key(),
                Download.Value().try_as<ApplicationDataCompositeValue>(),
                this->m_FutureAccessList,
                DownloadsList);

            if (NeedSearchFilter)
            {
                if (!M2FindSubString(
                    Task.FileName(), CurrentSearchFilter, true))
                {
                    continue;
                }
            }

            this->m_TaskList.push_back(Task);
        }

        // Asynchronous call return.
        co_return make<M2::BindableVectorView<ITransferTask>>(
            this->m_TaskList);
    }

    /**
     * Add a task to the task list.
     *
     * @param SourceUri The source uri object of task.
     * @param DesiredFileName The file name you desire.
     * @param SaveFolder The object of the folder which you want to save.
     * @return The asynchronous object used to wait.
     */
    IAsyncAction TransferManager::AddTaskAsync(
        Uri const SourceUri,
        hstring const DesiredFileName,
        IStorageFolder const SaveFolder)
    {
        co_await resume_background();

        IStorageFile SaveFile = co_await SaveFolder.CreateFileAsync(
            DesiredFileName,
            CreationCollisionOption::GenerateUniqueName);

        hstring Token = this->m_FutureAccessList.Add(SaveFolder);

        this->m_LastusedFolder = SaveFolder;
        this->m_RootContainer.Values().Insert(
            L"LastusedFolder",
            box_value(Token));

        DownloadOperation Operation = this->m_Downloader.CreateDownload(
            SourceUri, SaveFile);

        ApplicationDataCompositeValue TaskConfig =
            ApplicationDataCompositeValue();

        TaskConfig.Insert(
            L"SourceUri",
            box_value(SourceUri.RawUri()));
        TaskConfig.Insert(
            L"FileName",
            box_value(SaveFile.Name()));
        TaskConfig.Insert(
            L"SaveFolder",
            box_value(Token));
        TaskConfig.Insert(
            L"Status",
            box_value(
                static_cast<uint32_t>(TransferTaskStatus::Queued)));

        TaskConfig.Insert(
            L"BackgroundTransferGuid",
            box_value(to_hstring(Operation.Guid())));

        this->m_TasksContainer.Values().Insert(
            to_hstring(M2CreateGuid()),
            TaskConfig);

        Operation.StartAsync();

        // Asynchronous call return.
        co_return;
    }

    /**
     * Removes a task to the task list.
     *
     * @param The task object.
     * @return The asynchronous object used to wait.
     */
    IAsyncAction TransferManager::RemoveTaskAsync(
        ITransferTask const Task)
    {
        co_await resume_background();

        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        if (!NAIsFinalTransferTaskStatus(Task.Status()))
        {
            Task.Cancel();
        }

        if (TransferTaskStatus::Completed != Task.Status())
        {
            try
            {
                IStorageFile SaveFile = co_await Task.SaveFile();
                if (nullptr != SaveFile)
                {
                    co_await SaveFile.DeleteAsync(
                        StorageDeleteOption::PermanentDelete);
                }
            }
            catch (...)
            {

            }
        }

        for (size_t i = 0; i < this->m_TaskList.size(); ++i)
        {
            if (nullptr == this->m_TaskList[i]) continue;

            if (Task.Guid() == this->m_TaskList[i].Guid())
            {
                this->m_TaskList[i] = nullptr;
            }
        }
        this->m_TasksContainer.Values().Remove(Task.Guid());

        // Asynchronous call return.
        co_return;
    }

    /**
     * Start all tasks.
     */
    void TransferManager::StartAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        for (ITransferTask Task : this->m_TaskList)
        {
            if (nullptr == Task) continue;

            Task.Resume();
        }
    }

    /**
     * Pause all tasks.
     */
    void TransferManager::PauseAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        for (ITransferTask Task : this->m_TaskList)
        {
            if (nullptr == Task) continue;

            Task.Pause();
        }
    }

    /**
     * Clears the task list.
     */
    void TransferManager::ClearTaskList()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        for (ITransferTask Task : this->m_TaskList)
        {
            if (NAIsFinalTransferTaskStatus(Task.Status()))
            {
                this->RemoveTaskAsync(Task);
            }
        }
    }
}
