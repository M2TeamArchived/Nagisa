# Nagisa - Changelog

## Notice
- This is the changelog about project Nagisa.
- For more information on the names of persons mentioned here, please read 
  [People.md](People.md).
- For more information on the changes for project Nagisa API implementations, 
  please read [APIChanges.md](APIChanges.md).

## Changelog

**Nagisa 0.3 [Build *Current*]**
- Sync the Changelog.md and APIChanges.md.
- Sync the version number.
- Start to add the BackgroundWorker.
- Fix bug for M2MakeCXString function.
- Fix bug for M2FormatString function.
- Add OpenSSL for implementing SSL support.

### Nagisa 0.3 [Build 25]
- Remove ARM64 support because the Microsoft don't provide the ARM64 Visual C++
  Runtime for UWP.
- Fix a bug that could cause the app to crash.
- Adjust the MainPage UI. (Thanks to suwakowww.)
- According to system requirements for Windows 8, 8.1 and 10, re-enable the SSE
  and SSE2 optimization in the x86 binaries.
- Improve several implementations.

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
- Muitl-language Support: English and Simplified Chinese.
- Support background download. 
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support single-threaded multi-tasking download.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support for searching tasks from the task list.
- The color themes of Nagisa follows the options in Windows Settings.
