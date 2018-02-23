# Nagisa - Changelog

## Notice
- This is the changelog about project Nagisa.
- For more information on the names of persons mentioned here, please read 
  People.md.

## Changelog

**Nagisa 0.3 [Build 24]**
- Add M2MakeUTF16String function.
- Add M2MakeUTF8String function.
- Add M2GetPointer function.
- Add M2MakeIBuffer function.
- Add M2MakeCXString function.
- Add M2GetNumberOfHardwareThreads function.

**Nagisa 0.3 [Build 23]**
- Remove ARM64 support because the Microsoft don't provide the ARM64 Visual C++
  Runtime for UWP.
- Fix a bug that could cause the app to crash.
- Adjust the MainPage UI. (Thanks to suwakowww.)
- According to system requirements for Windows 8, 8.1 and 10, re-enable the SSE
  and SSE2 optimization in the x86 binaries.

### Nagisa 0.2 [Build 22]
**New Changes**
- Change the icon.
- Fix several bugs and improve several implementations.
- Add configuration infrastructure.
- You can cancel task without remove it in the UI.
- You can open every task's folder in the UI.
- You can start or pause all tasks in the UI.
- You can clear task list in the UI.
- You can use the custom or last used folder as the default download folder.
- You can get the total download and total upload bandwidth in the UI.
- You can open default download folder by File Explorer in the UI.

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
**New Features**
- Supported Transfer Protocols: HTTP, HTTPS, FTP, FTPS, WebSocket and WebSocket
  Secure. (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Muitl-language Support: English and Simplified Chinese.
- Support background download. 
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support single-threaded multi-tasking download.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support for searching tasks from the task list.
- The color themes of Nagisa follows the options in Windows Settings.

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
