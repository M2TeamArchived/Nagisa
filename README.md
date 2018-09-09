![Logo](Logo.png)
# Nagisa - One of Download Utilities for Windows Universal Platform.
 
[![release.badge]][release.link]
[![latestdownloads.badge]][latestdownloads.link]
[![totaldownloads.badge]][totaldownloads.link]
[![license.badge]][license.link]

![Screenshot](Screenshot.png)

**Warning: This project is in the preview stage until the 1.0 is released.**

Nagisa is an open source download utility that supports multi languages. 
Running in the Windows Universal Platform, its distributed under the MIT 
License.

Nagisa is written in C++/CX and uses pure WinRT API, Win32 API, WRL and STL 
which ensures a higher execution speed and smaller program size. By optimizing
as many routines as possible without losing user friendliness, Nagisa is trying
to reduce the world carbon dioxide emissions. When using less CPU power, the PC
can throttle down and reduce power consumption, resulting in a greener 
environment.

You're encouraged to translate Nagisa into your native language if there's not 
already a translation present in the Nagisa's GitHub Repository.

> P.S. The above introduction is inspired by the Notepad++ project.

# Development Roadmap
**For more information, please 
[click here](https://github.com/Project-Nagisa/Nagisa/issues/6)**

Here are the features we will implement for Nagisa in the future. (The **blod**
text represents the features we **have not** implemented it yet.)

- Assassin Transfer Engine for replacing Windows.Networking.BackgroundTransfer
  - Support background download. 
  - Support resume broken/dead downloads.
  - Support **multi**-threaded multi-tasking download. 
  - Support HTTP 1.1 and **HTTP/2** protocol for HTTP and HTTPS support.
  - Support FTP, FTPS and **SFTP**.
  - Support WebSocket and WebSocket Secure.
  - Support **BitTorrent, Magnet and ED2K.**
  - Support **downloading a file from multiple URIs**.
- Experience
  - Support **providing HASH value for downloaded files**.
  - Support **get download URI from QR code and texts on the paper**.
  - Support **building a download server on devices like Raspberry Pi.** 
  - Support **pushing a download task to the other devices**.

**I hope someone can help this project. All suggestions, pull requests and 
issues are welcome.**

# System Requirement
- Supported OS Version: Windows 10 Build 10240 or later
- Support Platforms: x86, x86-64(AMD64), ARM and ARM64.

# Features
- Supported Transfer Protocols: HTTP, HTTPS, FTP, FTPS, WebSocket and WebSocket
  Secure. (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Multi-language Support: English and Simplified Chinese.
- Support background download. 
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support resume broken/dead downloads.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)
- Support single-threaded multi-tasking download.
  (Temporarily Based on Windows.Networking.BackgroundTransfer)

# How to support Nagisa
- Contribute
  - **Send pull request directly.**
  - You should send an e-mail to Mouri_Naruto@Outlook.com first if you have any
    doubts.
- Donate
  - Please write a note like "Nagisa donation" when donating.
  - The way to donate 
    - PayPal: https://www.paypal.me/MouriNaruto

# License
Nagisa is distributed under the MIT License except the third-party libraries. 
For more information, please read [Nagisa's License](LICENSE).

# Third-party libraries
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
