# About ff Stream Files

## Intro

- This is Documentation for Stream file
- Related:[File handler](./about_file_handler.md)

### Design

#### ChatGPT-Note:

- We use "Strategy" Design Pattern to help organize code into function feature list
  - create, write, open, muxer, play in stream_manager
  - stream_manager has 2 file_object (A/V)
  - stream_manager has 1 handlerStrategy
  - <注意設計> Function的前提條件，讓 operation 安全可用
- Inheriting behavior, which means strategy classes are coupled — this is slightly different from a typical Strategy Pattern where strategies are interchangeable. If you're okay with coupling and step-wise processing, this works well.

#### Previous-Experience

- Good decople design , can help us reuse class, instead of re-assemble
- ME: 單元應該可以被操作，達成不同功能。而非像是，根據舊元件樣貌，打造新 class 達到同樣資料的不同作用
  - EX: stream_file_object_manager , download_file_handler

#### Version

- 0.4.0 : support header file for stream file beginning, (plain-text JSON with Base64 encode)
- 0.3.0 : support stream_file_obj , init and shutdown with audio file support.
- 0.2.0 : add 3 major class
  - ffStreamHandler : global storage
  - stream_file_mgr : file operation class
  - stream_file_obj : file object for file related operations
- 0.1.0 : create wrapper class for build and update CMake Files 