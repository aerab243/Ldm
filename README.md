# LDM - Like Download Manager

[![Version](https://img.shields.io/badge/version-1.0.1-blue.svg)](https://github.com/aerab243/ldm)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.5+-red.svg)](https://qt.io)
[![C++](https://img.shields.io/badge/C++-20-orange.svg)](https://isocpp.org)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/aerab243/ldm)

🚀 **A powerful, modern download manager with an Internet Download Manager (IDM) style interface, built with Qt 6 and C++20.**

![LDM Interface](docs/images/ldm-interface-preview.png)

## ✨ Features

### 🎯 Core Download Features
- **⚡ Multi-threaded Downloads**: Split files into segments for maximum speed
- **🔄 Resume Support**: Continue interrupted downloads seamlessly
- **⏸️ Pause/Resume**: Full control over download state
- **📊 Real-time Progress**: Live speed and progress monitoring
- **📁 Smart Organization**: Auto-categorize downloads by file type
- **🔗 URL Detection**: Automatic filename extraction from URLs

### 🎨 Clean, Modern Interface
- **🖥️ IDM-Inspired Design**: Familiar interface with modern Qt6 styling
- **📋 Intuitive Toolbar**: Add URL, Resume, Stop, Delete, and Options buttons
- **📂 Category Panel**: Organize downloads (Compressed, Documents, Music, Video, Programs)
- **📈 Live Statistics**: Real-time transfer rates, progress bars, and ETA
- **💾 Download History**: Track completed and failed downloads
- **🔍 Clean Start**: Interface launches empty, ready for your downloads

### 🛠️ Available Components
- **🖼️ GUI Application**: `ldm-complete` - Full-featured graphical interface
- **💻 CLI Tool**: `ldm-cli` - Command-line interface for automation
- **🌐 Browser Integration**: `ldm-native-host` - Native messaging for browser extensions
- **📦 Automated Installer**: Cross-platform installation scripts
- **🧹 Maintenance Tools**: Clean, uninstall, and update scripts

### 🔧 Technical Features
- **🏗️ Modern C++20**: Latest language standards and best practices
- **🎨 Qt 6 Framework**: Cross-platform native GUI with modern APIs
- **🌐 libcurl Integration**: Reliable HTTP/HTTPS download engine
- **🔒 OpenSSL Support**: Secure connections and encryption
- **📊 SQLite Database**: Efficient download history storage
- **🚀 CMake Build System**: Modern, cross-platform build configuration
- **📦 Automated Installation**: One-command install for all Linux distributions
- **🧹 Clean Architecture**: Single main.cpp with organized modular structure

## 📦 Quick Start

### 🚀 Automated Installation (Recommended)

LDM provides automated installation scripts that work on all major Linux distributions:

```bash
# Clone the repository
git clone https://github.com/aerab243/ldm.git
cd ldm

# Run the automated installer (detects your Linux distribution automatically)
./install.sh

# Launch LDM
ldm-complete
```

The installer automatically:
- ✅ Detects your Linux distribution (Ubuntu, Fedora, Arch, openSUSE, etc.)
- ✅ Installs all required dependencies
- ✅ Builds LDM using CMake
- ✅ Installs system-wide with desktop integration

### 🐧 Manual Installation (Alternative)

If you prefer manual installation:

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev \
                 libcurl4-openssl-dev libssl-dev libsqlite3-dev

git clone https://github.com/aerab243/ldm.git
cd ldm
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

#### Fedora/RHEL
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qttools-devel \
                libcurl-devel openssl-devel sqlite-devel

git clone https://github.com/aerab243/ldm.git
cd ldm
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

#### Arch Linux
```bash
sudo pacman -S gcc cmake qt6-base qt6-tools curl openssl sqlite

git clone https://github.com/aerab243/ldm.git
cd ldm
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 📖 Usage Guide

### 🚀 Getting Started

1. **Launch LDM**: Run `ldm-complete` - starts with a clean, empty interface ready for your downloads
2. **Add Your First Download**: Click "Add URL" or press `Ctrl+N` to begin downloading
3. **Configure Settings**: Access via "Options" menu for paths, connections, and preferences
4. **Manage Downloads**: Use toolbar buttons for resume, pause, stop, and delete operations
5. **Monitor Progress**: View real-time statistics in the status bar and download details panel

### 💡 Key Features

#### 📥 Adding Downloads
- **Single URL**: Click "Add URL" and paste the download link
- **Batch URLs**: Use Link Grabber to extract multiple links
- **Auto-detection**: Filename and category detected automatically
- **Custom Settings**: Configure connections, save path, and priority

#### ⚡ Download Control
- **Start/Resume**: Begin or continue downloads
- **Pause**: Temporarily stop downloads (can resume later)
- **Stop**: Permanently halt downloads
- **Delete**: Remove downloads from list (optionally delete files)

#### 📊 Monitoring
- **Real-time Progress**: See speed, ETA, and completion percentage
- **Global Statistics**: Overall download speed and progress
- **History Tracking**: View completed and failed downloads

### 🔧 Advanced Usage

#### ⏰ Scheduling Downloads
```
Tools → Scheduler → Add Schedule
```
- Set specific start times for downloads
- Configure recurring download schedules
- Manage bandwidth during peak hours

#### 🎯 Queue Management
```
Tools → Queue Manager
```
- Create custom download queues
- Set priority levels (High, Normal, Low)
- Control concurrent download limits

#### 🕷️ Link Grabbing
```
Tools → Link Grabber
```
- Extract all downloadable links from web pages
- Filter by file type and size
- Bulk add to download queue

## ⚙️ Configuration

### 🔧 Settings Overview

LDM uses Qt's standard settings system for configuration. Settings are automatically saved and restored between sessions.

#### 📁 General Settings
- **Download Directory**: Configurable default save location (set via Options dialog)
- **Interface Language**: Automatic system language detection
- **Window Geometry**: Remembers window size and position

#### 🌐 Network Settings
- **Connection Timeout**: Configurable timeout for network operations
- **User Agent**: Customizable browser identification string
- **SSL/TLS Verification**: Secure connection validation

#### 🎨 Interface Settings
- **Qt6 Theme**: Follows system theme with IDM-inspired styling
- **System Tray**: Minimizable to tray with notifications
- **Clean Startup**: Interface launches empty, no demo data

### 📝 Configuration Files

LDM stores configuration using Qt's standard locations:

```
~/.config/aerab243/LDM.conf          # Main application settings
~/.local/share/LDM/                   # Application data directory
├── downloads.db                      # SQLite database for download history
├── logs/                             # Application log files
└── cache/                            # Temporary cache files
```

**Note**: Configuration files are created automatically on first run. No manual setup required.

## 🏗️ Building from Source

### 📋 Prerequisites

**Required Dependencies:**
- **Qt 6.2+** (Core, Widgets, Network, Sql, HttpServer, Concurrent)
- **C++20 compatible compiler** (GCC 11+, Clang 13+)
- **libcurl 7.60+** - HTTP/HTTPS client library
- **SQLite 3.0+** - Database storage
- **FFmpeg** (libavcodec, libavformat, libavutil, libswscale) - Media processing
- **OpenSSL 1.1+** - SSL/TLS encryption
- **CMake 3.16+** - Build system

**Optional Dependencies:**
- **Qt6 Charts** - Enhanced statistics visualization

**Development Tools:**
- **pkg-config** - Library detection
- **make** - Build automation
- **Qt6 MOC** (automatically handled by CMake)

### 🔨 Build Options

#### 🚀 CMake Build (Recommended)
```bash
cd ldm
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..    # Release build
cmake -DCMAKE_BUILD_TYPE=Debug ..      # Debug build
make -j$(nproc)

# Run tests
ctest
```

#### 🛠️ Automated Build Scripts
```bash
cd ldm

# Full automated build and install
./install.sh

# Clean build artifacts
./clean.sh
```

### 📦 Installation

#### Automated Installation (Recommended)
```bash
# Install system-wide
./install.sh

# Uninstall completely
sudo ./uninstall.sh
```

#### Manual Installation
```bash
# System-wide installation
sudo make install

# Uninstall
sudo make uninstall
```

## 🏗️ Project Structure

```
ldm/
├── desktop/                    # Main GUI application
│   ├── src/
│   │   ├── main.cpp           # Main application entry point
│   │   ├── core/              # Download engine core
│   │   │   ├── DownloadEngine.cpp    # Main download controller
│   │   │   ├── DownloadItem.cpp      # Download item model
│   │   │   ├── NetworkManager.cpp    # HTTP/HTTPS handling
│   │   │   ├── Database.cpp          # SQLite storage
│   │   │   └── ...
│   │   ├── ui/                # User interface components
│   │   │   ├── MainWindow.cpp        # Main application window
│   │   │   ├── AddUrlDialog.cpp      # URL input dialog
│   │   │   ├── DownloadTableWidget.cpp # Downloads table
│   │   │   └── ...
│   │   ├── api/               # REST API server
│   │   ├── cli/               # Command-line interface
│   │   └── utils/             # Utility functions
│   ├── resources/             # Icons, UI resources, translations
│   ├── tests/                 # Unit and integration tests
│   └── CMakeLists.txt         # CMake build configuration
├── browser-extension/          # Web browser integration (planned)
├── native-messaging/          # Browser communication host
├── docs/                      # Documentation and images
├── scripts/                   # Utility scripts
├── installer/                 # Installation packages
├── install.sh                 # Automated installer script
├── uninstall.sh               # Automated uninstaller script
├── clean.sh                   # Build cleanup script
└── README.md                  # This file
```

## 🎮 Interface Overview

LDM provides a clean, modern interface that starts empty and ready for your downloads:

```
┌─────────────────────────────────────────────────────────────┐
│ File  Downloads  Tools  Help                    [_] [□] [×] │
├─────────────────────────────────────────────────────────────┤
│ [Add URL] [Resume] [Stop] [Delete] │ [Options] [Scheduler]  │
├───────────────┬─────────────────────────────────────────────┤
│ 📁 Categories │ Downloads Table (Empty on first launch)    │
│ 📦 Compressed │ ┌─────────────────────────────────────────┐ │
│ 📄 Documents  │ │                                            │
│ 🎵 Music      │ │         Click "Add URL" to start          │
│ 🎬 Video      │ │                                            │
│ ⚙ Programs    │ └─────────────────────────────────────────┘ │
│ ✓ Finished    │                                              │
│ ⏳ Queue       │                                              │
├───────────────┴─────────────────────────────────────────────┤
│ Download Details                                            │
│ URL: [No download selected]                                 │
│ Status: Ready │ Speed: 0 KB/s │ ETA: --                     │
│ [░░░░░░░░░░░░░░░░░░░░░] 0% │ Size: 0/0 MB                   │
│ [Show Details] [Pause] [Cancel]                            │
├─────────────────────────────────────────────────────────────┤
│ Ready - No active downloads │ Total: 0 KB/s │ [░░░] 0%     │
└─────────────────────────────────────────────────────────────┘
```

### 🎯 Available Executables

After installation, LDM provides:

- **`ldm-complete`** - Main GUI application with full IDM-style interface
- **`ldm-cli`** - Command-line interface for scripting and automation
- **`ldm-native-host`** - Native messaging host for browser integration

## 🎯 Roadmap

### 🚀 Version 1.1.0 (Next Release)
- [ ] **Browser Extension**: Chrome/Firefox integration
- [ ] **Advanced Scheduling**: Recurring downloads
- [ ] **Bandwidth Management**: Time-based speed limits
- [ ] **Download Verification**: Checksum validation
- [ ] **Plugin System**: Extensible architecture

### ✅ Version 1.0.1 (Current)
- [x] **Automated Installation**: Cross-platform installer scripts for all Linux distributions
- [x] **Clean Interface**: Starts empty with no demo data - ready for user downloads
- [x] **CMake Build System**: Modern, cross-platform build configuration
- [x] **System Integration**: Desktop files, icons, and menu integration
- [x] **CLI Support**: Command-line interface available for automation
- [x] **Project Cleanup**: Single main.cpp with organized modular structure
- [x] **Maintenance Scripts**: install.sh, uninstall.sh, and clean.sh included

### 🔮 Long-term Vision
- [ ] **AI-powered Downloads**: Smart categorization
- [ ] **Streaming Integration**: YouTube, Netflix support
- [ ] **Enterprise Features**: Centralized management
- [ ] **Protocol Support**: FTP, SFTP, WebDAV
- [ ] **Mobile App**: Android/iOS companion
- [ ] **Cloud Sync**: Cross-device synchronization

## 🤝 Contributing

We welcome contributions! Here's how to get started:

### 🐛 Bug Reports
1. Check existing issues on GitHub
2. Provide detailed reproduction steps
3. Include system information and logs
4. Add screenshots if applicable

### ✨ Feature Requests
1. Search existing feature requests
2. Describe the use case clearly
3. Explain expected behavior
4. Consider implementation complexity

### 💻 Code Contributions
1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Follow** coding standards (see CONTRIBUTING.md)
4. **Write** tests for new functionality
5. **Commit** with clear messages: `git commit -m 'Add amazing feature'`
6. **Push** to your branch: `git push origin feature/amazing-feature`
7. **Create** a Pull Request

### 📝 Code Style
- **C++20** modern standards
- **Qt6** framework with modern APIs
- **CMake** build system
- **Google** C++ style guide
- **Doxygen** documentation
- **Unit tests** for new features
- **Automated** installation and cleanup scripts

## 📋 System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu 20.04+, Fedora 34+, Arch Linux, etc.)
- **CPU**: x64 architecture, 1 GHz or faster
- **RAM**: 512 MB minimum, 1 GB recommended
- **Storage**: 100 MB free space
- **Display**: 1024x768 resolution minimum

### Dependencies
- **Qt6** (Core, Widgets, Network) - GUI framework
- **libcurl** - HTTP/HTTPS client library
- **OpenSSL** - SSL/TLS encryption
- **SQLite3** - Database storage
- **CMake** 3.16+ - Build system
- **GCC/Clang** - C++20 compiler

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 aerab243 (Anna-el Gerard RABENANDRASANA)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- **Internet Download Manager**: Interface design inspiration
- **Qt Project**: Excellent cross-platform framework
- **libcurl**: Robust networking library
- **OpenSSL**: Secure communication protocols
- **SQLite**: Embedded database engine
- **CMake**: Modern build system
- **Developer**: aerab243 (Anna-el Gerard RABENANDRASANA)
- **Contributors**: Everyone who helped build LDM
- **Linux Community**: For making cross-platform development possible

## 📞 Support

### 💬 Community Support
- **GitHub Issues**: [Report bugs](https://github.com/aerab243/ldm/issues)
- **Discussions**: [Ask questions](https://github.com/aerab243/ldm/discussions)
- **Wiki**: [Documentation](https://github.com/aerab243/ldm/wiki)

### 📧 Contact
- **Email**: aerabenandrasana@gmail.com
- **Developer**: aerab243 (Anna-el Gerard RABENANDRASANA)

### 💼 Professional Support
- **Commercial Licensing**: Available for enterprise use
- **Custom Development**: Tailored solutions
- **Support Contracts**: Priority assistance
- **Training**: Developer workshops and documentation

## 🌟 Star History

[![Star History Chart](https://api.star-history.com/svg?repos=aerab243/ldm&type=Date)](https://star-history.com/#aerab243/ldm&Date)

## 📊 Statistics

![GitHub repo size](https://img.shields.io/github/repo-size/aerab243/ldm)
![GitHub language count](https://img.shields.io/github/languages/count/aerab243/ldm)
![GitHub top language](https://img.shields.io/github/languages/top/aerab243/ldm)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/aerab243/ldm)
![GitHub last commit](https://img.shields.io/github/last-commit/aerab243/ldm)
![GitHub contributors](https://img.shields.io/github/contributors/aerab243/ldm)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![C++ Version](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Qt Version](https://img.shields.io/badge/Qt-6.2+-green.svg)
![CMake Version](https://img.shields.io/badge/CMake-3.16+-yellow.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)

---

<div align="center">
  <h3>🚀 Built with ❤️ by aerab243</h3>
  <p><em>"Clean, automated download management for Linux - Qt6, CMake, and cross-platform installation."</em></p>

  [⭐ Star this repo](https://github.com/aerab243/ldm) •
  [🐛 Report Bug](https://github.com/aerab243/ldm/issues) •
  [✨ Request Feature](https://github.com/aerab243/ldm/issues) •
  [💬 Join Community](https://github.com/aerab243/ldm/discussions) •
  [📖 Documentation](https://github.com/aerab243/ldm/wiki)
</div>
