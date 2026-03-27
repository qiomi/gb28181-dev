# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Complete SIP stack implementation (REGISTER, MESSAGE, INVITE/ACK/BYE)
- Full RTP packetization for H.264 and H.265
- PS payload encapsulation according to GB/T 28181-2016
- Audio support (G.711, AAC)
- TCP passive mode
- Two-way audio intercom
- PTZ control

## [1.0.0] - 2026-03-27

### Added

#### Project Infrastructure
- Initial project structure with CMake build system
- Google C++ Style code formatting with clang-format configuration
- compile_commands.json generation for IDE support
- Cross-platform support for Linux x86/x86_64 and ARM
- Build scripts for easy compilation
- Code formatting script

#### Core API
- `gb28181_dev_create()` - Create device instance
- `gb28181_dev_start()` - Start device
- `gb28181_dev_send_video()` - Send video data
- `gb28181_dev_send_audio()` - Send audio data (stub)
- `gb28181_dev_stop()` - Stop device
- `gb28181_dev_destroy()` - Destroy device

#### Configuration
- Device configuration (platform IP, device ID, credentials)
- Multi-stream configuration support
- Codec selection (H.264/H.265)
- Stream mode selection (RAW/PS)

#### Module Framework
- **Core module**: DeviceImpl class for device management
- **SIP module**: SipStack framework (ready for implementation)
- **RTP module**: RtpSession framework (ready for implementation)
- **Media module**: MediaProcessor with basic NALU extraction
- **Utils module**: Helper functions (timestamp conversion, SSRC generation, logging)

#### Documentation
- README.md - Project overview and features
- CONTRIBUTING.md - Contribution guidelines
- ARCHITECTURE.md - Detailed architecture documentation
- QUICKSTART.md - Quick start guide
- PROJECT_SUMMARY.md - Project summary and roadmap
- LICENSE - MIT License

#### Examples & Tests
- simple_push.cpp - Example video push application
- test_basic.cpp - Basic unit tests

#### GitHub Integration
- .gitignore for clean repository
- GitHub Actions CI workflow
- Issue templates (bug report, feature request)
- Repository setup

### Technical Details
- **Language**: C++17 with C interface
- **Build System**: CMake 3.14+
- **Code Style**: Google C++ Style (4 spaces indent, braces on new line)
- **License**: MIT License (open source, commercial-friendly)
- **Dependencies**: Only pthread (no commercial dependencies)

### Known Limitations
- SIP stack implementation is incomplete (framework only)
- RTP packetization needs full implementation
- PS encapsulation needs completion per GB/T 28181-2016
- Audio sending not yet implemented

---

## Version History

- **v1.0.0** (2026-03-27) - Initial release with project framework

---

*For more details, see:*
- [README.md](README.md)
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- [docs/QUICKSTART.md](docs/QUICKSTART.md)
- [docs/PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md)
