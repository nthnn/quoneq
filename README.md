# Quoneq

![Build CI](https://github.com/nthnn/quoneq/actions/workflows/build_ci.yml/badge.svg)

Quoneq is a lightweight, multi-protocol networking library for C++ that provides a simple, unified API for a variety of network protocols based on libcurl developed specially for [n8 programming language](https://github.com/n8lang/n8). It is designed to help developers integrate network communication into their applications easily and efficiently.

## Overview

Quoneq currently supports several protocols, including:
- **FTP**: Upload, download, list directories (including recursive listings), move files, and query file/folder information.
- **HTTP**: GET and POST requests, file downloads, custom header/cookie handling, and connectivity checks.
- **SMTP**: Sending emails in plain text or HTML format with support for attachments.
- **Telnet**: Connecting to Telnet servers, sending commands, executing Telnet scripts, and negotiating Telnet options.
- **TOR**: Sending HTTP requests through the Tor network, checking Tor connectivity, and downloading files via Tor.

Additional protocols such as MQTT and RTMP are planned for future releases.

## Supported Protocols

- [x] FTP
- [x] HTTP
- [ ] MQTT
- [ ] RTMP
- [x] SMTP
- [x] Telnet
- [x] TOR

## Contribution and Feedback

Contributions and feedback are all welcome to enhance this library. If you encounter any issues, have suggestions for improvements, or would like to contribute code, please do so.

## License

Copyright 2025 - Nathanne Isip

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.