# Nagisa - Changelog

## Notice
- This is the changelog about project Nagisa.
- For more information on the names of persons mentioned here, please read 
  People.md.

## Changelog

**Nagisa 0.1 [Build 9]**
- Merge the changelog and update the readme.
- Add M2ConvertByteSizeToString function.
- Improve the Implemention of TransferManager.
- Fix a bug about searching tasks from the task list.
- Remove Uint64ToByteSpeedStringConverter.
- Remove ITransferTask::Cancel method.
- Add ITransferTask::CancelAsync method.
- Fix a bug when using M2AsyncCreate function. (Thanks to MichaelSuen)
- Fix a bug when cancelling the task.
- Fix a bug when changing the task status.
- Rename Uint64ToByteSizeStringConverter as Uint64ByteSizeToStringConverter.
- Rename RemainTimeToTimeStringConverter as Uint64RemainTimeToStringConverter.
- Add TaskStatusToVisibleConverter.
- Remove StatusErrorToVisibleConverter.
- Remove StatusPausedToVisibleConverter.
- Remove StatusRunningToVisibleConverter.
- Remove unused assets.

### Nagisa 0.1 [Build 8]
**New Features**
- Supported Transfer Protocols: HTTP, HTTPS, FTP, FTPS, WebSocket and WebSocket
  Secure.
- Muitl-language Support: English and Simplified Chinese.
- Support background download. 
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support single-threaded multi-tasking download.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support for searching tasks from the task list.
- The color themes of Nagisa follows the options in Windows Settings.

**Added APIs**
- Assassin
  - TransferManager class or ITransferManager interface.
    - Version property.
	- SearchFilter property.
	- GetTasksAsync method.
	- AddTask method.
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
	- Cancel method.
	- NotifyPropertyChanged method.
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
  - M2FormatString function.
  - M2GetTickCount function.
  - M2PathFindFileName function.
  - M2::CThread class.
  - M2::CObject template.
  - M2::CHandle class.
  - M2::CComObject class.
- XAML
  - Uint64ToDoubleConverter converter.
  - Uint64ToByteSizeStringConverter converter.
  - Uint64ToByteSpeedStringConverter converter.
  - StorageFileToFileNameConverter converter.
  - StatusErrorToVisibleConverter converter.
  - StatusPausedToVisibleConverter converter
  - StatusRunningToVisibleConverter converter.
  - RemainTimeToTimeStringConverter converter.
  - TaskListEmptyToVisibilityConverter converter.
  - CustomContentDialogStyle style.
  - CustomIconButtonStyle style.
  - CustomListViewItemStyle style.
