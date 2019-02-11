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

    DownloadOperation TransferTask::Operation() const
    {
        return this->m_Operation;
    }

    ApplicationDataCompositeValue TransferTask::TaskConfig() const
    {
        return this->m_TaskConfig;
    }

    IStorageItemAccessList TransferTask::FutureAccessList() const
    {
        return this->m_FutureAccessList;
    }

    void TransferTask::Operation(
        DownloadOperation const& value)
    {
        this->m_Operation = value;
    }

    void TransferTask::TaskConfig(
        ApplicationDataCompositeValue const& value)
    {
        this->m_TaskConfig = value;
    }

    void TransferTask::FutureAccessList(
        IStorageItemAccessList const& value)
    {
        this->m_FutureAccessList = value;
    }

    void TransferTask::Guid(
        hstring const& value)
    {
        this->m_Guid = value;
    }

    void TransferTask::SourceUri(
        Uri const& value)
    {
        this->m_TaskConfig.Insert(
            L"SourceUri",
            box_value(value.RawUri()));
    }

    void TransferTask::FileName(
        hstring const& value)
    {
        this->m_TaskConfig.Insert(
            L"FileName",
            box_value(value));
    }

    void TransferTask::SaveFolder(
        IStorageFolder const& value)
    {
        if (value)
        {
            this->m_TaskConfig.Insert(
                L"SaveFolder",
                box_value(this->m_FutureAccessList.Add(value)));
        }
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

    void TransferTask::UpdateChangedProperties()
    {
        this->BytesReceivedSpeed(0);

        if (this->m_Operation)
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
        M2ExecuteOnUIThread([this]()
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
        });
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
        IStorageFile FileObject = nullptr;

        IStorageFolder FolderObject = co_await this->SaveFolder();
        if (FolderObject)
            FileObject = co_await FolderObject.GetFileAsync(this->FileName());

        return FileObject;
    }

    // Gets the save folder object which to download the file. 
    IAsyncOperation<IStorageFolder> TransferTask::SaveFolder() const
    {
        hstring FolderString = unbox_value_or<hstring>(
            this->m_TaskConfig.Lookup(L"SaveFolder"),
            hstring());

        IStorageFolder FolderObject = nullptr;

        if (!FolderString.empty())
            FolderObject = co_await this->FutureAccessList().GetFolderAsync(
                FolderString);
            
        co_return FolderObject;
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

    void TransferManager::UpdateTransferTaskStatusWithoutLock()
    {
        this->m_TotalDownloadBandwidth = 0;
        this->m_TotalUploadBandwidth = 0;

        for (auto& Task : this->m_TaskList)
        {
            TransferTask& TaskInternal = *Task.second.try_as<TransferTask>();

            TaskInternal.UpdateChangedProperties();

            this->m_TasksContainer.Values().Insert(
                TaskInternal.Guid(),
                TaskInternal.TaskConfig());

            this->m_TotalDownloadBandwidth +=
                TaskInternal.BytesReceivedSpeed();
            this->m_TotalUploadBandwidth += 0;

            if (this->m_EnableUINotify)
            {
                TaskInternal.NotifyPropertyChanged();
            }
        }

        if (this->m_EnableUINotify)
        {
            M2ExecuteOnUIThread([this]()
            {
                this->RaisePropertyChanged(L"TotalDownloadBandwidth");
                this->RaisePropertyChanged(L"TotalUploadBandwidth");
            });
        }
    }

    void TransferManager::NotifyTimerTick(
        ThreadPoolTimer const& source)
    {
        UNREFERENCED_PARAMETER(source);  // Unused parameter.

        M2::AutoTryCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        if (Lock.IsLocked())
        {
            this->UpdateTransferTaskStatusWithoutLock();
        }
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

    void TransferManager::LastusedFolder(
        IStorageFolder const& value)
    {
        if (value)
        {
            this->m_RootContainer.Values().Insert(
                L"LastusedFolder",
                box_value(this->m_FutureAccessList.Add(value)));
        }
    }

    void TransferManager::NotifyTaskListUpdated()
    {
        if (this->m_EnableUINotify)
        {
            M2ExecuteOnUIThread([this]()
            {
                this->RaisePropertyChanged(L"Tasks");
            });
        }
    }

    IAsyncOperation<IStorageFolder> TransferManager::GetFolderObjectInternal(
        hstring const& FolderStringKey)
    {
        hstring FolderString = unbox_value_or<hstring>(
            this->m_RootContainer.Values().Lookup(FolderStringKey),
            hstring());

        IStorageFolder FolderObject = nullptr;

        if (!FolderString.empty())
            FolderObject = co_await this->m_FutureAccessList.GetFolderAsync(
                FolderString);

        co_return FolderObject;
    }

    /**
     * Creates a new TransferManager object.
     *
     * @param EnableUINotify Enable the UI notify timer if true.
     */
    TransferManager::TransferManager(
        bool EnableUINotify) :
        m_EnableUINotify(EnableUINotify)
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

        M2::CThread([this]()
        {
            std::map<hstring, DownloadOperation> DownloadsList;

            for (DownloadOperation Item
                : this->m_Downloader.GetCurrentDownloadsAsync().get())
            {
                DownloadsList.insert(std::pair(to_hstring(Item.Guid()), Item));
            }

            for (IKeyValuePair<hstring, IInspectable> Download
                : this->m_TasksContainer.Values())
            {
                ITransferTask Task = make<TransferTask>();

                TransferTask& TaskInternal = *Task.try_as<TransferTask>();

                TaskInternal.Guid(
                    Download.Key());
                TaskInternal.TaskConfig(
                    Download.Value().try_as<ApplicationDataCompositeValue>());
                TaskInternal.FutureAccessList(
                    this->m_FutureAccessList);

                std::map<hstring, DownloadOperation>::iterator iterator =
                    DownloadsList.find(unbox_value_or<hstring>(
                        TaskInternal.TaskConfig().Lookup(
                            L"BackgroundTransferGuid"),
                        hstring()));
                if (DownloadsList.end() != iterator)
                {
                    TaskInternal.Operation(iterator->second);

                    if (TransferTaskStatus::Running == TaskInternal.Status())
                    {
                        TaskInternal.Operation().AttachAsync();
                    }
                }
                else
                {
                    if (!NAIsFinalTransferTaskStatus(TaskInternal.Status()))
                    {
                        TaskInternal.Status(TransferTaskStatus::Error);
                    }
                }

                this->m_TaskList.insert(std::pair(Task.Guid(), Task));
            }
        }).Wait();

        using Windows::System::Threading::TimerElapsedHandler;

        this->m_NotifyTimer = ThreadPoolTimer::CreatePeriodicTimer(
            { this, &TransferManager::NotifyTimerTick },
            std::chrono::seconds(1));
    }

    /**
     * Destroys a TransferManager object.
     */
    TransferManager::~TransferManager()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        if (this->m_NotifyTimer)
        {
            this->m_NotifyTimer.Cancel();
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

        this->NotifyTaskListUpdated();
    }

    // Gets the last used folder.
    IAsyncOperation<IStorageFolder> TransferManager::LastusedFolder()
    {
        return this->GetFolderObjectInternal(L"LastusedFolder");
    }

    // Gets the default download folder.
    IAsyncOperation<IStorageFolder> TransferManager::DefaultFolder()
    {
        return this->GetFolderObjectInternal(L"DefaultFolder");
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

    // Gets the task list.
    IVectorView<ITransferTask> TransferManager::Tasks()
    {
        M2::AutoTryCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        IVectorView<ITransferTask> Result = nullptr;

        if (Lock.IsLocked())
        {
            hstring CurrentSearchFilter = this->SearchFilter();

            bool NeedSearchFilter = !CurrentSearchFilter.empty();

            std::vector<ITransferTask> TaskList;

            for (auto& Task : this->m_TaskList)
            {
                if (NeedSearchFilter)
                {
                    if (!M2FindSubString(
                        Task.second.FileName(),
                        CurrentSearchFilter,
                        true))
                    {
                        continue;
                    }
                }

                TaskList.push_back(Task.second);
            }

            if (TaskList.size())
                Result = make<M2::BindableVectorView<ITransferTask>>(TaskList);
        }
        else
        {
            this->NotifyTaskListUpdated();
        } 

        return Result;
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
        //co_await resume_background();

        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        IStorageFile SaveFile = co_await SaveFolder.CreateFileAsync(
            DesiredFileName,
            CreationCollisionOption::GenerateUniqueName);

        this->LastusedFolder(SaveFolder);

        ITransferTask Task = make<TransferTask>();

        TransferTask& TaskInternal = *Task.try_as<TransferTask>();

        TaskInternal.Guid(
            to_hstring(M2CreateGuid()));
        TaskInternal.TaskConfig(
            ApplicationDataCompositeValue());
        TaskInternal.FutureAccessList(
            this->m_FutureAccessList);
        TaskInternal.Operation(
            this->m_Downloader.CreateDownload(SourceUri, SaveFile));

        TaskInternal.SourceUri(SourceUri);
        TaskInternal.FileName(SaveFile.Name());
        TaskInternal.SaveFolder(SaveFolder);
        TaskInternal.Status(TransferTaskStatus::Queued);

        TaskInternal.TaskConfig().Insert(
            L"BackgroundTransferGuid",
            box_value(to_hstring(TaskInternal.Operation().Guid())));

        this->m_TasksContainer.Values().Insert(
            TaskInternal.Guid(),
            TaskInternal.TaskConfig());

        this->m_TaskList.insert(std::pair(Task.Guid(), Task));

        TaskInternal.Operation().StartAsync();

        this->NotifyTaskListUpdated();

        // Asynchronous call return.
        co_return;
    }

    /**
     * Removes a task to the task list.
     *
     * @param Task The task object.
     * @return The asynchronous object used to wait.
     */
    IAsyncAction TransferManager::RemoveTaskAsync(
        ITransferTask const Task)
    {
        //co_await resume_background();

        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        if (!NAIsFinalTransferTaskStatus(Task.Status()))
        {
            Task.Cancel();
        }

        if (TransferTaskStatus::Completed != Task.Status())
        {
            IStorageFile SaveFile = co_await Task.SaveFile();
            if (SaveFile)
            {
                co_await SaveFile.DeleteAsync(
                    StorageDeleteOption::PermanentDelete);
            }
        }

        this->m_TaskList.erase(Task.Guid());
        this->m_TasksContainer.Values().Remove(Task.Guid());

        this->NotifyTaskListUpdated();

        // Asynchronous call return.
        co_return;
    }

    /**
     * Start all tasks.
     */
    void TransferManager::ResumeAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        for (auto& Task : this->m_TaskList)
        {
            Task.second.Resume();
        }
    }

    /**
     * Pause all tasks.
     */
    void TransferManager::PauseAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        for (auto& Task : this->m_TaskList)
        {
            Task.second.Pause();
        }
    }

    /**
     * Clears the task list.
     * @return The asynchronous object used to wait.
     */
    IAsyncAction TransferManager::ClearTaskListAsync()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateCS);

        std::vector<ITransferTask> NeedToRemove;

        for (auto& Task : this->m_TaskList)
        {
            if (NAIsFinalTransferTaskStatus(Task.second.Status()))
            {
                NeedToRemove.push_back(Task.second);
            }
        }

        for (auto& Task : NeedToRemove)
        {
            co_await this->RemoveTaskAsync(Task);
        }
    }

    /**
    * Set the default folder.
    * @param DefaultFolder The default folder you want to set.
    */
    void TransferManager::SetDefaultFolder(
        IStorageFolder const& DefaultFolder)
    {
        if (DefaultFolder)
        {
            this->m_RootContainer.Values().Insert(
                L"DefaultFolder",
                box_value(this->m_FutureAccessList.Add(DefaultFolder)));
        }
        else
        {
            if (this->m_RootContainer.Values().HasKey(L"DefaultFolder"))
            {
                this->m_RootContainer.Values().Remove(L"DefaultFolder");
            }
        }
    }
}
