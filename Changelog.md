# Nagisa - Changelog

## Notice
- This is the changelog about project Nagisa.
- For more information on the names of persons mentioned here, please read 
  [People.md](People.md).

## Changelog

**Nagisa 0.4 [Build *Current*]**
- Update documents.
- Update the version number.
- Start to add the BackgroundWorker.

### Nagisa 0.4 [Build 124]
- Improve several implementations.
- Remove OpenSSL static library binaries.
- Remove AppVeyor CI support because we can't use C++/WinRT in the AppVeyor CI 
  environment currently.
- Decided to upgrade the system requirement to Windows 10, version 1703. 
- Introduce the new user interface. (Partially contributed by 鳶一雨音.)
- Improve the user experience. (Thanks to Yue Yin.)

### Nagisa 0.3 [Build 62]
- Add Multi-language Support: Spanish. (Contributed by Jhovany200.)
- Fix a bug that could cause the app to crash.
- Adjust the MainPage UI. (Thanks to suwakowww.)
- Fix a bug that you need to click the "File Name" TextBox before you download 
  a file. (Thanks to 桥月梦.)
- According to system requirements for Windows 8, 8.1 and 10, re-enable the SSE
  and SSE2 optimization in the x86 binaries.
- Add OpenSSL for implementing SSL support for the future builtin transfer 
  engine.
- Improve several implementations and add AppVeyor CI support.
- Update to Windows 10 SDK Build 17134.
- Update to C++ 17 standard.
- Refactor the Assassin with C++/WinRT.

### Nagisa 0.2 [Build 22]
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

### Nagisa 0.1 [Build 10]
- Supported Transfer Protocols: HTTP, HTTPS, FTP, FTPS, WebSocket and WebSocket
  Secure. (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Multi-language Support: English and Simplified Chinese.
- Support background download. 
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support single-threaded multi-tasking download.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support for searching tasks from the task list.
- The color themes of Nagisa follows the options in Windows Settings.
