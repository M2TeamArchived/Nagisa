# Nagisa - Changelog

## Notice
- This is the changelog about project Nagisa.
- For more information on the names of persons mentioned here, please read 
  People.md.

## Changelog
**Nagisa 0.1 Preview 1 [Build 6]**
- Add ITransferManager interface.
- Add M2ThrowPlatformExceptionIfFailed, M2ThrowPlatformException and 
  M2ThrownPlatformExceptionToHResult functions.
- Rename M2SetAsyncCompletedHandler and M2SetAsyncProgressHandler as
  M2AsyncSetCompletedHandler and M2AsyncSetProgressHandler.
- Add M2::CThread class.
- Add M2AsyncCreate function; IM2AsyncController and IM2AsyncControllerEx 
  interfaces.
- Improve the implement of GetTasksAsync method in TransferManager class.

**Nagisa 0.1 Preview 1 [Build 5]**
- Improve the implement of M2ExecuteOnUIThread, M2SetAsyncCompletedHandler, 
  M2SetAsyncProgressHandler and M2AsyncWait functions.
- Add StatusErrorToVisibleConverter, StatusPausedToVisibleConverter and 
  StatusRunningToVisibleConverter.
- Add ITransferTask interface and M2::Object class. 
- Using TransferTask instaead of ITransferTask.
- Add M2GetInspectable and M2AsyncHandleCompleted functions.
- Adjust the project settings for reducing the binary size.
- Add GetTasks method in TransferManager class.
- Continue to implement the "MainPage" UI.

**Nagisa 0.1 Preview 1 [Build 4]**
- Improve the "New Task" Dialog.
- Add Pause, Resume and Cancel methods in TransferTask class.
- Add Uint64ToDoubleConverter, Uint64ToByteSizeStringConverter and 
  StorageFileToFileNameConverter.
- Continue to implement the "MainPage" UI.

**Nagisa 0.1 Preview 1 [Build 3]**
- Adjust the git exclude list.
- Add M2RemoveReference struct template; M2SetAsyncCompletedHandler, 
  M2AsyncWait, M2SetAsyncProgressHandler and M2ExecuteOnUIThread functions.
- Add TransferTask class; TransferTaskStatus enum; RequestedUri, ResultFile, 
  Status, BytesReceived and TotalBytesToReceive properties in TransferTask 
  class.
- Add GetTasksAsync and AddTask methods in TransferManager class.
- Add "New Task" Dialog.
- Improve the "About" Dialog.

**Nagisa 0.1 Preview 1 [Build 2]**
- Create the "Assassin" sub project.
- Associate the version info for binaries with the version definition header.
- Adjust the solution folders for fixing the MakePri warning.
- Add CustomContentDialogStyle, CustomConverter, CustomIconButtonStyle and 
  CustomListViewItemStyle Xaml resources.
- Add TransferManager class and TransferManager::Version property.
- Add "About" Dialog.

**Nagisa 0.1 Preview 1 [Build 1]**
- Create the solution, "Nagisa" and "Shared" sub project.
- Configure the package manifest, the project settings, and associate with my 
  Windows Developer Account.
- Add the logo asset, the version info for binaries, the version definition 
  header, the documents for the project and the ARM64 platform support.
- Remove UseDotNetNativeToolchain option to support the ARM64 platform and 
  improve the speed of compiling.
- The color themes of Nagisa follows the options in Windows Settings.
- Add the base of multi languages support.
