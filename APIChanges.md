# Nagisa - API Changes

## Notice
- This is the changelog for project Nagisa API implementations.

## Changelog

### Nagisa 0.3 [Build 25]
**New APIs**
- M2-Team Common Library
  - M2MakeUTF16String function.
  - M2MakeUTF8String function.
  - M2GetPointer function.
  - M2MakeIBuffer function.
  - M2MakeCXString function.
  - M2GetNumberOfHardwareThreads function.
  - M2GetLastError function.
  - M2::CDisableObjectCopying class.
  - M2::CMemory class.
  - M2GetProcAddress function.

**Changed APIs**
- M2-Team Common Library
  - M2::CObject template.

### Nagisa 0.2 [Build 22]
**New APIs**
- Assassin
  - ITransferTask interface.
    - Guid property.
    - SourceUri property.
    - FileName property.
    - SaveFile property.
    - SaveFolder property.
    - Cancel method.
  - ITransferManager interface.
    - AddTaskAsync method.
    - RemoveTaskAsync method.
	- StartAllTasks method.
	- PauseAllTasks method.
	- ClearTaskList method.
	- LastusedFolder property.
    - DefaultFolder property.
    - TotalDownloadBandwidth property.
    - TotalUploadBandwidth property.
  - TransferManagerFactory class.
    - CreateInstance method.
- M2-Team Common Library
  - M2CreateGuid function.
  - CFutureAccessList class.
- XAML
  - NullableBooleanToBooleanConverter converter.

**Removed APIs**
- Assassin
  - ITransferTask interface.
    - RequestedUri property.
    - ResultFile property.
    - CancelAsync method.
    - NotifyPropertyChanged method.
  - ITransferManager interface.
    - AddTask method.
  - TransferManager class (Based on ITransferManager interface).
    - Constructor.
	- Close method.
- XAML
  - StorageFileToFileNameConverter converter.

### Nagisa 0.1 [Build 10]
**New APIs**
- Assassin
  - TransferTaskStatus enum.
  - ITransferTask interface.
    - RequestedUri property.
	- ResultFile property.
	- Status property.
	- BytesReceived property.
	- BytesReceivedSpeed property.
	- RemainTime property.
	- TotalBytesToReceive property.
	- Pause method.
	- Resume method.
	- CancelAsync method.
	- NotifyPropertyChanged method.
  - ITransferManager interface.
    - Version property.
	- SearchFilter property.
	- GetTasksAsync method.
	- AddTask method.
  - TransferManager class (Based on ITransferManager interface).
    - Constructor.
	- Close method.
- M2-Team Common Library
  - M2RemoveReference trait.
  - M2AsyncSetCompletedHandler function.
  - M2AsyncSetProgressHandler function.
  - M2AsyncWait function.
  - M2ExecuteOnUIThread function.
  - IM2AsyncController interface.
  - IM2AsyncControllerEx interface.
  - M2AsyncCreate function.
  - M2GetInspectable function.
  - M2ThrowPlatformException function.
  - M2ThrowPlatformExceptionIfFailed function.
  - M2ThrownPlatformExceptionToHResult function.
  - M2FindSubString function.
  - M2ConvertByteSizeToString function.
  - M2FormatString function.
  - M2GetTickCount function.
  - M2PathFindFileName function.
  - M2::CThread class.
  - M2::CObject template.
  - M2::CHandle class.
  - M2::CComObject class.
- XAML
  - Uint64ToDoubleConverter converter.
  - Uint64ByteSizeToStringConverter converter.
  - StorageFileToFileNameConverter converter.
  - TaskStatusToVisibleConverter converter.
  - Uint64RemainTimeToStringConverter converter.
  - TaskListEmptyToVisibilityConverter converter.
  - CustomContentDialogStyle style.
  - CustomIconButtonStyle style.
  - CustomListViewItemStyle style.
