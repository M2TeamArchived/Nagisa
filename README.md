![Logo](Logo.png)
# Nagisa - An open source file transfer utility on Universal Windows Platform.
 
[![release.badge]][release.link]
[![latestdownloads.badge]][latestdownloads.link]
[![totaldownloads.badge]][totaldownloads.link]
[![license.badge]][license.link]

![Screenshot](Screenshot.png)

**Warning: This project is in early stages of development, the final product may differ from what you see now.**

Nagisa is an open source file transfer utility (UWP), distributed under the MIT License. 

Nagisa is mainly developed in C++ 17, with WinRT API, Win32 API, WRL, STL, C++/CX and C++/WinRT. Thus it has better efficiency and consumes less storage space. 

We need help to translate Nagisa into native languages that have not been supported.

# Development Roadmap
**For more information, please 
[click here](https://github.com/Project-Nagisa/Nagisa/issues/6)**

Here are the features we will implement for Nagisa in the future. (The **blod**
texts represent the features we **have not** implemented yet.)

- Assassin Transfer Engine (An alternative to Windows.Networking.BackgroundTransfer)
  - Support background download. 
  - Support resuming broken/dead downloads.
  - Support **multi-thread** multi-task download. 
  - Support HTTP 1.1 and **HTTP/2** protocol for HTTP and HTTPS support.
  - Support FTP, FTPS and **SFTP**.
  - Support WebSocket and WebSocket Secure.
  - Support **BitTorrent, Magnet and ED2K.**
  - Support **downloading files from multiple URIs**.
- Experience
  - Support **providing HASH value for downloaded files**.
  - Support **get download URI from QR code and texts in images**.
  - Support **establishing download daemons on IoT devices (like Raspberry Pi).** 
  - Support **pushing a download task to the other devices**.

**All kinds of contributions will be appreciated. All suggestions, pull requests and 
issues are welcome.**

# System Requirement
- Supported OS: Windows 10 Build 10240 or later
- Supported Platforms: x86, x86-64(AMD64), ARM and ARM64.

# Features
- Supported Transfer Protocols: HTTP, HTTPS, FTP, FTPS, WebSocket and WebSocket
  Secure. (Temporarily based on Windows.Networking.BackgroundTransfer)
- Multi-language Support: English, Spanish and Simplified Chinese.
- Support background download. 
  (Temporarily based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily based on Windows.Networking.BackgroundTransfer)
- Support single-thread multi-task download.
  (Temporarily based on Windows.Networking.BackgroundTransfer)

# How to help to develop Nagisa
- Direct contributions
  - **Create pull requests directly.**
  - Please send e-mails to Mouri_Naruto@Outlook.com if you have any
    questions.
- Feedback suggestions and bugs.

# License
Nagisa (not including third-party libraries) is distributed under the MIT License. 
For more information, please read [Nagisa's License](LICENSE).

# Third-party librarie(s)
- [OpenSSL](ThirdParty/OpenSSL/README.md)

# Documents
- [Nagisa's Changelog](Changelog.md)
- [Nagisa's Relevant People](People.md)
- [Nagisa's Privacy Policy](Privacy.md)
- [Nagisa's Windows Store](https://www.microsoft.com/store/apps/9NFW53N9MFJR)

[release.badge]: https://img.shields.io/github/release/Project-Nagisa/Nagisa.svg
[release.link]: https://github.com/Project-Nagisa/Nagisa/releases/latest
[latestdownloads.badge]: https://img.shields.io/github/downloads/Project-Nagisa/Nagisa/latest/total.svg
[latestdownloads.link]: https://github.com/Project-Nagisa/Nagisa/releases/latest
[totaldownloads.badge]: https://img.shields.io/github/downloads/Project-Nagisa/Nagisa/total.svg
[totaldownloads.link]: https://github.com/Project-Nagisa/Nagisa/releases
[license.badge]: https://img.shields.io/github/license/Project-Nagisa/Nagisa.svg
[license.link]: LICENSE
