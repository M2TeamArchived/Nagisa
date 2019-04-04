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
            TransferTaskStatus::Error == Status ||
            TransferTaskStatus::Removed == Status);
    }

    hstring TransferTask::ReadHString(
        std::wstring_view const& KeyName) const
    {
        return unbox_value_or<hstring>(
            this->m_TaskConfig.Lookup(KeyName), hstring());
    }

    uint64_t TransferTask::ReadUInt64(
        std::wstring_view const& KeyName) const
    {
        return unbox_value_or<uint64_t>(this->m_TaskConfig.Lookup(KeyName), 0);
    }

    void TransferTask::WriteHString(
        std::wstring_view const& KeyName,
        hstring const& Value)
    {
        this->m_TaskConfig.Insert(KeyName, box_value(Value));
    }

    void TransferTask::WriteUInt64(
        std::wstring_view const& KeyName,
        uint64_t const& Value)
    {
        this->m_TaskConfig.Insert(KeyName, box_value(Value));
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
        this->WriteHString(L"SourceUri", value.RawUri());
    }

    void TransferTask::FileName(
        hstring const& value)
    {
        this->WriteHString(L"FileName", value);
    }

    void TransferTask::SaveFolder(
        IStorageFolder const& value)
    {
        if (!value) return;

        this->WriteHString(L"SaveFolder", this->m_FutureAccessList.Add(value));
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
        this->WriteUInt64(L"BytesReceived", value);
    }

    void TransferTask::BytesReceivedSpeed(
        uint64_t const& value)
    {
        this->WriteUInt64(L"BytesReceivedSpeed", value);
    }

    void TransferTask::RemainTime(
        uint64_t const& value)
    {
        this->WriteUInt64(L"RemainTime", value);
    }

    void TransferTask::TotalBytesToReceive(
        uint64_t const& value)
    {
        this->WriteUInt64(L"TotalBytesToReceive", value);
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
        if (this->m_Operation)
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
            return Uri(this->ReadHString(L"SourceUri"));
        }
        catch (...)
        {
            return nullptr;
        }
    }

    // Gets the file name which to download the file.
    hstring TransferTask::FileName() const
    {
        return this->ReadHString(L"FileName");
    }

    // Gets the save file object which to download the file.
    IAsyncOperation<IStorageFile> TransferTask::SaveFile() const
    {
        try
        {
            IStorageFile FileObject = nullptr;

            IStorageFolder FolderObject = co_await this->SaveFolder();
            if (FolderObject)
                FileObject = co_await FolderObject.GetFileAsync(this->FileName());

            co_return FileObject;
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
            hstring FolderString = this->ReadHString(L"SaveFolder");

            IStorageFolder FolderObject = nullptr;

            if (!FolderString.empty())
                FolderObject = co_await this->FutureAccessList().GetFolderAsync(
                    FolderString);

            co_return FolderObject;
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
        return this->ReadUInt64(L"BytesReceived");
    }

    // Gets the speed of bytes received in one second.
    uint64_t TransferTask::BytesReceivedSpeed() const
    {
        return this->ReadUInt64(L"BytesReceivedSpeed");
    }

    // Gets the remain time, in seconds.
    uint64_t TransferTask::RemainTime() const
    {
        return this->ReadUInt64(L"RemainTime");
    }

    // Gets the total number of bytes of data to download. If this number
    // is unknown, this value is set to 0.
    uint64_t TransferTask::TotalBytesToReceive() const
    {
        return this->ReadUInt64(L"TotalBytesToReceive");
    }

    /**
     * Pauses a download operation.
     */
    void TransferTask::Pause()
    {
        try
        {
            if (TransferTaskStatus::Running != this->Status())
                return;

            if (this->m_Operation)
                this->m_Operation.Pause();
        }
        catch (...)
        {
            this->Status(TransferTaskStatus::Error);
        }
    }

    /**
     * Resumes a paused download operation.
     */
    void TransferTask::Resume()
    {
        try
        {
            if (TransferTaskStatus::Paused != this->Status())
                return;

            if (this->m_Operation)
            {
                this->m_Operation.Resume();
                auto Operation = this->m_Operation.AttachAsync();
                UNREFERENCED_PARAMETER(Operation);
            }
        }
        catch (...)
        {
            this->Status(TransferTaskStatus::Error);
        }
    }

    /**
     * Cancels a download operation.
     */
    void TransferTask::Cancel()
    {
        try
        {
            if (NAIsFinalTransferTaskStatus(this->Status()))
                return;

            if (this->m_Operation)
                this->m_Operation.AttachAsync().Cancel();
        }
        catch (...)
        {
            this->Status(TransferTaskStatus::Error);
        }
    }

    fire_and_forget TransferManager::NotifyTimerTick(
        ThreadPoolTimer const& source)
    {
        UNREFERENCED_PARAMETER(source);  // Unused parameter.

        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        bool TaskListChanged = false;

        this->m_TotalDownloadBandwidth = 0;
        this->m_TotalUploadBandwidth = 0;

        for (auto it = this->m_TaskList.begin(); it != this->m_TaskList.end();)
        {
            auto& Task = *it;

            TransferTask& TaskInternal = *Task.try_as<TransferTask>();

            if (TransferTaskStatus::Removed == TaskInternal.Status())
            {
                TaskListChanged = true;
                co_await this->RemoveTaskInternalAsync(Task);
                this->m_TaskList.erase(it);
                continue;
            }

            TaskInternal.UpdateChangedProperties();

            this->m_TasksContainer.Values().Insert(
                TaskInternal.Guid(),
                TaskInternal.TaskConfig());

            this->m_TotalDownloadBandwidth +=
                TaskInternal.BytesReceivedSpeed();
            this->m_TotalUploadBandwidth += 0;

            ++it;
        }

        if (this->m_EnableUINotify)
        {
            auto Operation = M2ExecuteOnUIThread([this]()
                {
                    for (auto& Task : this->m_TaskList)
                        Task.try_as<TransferTask>()->NotifyPropertyChanged();

                    this->RaisePropertyChanged(L"TotalDownloadBandwidth");
                    this->RaisePropertyChanged(L"TotalUploadBandwidth");
                });
            UNREFERENCED_PARAMETER(Operation);
        }

        if (TaskListChanged)
        {
            this->NotifyTaskListUpdated();
        }

        co_return;
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
            auto Operation = M2ExecuteOnUIThread([this]()
                {
                    this->RaisePropertyChanged(L"Tasks");
                });
            UNREFERENCED_PARAMETER(Operation);
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

    IAsyncAction TransferManager::RemoveTaskInternalAsync(
        ITransferTask const Task)
    {
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

        this->m_TasksContainer.Values().Remove(Task.Guid());
    }

    fire_and_forget TransferManager::Initialize()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

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

        std::map<hstring, DownloadOperation> DownloadsList;

        for (DownloadOperation Item
            : co_await this->m_Downloader.GetCurrentDownloadsAsync())
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
                    auto Operation = TaskInternal.Operation().AttachAsync();
                    UNREFERENCED_PARAMETER(Operation);
                }
            }
            else
            {
                if (!NAIsFinalTransferTaskStatus(TaskInternal.Status()))
                {
                    TaskInternal.Status(TransferTaskStatus::Error);
                }
            }

            this->m_TaskList.push_back(Task);
        }

        using Windows::System::Threading::TimerElapsedHandler;

        this->m_NotifyTimer = ThreadPoolTimer::CreatePeriodicTimer(
            { this, &TransferManager::NotifyTimerTick },
            std::chrono::seconds(1));
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
        this->Initialize();
    }

    /**
     * Destroys a TransferManager object.
     */
    TransferManager::~TransferManager()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

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
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        IVectorView<ITransferTask> Result = nullptr;

        hstring CurrentSearchFilter = this->SearchFilter();

        std::vector<ITransferTask> TaskList;

        for (auto& Task : this->m_TaskList)
        {
            if (!CurrentSearchFilter.empty())
            {
                if (!M2FindSubString(
                    Task.FileName(),
                    CurrentSearchFilter,
                    true))
                {
                    continue;
                }
            }

            TaskList.push_back(Task);
        }

        if (TaskList.size())
            Result = make<M2::BindableVectorView<ITransferTask>>(TaskList);

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
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

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

        this->m_TaskList.push_back(Task);

        auto Operation = TaskInternal.Operation().StartAsync();
        UNREFERENCED_PARAMETER(Operation);

        this->NotifyTaskListUpdated();

        // Asynchronous call return.
        co_return;
    }

    /**
     * Removes a task to the task list.
     *
     * @param Task The task object.
     */
    void TransferManager::RemoveTask(
        ITransferTask const Task)
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        TransferTask& TaskInternal = *Task.try_as<TransferTask>();
        TaskInternal.Status(TransferTaskStatus::Removed);
    }

    /**
     * Start all tasks.
     */
    void TransferManager::ResumeAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        for (auto& Task : this->m_TaskList)
        {
            Task.Resume();
        }
    }

    /**
     * Pause all tasks.
     */
    void TransferManager::PauseAllTasks()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        for (auto& Task : this->m_TaskList)
        {
            Task.Pause();
        }
    }

    /**
     * Clears the task list.
     */
    void TransferManager::ClearTaskList()
    {
        M2::AutoCriticalSectionLock Lock(this->m_TaskListUpdateLock);

        for (auto& Task : this->m_TaskList)
        {
            TransferTask& TaskInternal = *Task.try_as<TransferTask>();
            
            if (NAIsFinalTransferTaskStatus(TaskInternal.Status()))
            {
                TaskInternal.Status(TransferTaskStatus::Removed);
            }
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
