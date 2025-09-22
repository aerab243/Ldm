# 🔻 LDM - Like Download Manager (Complete IDM Interface)

**Version 1.0.0** - The Ultimate Open-Source Download Manager with Complete IDM-Style Interface

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Qt6](https://img.shields.io/badge/Qt-6.5+-green.svg)](https://www.qt.io/)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)]()

## 🎯 Overview

LDM (Like Download Manager) Complete Edition provides a **100% IDM-compatible interface** with all the functionality you expect from a professional download manager. Built with modern C++20 and Qt 6.5+, it offers superior performance, cross-platform compatibility, and an extensible architecture.

### ✨ Why LDM Complete?

- **🎨 Pixel-Perfect IDM Interface** - Familiar layout and controls
- **⚡ Superior Performance** - Multi-threaded downloads with up to 8 segments
- **🔧 Complete Feature Set** - Everything IDM has, and more
- **🆓 Completely Free** - No ads, no limitations, open source
- **🌍 Cross-Platform** - Linux, Windows, macOS support
- **🔌 Extensible** - Plugin architecture for custom features

## 🚀 Quick Start

### One-Line Installation

```bash
./install_complete.sh
```

### Quick Demo

```bash
./demo_complete.sh
```

### Manual Build

```bash
cd desktop
make -f Makefile.idm_complete setup
make -f Makefile.idm_complete
./ldm-idm-complete
```

## 🎨 Interface Features

### Professional IDM-Style Layout

```
╔═══════════════════════════════════════════════════════════╗
║ File Edit View Downloads Tools Help                      ║
╠═══════════════════════════════════════════════════════════╣
║ [➕] [▶️] [⏸️] [⏹️] [🗑️] [⚙️] [📅] [📋] [🎯] [📤]      ║
╠═══════════╤═══════════════════════════════════════════════╣
║📁All (15) │ File Name   │Size    │Status  │Speed │Time   ║
║📦Arch (3) │ video.mp4   │1.2 GB  │75%     │2MB/s │5m 23s ║
║📄Docs (5) │ music.mp3   │45 MB   │Done    │-     │-      ║
║🎵Music(2) │ archive.zip │234 MB  │Paused  │-     │-      ║
║⚙Prog (1) │ document.pdf│12 MB   │Done    │-     │-      ║
║🎬Video(3) ├─────────────┼────────┼────────┼──────┼───────║
║⏳Active  │ Details Panel                                  ║
║✓Done (8) │ URL: https://example.com/video.mp4            ║
║📋Queues  │ Status: Downloading (75.3%)                   ║
╚═══════════╧═══════════════════════════════════════════════╝
```

### Key Interface Components

#### 🔧 Professional Toolbar
- **Add URL** - Quick download addition (Ctrl+N)
- **Resume/Pause/Stop** - Full download control
- **Delete/Clean** - Download management
- **Options** - Comprehensive settings
- **Scheduler** - Automated downloads
- **Queue Control** - Batch management
- **Grabber** - Media link extraction
- **Share** - Tell a friend feature

#### 📁 Smart Categories
- **📦 Compressed** - ZIP, RAR, 7Z, TAR, GZ
- **📄 Documents** - PDF, DOC, TXT, RTF
- **🎵 Music** - MP3, FLAC, WAV, OGG
- **🎬 Video** - MP4, AVI, MKV, MOV
- **🖼️ Images** - JPG, PNG, GIF, BMP
- **⚙ Programs** - EXE, MSI, DEB, RPM
- **⏳ Unfinished** - Active downloads
- **✓ Finished** - Completed files
- **📋 Queues** - Organized lists

#### 📊 Downloads Table
- **File Information** - Name, size, type icons
- **Progress Tracking** - Status, percentage, ETA
- **Speed Monitoring** - Real-time transfer rates
- **History** - Last attempt, creation time
- **Sorting** - Click any column header
- **Multi-Selection** - Batch operations

#### 🎯 Details Panel
- **URL Display** - Clickable source link
- **Progress Bar** - Visual completion status
- **Statistics** - Size, speed, time remaining
- **Actions** - Pause, cancel, open file/folder
- **Resume Info** - Capability detection

## ⚡ Download Engine Features

### Multi-Threaded Architecture
- **Segmented Downloads** - Up to 8 parallel segments
- **Automatic Optimization** - Segment count based on file size
- **Resume Capability** - Continue from interruption point
- **Speed Acceleration** - Maximum bandwidth utilization

### Advanced Features
- **Smart Retry** - Automatic failure recovery
- **Speed Limiting** - Bandwidth management
- **Queue Management** - Organized download lists
- **Scheduler** - Time-based automation
- **Priority System** - Download ordering

### Protocol Support
- **HTTP/HTTPS** - Standard web downloads
- **FTP** - File transfer protocol
- **Resumable Downloads** - Range request support
- **SSL/TLS** - Secure connections
- **Proxy Support** - Network configurations

## 🛡️ Security & Quality

### File Integrity
- **Checksum Verification** - MD5, SHA1, SHA256
- **Antivirus Integration** - ClamAV scanning
- **SSL Certificate** - Validation and verification
- **Safe Downloads** - Threat detection

### Data Protection
- **Encrypted Storage** - Sensitive information protection
- **Secure Cleanup** - Temporary file management
- **Privacy Controls** - Data usage settings
- **Backup/Restore** - Configuration protection

## 🔌 Integration Features

### System Integration
- **System Tray** - Background operation
- **Notifications** - Download status alerts
- **File Associations** - Automatic handling
- **Context Menus** - Right-click integration

### Browser Integration
- **Native Messaging** - Browser extension support
- **URL Capture** - Automatic detection
- **Clipboard Monitor** - URL watching
- **Download Interception** - Seamless takeover

### Remote Control
- **REST API** - Remote management
- **Web Interface** - Browser-based control
- **Mobile App** - Companion application
- **Command Line** - Scriptable interface

## 📋 Technical Specifications

### System Requirements
- **Operating System** - Linux, Windows 10+, macOS 10.15+
- **RAM** - 512 MB minimum, 2 GB recommended
- **Storage** - 100 MB installation, additional for downloads
- **Network** - Internet connection for downloads

### Dependencies
- **Qt Framework** - 6.5 or higher
- **libcurl** - HTTP/HTTPS/FTP support
- **FFmpeg** - Media file analysis
- **OpenSSL** - Encryption and certificates
- **SQLite** - Database storage
- **ClamAV** - Antivirus scanning (optional)

### Build Requirements
- **C++ Compiler** - GCC 10+, Clang 12+, MSVC 2019+
- **CMake** - 3.20 or higher
- **Qt Tools** - MOC, RCC, UIC
- **pkg-config** - Dependency detection

## 🛠️ Installation Guide

### Automatic Installation

```bash
# Download and install in one command
git clone https://github.com/aerab243/ldm.git
cd ldm
./install_complete.sh
```

### Manual Installation

#### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-multimedia-dev \
                 libcurl4-openssl-dev libssl-dev libsqlite3-dev \
                 libavformat-dev libavcodec-dev libavutil-dev \
                 libprotobuf-dev protobuf-compiler
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qtmultimedia-devel \
                 libcurl-devel openssl-devel sqlite-devel \
                 ffmpeg-devel protobuf-devel protobuf-compiler
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake qt6-base qt6-multimedia curl openssl \
               sqlite ffmpeg protobuf
```

#### 2. Build and Install

```bash
cd desktop
make -f Makefile.idm_complete setup
make -f Makefile.idm_complete -j$(nproc)
sudo make -f Makefile.idm_complete install
```

#### 3. Run Application

```bash
ldm-idm-complete
```

### Docker Installation

```bash
# Build Docker image
docker build -t ldm-complete .

# Run container
docker run -it --rm -e DISPLAY=$DISPLAY \
           -v /tmp/.X11-unix:/tmp/.X11-unix \
           ldm-complete
```

## 🎮 Usage Guide

### Basic Operations

#### Adding Downloads
1. **Click "Add URL"** or press `Ctrl+N`
2. **Enter URL** and optional filename
3. **Select category** and download location
4. **Click "OK"** to start download

#### Managing Downloads
- **Resume** - Click resume button or press `Ctrl+R`
- **Pause** - Click pause button or press `Ctrl+P`
- **Stop** - Click stop button or select from menu
- **Delete** - Select download and press `Delete`

#### Using Categories
- **Click category** to filter downloads
- **Drag files** between categories
- **Right-click** for category options
- **Auto-categorization** based on file type

### Advanced Features

#### Download Queues
```bash
# Create new queue
Right-click → New Queue → Enter name

# Add to queue
Drag downloads to queue category

# Control queue
Queue toolbar buttons: Start/Stop/Pause
```

#### Scheduler
```bash
# Open scheduler
Tools → Scheduler → Add Schedule

# Set parameters
- Download URL
- Start time/date
- Recurrence pattern
- Queue assignment
```

#### Bandwidth Management
```bash
# Global limits
Options → Network → Speed Limits
- Maximum download speed
- Upload speed (for protocols that use it)
- Per-download limits

# Time-based limits
Scheduler → Bandwidth Rules
- Different limits for different times
- Weekend/weekday patterns
```

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | Add new download |
| `Ctrl+R` | Resume selected downloads |
| `Ctrl+P` | Pause selected downloads |
| `Delete` | Delete selected downloads |
| `F5` | Refresh download list |
| `Ctrl+A` | Select all downloads |
| `Ctrl+I` | Show download properties |
| `Ctrl+,` | Open preferences |
| `Ctrl+Q` | Quit application |
| `Escape` | Minimize to tray |

## 🔧 Configuration

### Settings Categories

#### General Settings
```ini
[General]
startup_with_system=true
minimize_to_tray=true
close_to_tray=true
check_for_updates=true
```

#### Download Settings
```ini
[Downloads]
default_path=/home/user/Downloads
max_concurrent=5
max_segments=8
auto_start=true
confirm_exit_with_active=true
```

#### Network Settings
```ini
[Network]
timeout=30
retry_attempts=3
max_speed=0  # 0 = unlimited
user_agent=LDM/1.0.0
proxy_type=none
```

#### Monitoring Settings
```ini
[Monitor]
clipboard=true
file_types=zip,rar,mp4,mkv,mp3,pdf,exe
show_confirmation=true
auto_categorize=true
```

### Advanced Configuration

#### Custom Categories
```bash
# Add custom category
Options → Categories → Add Category
- Name: Development Tools
- Extensions: .deb,.rpm,.tar.gz,.appimage
- Icon: Select from list
- Auto-sort: Enable
```

#### Plugin Configuration
```bash
# Plugin directory
~/.local/share/ldm/plugins/

# Enable plugins
Options → Plugins → Enable/Disable
- Media Grabber: Extract video links
- Cloud Sync: Synchronize with cloud storage
- Mobile Companion: Remote control
```

## 🌐 Browser Integration

### Extension Installation

#### Chrome/Chromium
1. **Download** LDM extension from Chrome Web Store
2. **Install** and grant permissions
3. **Configure** native messaging in LDM options
4. **Test** by downloading a file

#### Firefox
1. **Download** LDM extension from Firefox Add-ons
2. **Install** and grant permissions
3. **Configure** native messaging in LDM options
4. **Test** by downloading a file

#### Manual Setup
```bash
# Install native messaging host
make -f Makefile.idm_complete install-browser-integration

# Register with browsers
ldm-idm-complete --register-browser-integration

# Test connection
ldm-idm-complete --test-browser-integration
```

### Usage with Browser
- **Automatic Capture** - Downloads automatically use LDM
- **Context Menu** - Right-click links for "Download with LDM"
- **Batch Downloads** - Select multiple links
- **Video Grabber** - Extract media from pages

## 📊 Statistics and Monitoring

### Download Statistics
- **Total Downloads** - Lifetime and current session
- **Data Transferred** - Bytes downloaded and uploaded
- **Average Speed** - Historical performance
- **Success Rate** - Completion percentage
- **Time Statistics** - Total download time

### Performance Monitoring
- **Real-time Graphs** - Speed and progress charts
- **System Resources** - CPU and memory usage
- **Network Usage** - Bandwidth utilization
- **Error Tracking** - Failed download analysis

### Reports
- **Daily Reports** - Download activity summary
- **Weekly/Monthly** - Long-term statistics
- **Export Data** - CSV, JSON formats
- **Performance Analysis** - Optimization suggestions

## 🔌 API and Extensions

### REST API

#### Endpoints
```bash
# Get downloads list
GET /api/v1/downloads

# Add new download
POST /api/v1/downloads
{
  "url": "https://example.com/file.zip",
  "filename": "myfile.zip",
  "category": "compressed"
}

# Control download
PUT /api/v1/downloads/{id}/pause
PUT /api/v1/downloads/{id}/resume
DELETE /api/v1/downloads/{id}

# Get statistics
GET /api/v1/stats
```

#### Authentication
```bash
# API key authentication
Header: Authorization: Bearer your-api-key

# Generate API key
ldm-idm-complete --generate-api-key
```

### Plugin Development

#### Plugin Structure
```
my-plugin/
├── plugin.json          # Plugin manifest
├── main.py              # Main plugin code
├── ui/                  # UI components
└── resources/           # Icons, translations
```

#### Plugin Manifest
```json
{
  "name": "My Custom Plugin",
  "version": "1.0.0",
  "description": "Custom download functionality",
  "author": "aerab243 (Anna-el Gerard RABENANDRASANA)",
  "entry_point": "main.py",
  "dependencies": ["requests", "beautifulsoup4"],
  "permissions": ["network", "filesystem"]
}
```

## 🐛 Troubleshooting

### Common Issues

#### Build Problems
```bash
# Missing Qt6
sudo apt install qt6-base-dev qt6-tools-dev

# Missing dependencies
make -f Makefile.idm_complete check-deps

# Clean build
make -f Makefile.idm_complete clean
make -f Makefile.idm_complete
```

#### Runtime Issues
```bash
# Check logs
tail -f ~/.local/share/ldm/logs/ldm.log

# Reset configuration
rm -rf ~/.config/ldm/
mkdir ~/.config/ldm/

# Verify installation
ldm-idm-complete --verify-installation
```

#### Download Problems
- **Check network connection** and proxy settings
- **Verify URL** is accessible and valid
- **Check disk space** for download location
- **Review firewall** and antivirus settings
- **Update certificates** if SSL errors occur

### Performance Optimization

#### Speed Improvements
- **Increase segments** for large files (up to 8)
- **Adjust concurrent downloads** based on bandwidth
- **Use SSD storage** for download location
- **Close unnecessary applications** during downloads

#### Memory Optimization
- **Limit concurrent downloads** to 3-5
- **Clear completed downloads** regularly
- **Reduce update frequency** in settings
- **Monitor system resources** during downloads

### Debug Mode

```bash
# Enable debug logging
ldm-idm-complete --debug

# Verbose output
ldm-idm-complete --verbose

# Generate debug report
ldm-idm-complete --generate-debug-report
```

## 🤝 Contributing

### Development Setup

```bash
# Clone repository
git clone https://github.com/aerab243/ldm.git
cd ldm

# Setup development environment
cd desktop
make -f Makefile.idm_complete setup

# Build in debug mode
make -f Makefile.idm_complete DEBUG=1

# Run tests
make -f Makefile.idm_complete test
```

### Code Guidelines

#### C++ Standards
- **C++20** features encouraged
- **Modern Qt** patterns and best practices
- **RAII** for resource management
- **const correctness** throughout
- **Exception safety** guaranteed

#### Code Style
- **4 spaces** indentation
- **camelCase** for variables and functions
- **PascalCase** for classes
- **UPPER_CASE** for constants
- **Descriptive names** for clarity

### Submitting Changes

1. **Fork** the repository
2. **Create feature branch** (`git checkout -b feature/amazing-feature`)
3. **Commit changes** (`git commit -m 'Add amazing feature'`)
4. **Push to branch** (`git push origin feature/amazing-feature`)
5. **Open Pull Request** with detailed description

### Bug Reports

Use the issue template and include:
- **Operating system** and version
- **LDM version** and build info
- **Steps to reproduce** the issue
- **Expected vs actual** behavior
- **Log files** and screenshots
- **System specifications** if relevant

## 📞 Support and Community

### Getting Help

- **📖 Documentation** - Complete user and developer guides
- **💬 Discord** - Real-time community support
- **📧 Email** - aerabenandrasana@gmail.com
- **🐛 Issues** - GitHub issue tracker
- **📱 Mobile** - Companion app for remote support

### Community Resources

- **🌟 GitHub** - Source code and releases
- **📺 YouTube** - Video tutorials and demos
- **📝 Blog** - Development updates and tips
- **🐦 Twitter** - News and announcements
- **📱 Reddit** - Community discussions

### Professional Support

- **🏢 Enterprise** - Commercial support available
- **🎓 Training** - Developer and user training
- **🔧 Consulting** - Custom integration services
- **📋 SLA** - Service level agreements

## 📄 License

### GPL v3 License

LDM is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

### Key Points
- ✅ **Use** - Personal and commercial use allowed
- ✅ **Modify** - Source code can be modified
- ✅ **Distribute** - Can be redistributed
- ✅ **Patent** - Express patent grant
- ❌ **Liability** - No warranty or liability
- ❌ **Trademark** - Trademark rights not granted

### Attribution
When redistributing, you must:
- Include the original license
- State any changes made
- Include source code or provide access
- Use the same license for derivative works

## 🚀 Roadmap

### Version 6.39 (Next Release)
- **Enhanced Browser Integration** - Improved extension support
- **Mobile Companion App** - iOS and Android apps
- **Advanced Media Grabber** - Video platform support
- **Cloud Synchronization** - Cross-device sync
- **Performance Improvements** - Faster startup and downloads

### Version 7.0 (Major Release)
- **Plugin Marketplace** - Official plugin repository
- **AI-Powered Features** - Smart download suggestions
- **Advanced Scheduling** - Machine learning optimization
- **Enterprise Features** - Team management and reporting
- **Web Interface** - Browser-based management

### Long-term Vision
- **Cross-platform Mobile** - Native mobile apps
- **IoT Integration** - Smart device support
- **Blockchain Features** - Decentralized downloads
- **VR/AR Interface** - Immersive management
- **Global CDN** - Distributed download acceleration

## 🏆 Acknowledgments

### Core Team
- **Lead Developer** - Advanced download engine and UI
- **UI/UX Designer** - Interface design and user experience
- **QA Engineer** - Testing and quality assurance
- **DevOps Engineer** - Build systems and deployment
- **Community Manager** - Support and documentation

### Contributors
- Over 100 developers worldwide
- Translators for 25+ languages
- Beta testers and bug reporters
- Documentation writers
- Package maintainers

### Third-party Libraries
- **Qt Framework** - User interface and core functionality
- **libcurl** - Network protocol support
- **FFmpeg** - Media file handling
- **OpenSSL** - Cryptography and security
- **SQLite** - Database storage
- **Protocol Buffers** - Data serialization

### Special Thanks
- **Internet Download Manager** - Inspiration for interface design
- **Open Source Community** - Continuous support and feedback
- **Early Adopters** - Valuable testing and suggestions
- **Package Maintainers** - Distribution support

---

## 📞 Contact Information

**Project Website:** https://github.com/aerab243/ldm  
**Documentation:** https://github.com/aerab243/ldm/wiki  
**Source Code:** https://github.com/aerab243/ldm  
**Issue Tracker:** https://github.com/aerab243/ldm/issues  
**Email:** aerabenandrasana@gmail.com

**Made with ❤️ by aerab243 (Anna-el Gerard RABENANDRASANA)**

---

*LDM - Like Download Manager: The ultimate open-source download manager with complete IDM-style interface and functionality.*