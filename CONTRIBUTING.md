# Contributing to LDM - Like Download Manager

🎉 **Thank you for considering contributing to LDM!** 🎉

We welcome contributions from everyone, whether you're fixing a bug, adding a feature, improving documentation, or just asking questions. This guide will help you get started.

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Issue Guidelines](#issue-guidelines)
- [Community](#community)

## 🤝 Code of Conduct

By participating in this project, you agree to abide by our Code of Conduct:

- **Be respectful**: Treat everyone with respect and kindness
- **Be inclusive**: Welcome newcomers and diverse perspectives
- **Be constructive**: Provide helpful feedback and suggestions
- **Be patient**: Remember that everyone is learning
- **Be professional**: Keep discussions focused and appropriate

## 🚀 Getting Started

### Prerequisites

Before contributing, make sure you have:

- **Git** installed and configured
- **GitHub account** for submitting changes
- **Development environment** set up (see below)
- **Basic understanding** of C++20 and Qt 6

### Development Setup

1. **Fork the repository**
   ```bash
   # Fork on GitHub, then clone your fork
   git clone https://github.com/aerab243/ldm.git
   cd ldm
   ```

2. **Install dependencies**
   ```bash
   # Auto-install (recommended)
   ./install.sh
   
   # Or manually based on your distro:
   # Ubuntu/Debian:
   sudo apt install build-essential qt6-base-dev qt6-tools-dev cmake
   sudo apt install libcurl4-openssl-dev libssl-dev
   
   # Fedora:
   sudo dnf install gcc-c++ make qt6-qtbase-devel qt6-qttools-devel
   sudo dnf install libcurl-devel openssl-devel
   
   # Arch:
   sudo pacman -S gcc make qt6-base qt6-tools curl openssl
   ```

3. **Build the project**
   ```bash
   cd desktop
   make -f Makefile.complete
   ```

4. **Test the build**
   ```bash
   ./ldm-complete
   # Or run the demo
   cd .. && ./demo.sh
   ```

5. **Set up development branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

## 🛠️ How to Contribute

### Types of Contributions

We welcome various types of contributions:

- **🐛 Bug Fixes**: Fix issues and improve stability
- **✨ New Features**: Add functionality and enhancements
- **📚 Documentation**: Improve guides, comments, and examples
- **🎨 UI/UX**: Enhance the interface and user experience
- **⚡ Performance**: Optimize code and improve efficiency
- **🧪 Testing**: Add tests and improve coverage
- **🔧 Build System**: Improve build process and packaging
- **🌐 Translations**: Add support for more languages

### Feature Development Workflow

1. **Check existing issues** for similar requests
2. **Create an issue** to discuss your idea
3. **Wait for approval** from maintainers
4. **Fork and create branch** for your feature
5. **Implement the feature** following our standards
6. **Test thoroughly** on multiple systems
7. **Submit pull request** with detailed description

## 📏 Coding Standards

### C++ Guidelines

**Style and Formatting:**
```cpp
// Use camelCase for variables and functions
int downloadCount = 0;
void startDownload();

// Use PascalCase for classes
class DownloadManager {
public:
    // Use descriptive names
    void addNewDownloadItem(const QString &url);
    
private:
    // Use m_ prefix for member variables
    int m_activeDownloads;
    QString m_downloadPath;
};

// Use const correctness
QString getFileName() const;
void processDownload(const DownloadItem &item);

// Use smart pointers when appropriate
std::unique_ptr<DownloadEngine> m_engine;
```

**Modern C++20 Features:**
```cpp
// Use auto when type is obvious
auto downloads = getActiveDownloads();

// Use range-based for loops
for (const auto &download : downloads) {
    process(download);
}

// Use nullptr instead of NULL
DownloadItem* item = nullptr;

// Use enum class
enum class DownloadStatus {
    Waiting,
    Downloading,
    Completed,
    Failed
};
```

### Qt Conventions

```cpp
// Use Qt naming conventions
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onDownloadFinished();
    void onButtonClicked();

private:
    QPushButton *m_startButton;
    QTableWidget *m_downloadsTable;
};

// Use Qt parent-child system
auto button = new QPushButton("Download", this);

// Use Qt signals and slots
connect(button, &QPushButton::clicked, 
        this, &MainWindow::onStartDownload);
```

### File Organization

```
src/
├── main_complete.cpp           # Main application entry
├── core/                       # Core download engine
│   ├── DownloadEngine.h/cpp   # Main download controller
│   ├── DownloadItem.h/cpp     # Download item model
│   └── NetworkManager.h/cpp   # Network operations
├── ui/                        # User interface
│   ├── MainWindow.h/cpp       # Main application window
│   ├── AddUrlDialog.h/cpp     # Add URL dialog
│   └── SettingsDialog.h/cpp   # Settings configuration
└── utils/                     # Utility functions
    ├── FileUtils.h/cpp        # File operations
    └── StringUtils.h/cpp      # String utilities
```

### Documentation Standards

```cpp
/**
 * @brief Downloads a file from the specified URL
 * 
 * This function initiates a multi-threaded download of the file
 * at the given URL, saving it to the specified local path.
 * 
 * @param url The URL of the file to download
 * @param savePath The local path where the file should be saved
 * @param connections Number of concurrent connections (1-8)
 * @return Download ID for tracking, or -1 on error
 * 
 * @throws std::invalid_argument if URL is empty or invalid
 * @throws std::runtime_error if save path is not writable
 * 
 * @example
 * ```cpp
 * int id = startDownload("https://example.com/file.zip", 
 *                        "/home/user/Downloads/file.zip", 4);
 * if (id != -1) {
 *     qDebug() << "Download started with ID:" << id;
 * }
 * ```
 */
int startDownload(const QString &url, const QString &savePath, int connections = 4);
```

## 🧪 Testing Guidelines

### Unit Testing

```cpp
// Use Qt Test framework
#include <QtTest>

class DownloadEngineTest : public QObject
{
    Q_OBJECT

private slots:
    void testAddDownload();
    void testStartDownload();
    void testPauseResume();
    void cleanup();

private:
    DownloadEngine *m_engine;
};

void DownloadEngineTest::testAddDownload()
{
    QString url = "https://httpbin.org/json";
    int id = m_engine->addDownload(url, "test.json", "/tmp");
    
    QVERIFY(id > 0);
    QVERIFY(m_engine->getDownload(id) != nullptr);
}
```

### Integration Testing

```bash
# Test build process
cd desktop
make -f Makefile.complete clean
make -f Makefile.complete

# Test application startup
timeout 10s ./ldm-complete --version

# Test basic functionality
./ldm-complete --test-mode
```

### Manual Testing Checklist

- [ ] Application starts without errors
- [ ] Add URL dialog works correctly
- [ ] Downloads can be started, paused, and resumed
- [ ] Settings dialog saves preferences
- [ ] System tray integration functions
- [ ] All toolbar buttons respond
- [ ] Category filtering works
- [ ] Application closes cleanly

## 📝 Pull Request Process

### Before Submitting

1. **Update your fork**
   ```bash
   git remote add upstream https://github.com/aerab243/ldm.git
   git fetch upstream
   git checkout main
   git merge upstream/main
   ```

2. **Create feature branch**
   ```bash
   git checkout -b feature/description-of-feature
   ```

3. **Make your changes**
   - Follow coding standards
   - Add tests for new functionality
   - Update documentation as needed

4. **Test thoroughly**
   ```bash
   # Build and test
   cd desktop
   make -f Makefile.complete clean
   make -f Makefile.complete
   ./ldm-complete
   
   # Run demo
   cd .. && ./demo.sh test
   ```

5. **Commit with clear messages**
   ```bash
   git add .
   git commit -m "feat: add multi-segment download support
   
   - Implement segment-based downloading
   - Add progress tracking per segment  
   - Update UI to show segment progress
   - Add tests for segment manager
   
   Fixes #123"
   ```

### Pull Request Template

```markdown
## Description
Brief description of changes made.

## Type of Change
- [ ] Bug fix (non-breaking change that fixes an issue)
- [ ] New feature (non-breaking change that adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing completed
- [ ] Tested on multiple platforms: [ ] Linux [ ] Windows [ ] macOS

## Screenshots (if applicable)
Add screenshots showing the changes.

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Code is commented where necessary
- [ ] Documentation updated
- [ ] No new warnings introduced
```

### Review Process

1. **Automated checks** run on your PR
2. **Maintainer review** for code quality and design
3. **Testing** on different platforms
4. **Feedback incorporation** if needed
5. **Merge** when approved

## 🐛 Issue Guidelines

### Bug Reports

Use this template for bug reports:

```markdown
**Bug Description**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '...'
3. See error

**Expected Behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
- OS: [e.g. Ubuntu 22.04, Windows 11]
- Qt Version: [e.g. 6.5.2]
- LDM Version: [e.g. 1.0.0]
- Build Type: [Release/Debug]

**Additional Context**
Any other context about the problem.

**Logs**
```
Paste relevant log output here
```
```

### Feature Requests

```markdown
**Feature Description**
A clear description of what you want to happen.

**Problem Statement** 
What problem does this solve?

**Proposed Solution**
Describe the solution you'd like.

**Alternatives Considered**
Other solutions you've considered.

**Implementation Ideas**
Technical details if you have them.

**Additional Context**
Screenshots, mockups, or examples.
```

## 🏗️ Build System

### Makefile Structure

```bash
# Main builds
make -f Makefile.complete        # Complete application
make -f Makefile.idm            # IDM interface demo
make -f Makefile.simple         # Minimal build

# Development commands
make -f Makefile.complete debug  # Debug build
make -f Makefile.complete test   # Run tests
make -f Makefile.complete clean  # Clean build files

# Installation
make -f Makefile.complete install     # System install
make -f Makefile.complete uninstall   # Remove installation
```

### Adding New Source Files

1. **Add to appropriate directory**
   ```
   src/core/NewFeature.h
   src/core/NewFeature.cpp
   ```

2. **Update Makefile if needed**
   ```makefile
   SOURCES = src/main_complete.cpp \
             src/core/NewFeature.cpp
   ```

3. **Add to CMakeLists.txt**
   ```cmake
   add_executable(ldm-complete
       src/main_complete.cpp
       src/core/NewFeature.cpp
   )
   ```

## 🌍 Community

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and ideas
- **Pull Requests**: Code contributions and reviews

### Getting Help

- **Documentation**: Check README.md and docs/
- **Search Issues**: Look for existing solutions
- **Ask Questions**: Use GitHub Discussions
- **Join Development**: Comment on issues you'd like to work on

### Recognition

Contributors are recognized in:
- **README.md**: Major contributors listed
- **CHANGELOG.md**: Feature and fix attributions  
- **Release Notes**: Contributor acknowledgments
- **GitHub**: Contributor graph and statistics

## 🎖️ Contributor Levels

### 🌱 New Contributor
- First-time contributor
- Focus on documentation, small bugs
- Mentorship available

### 🔧 Regular Contributor  
- Multiple accepted contributions
- Can work on medium features
- Participate in design discussions

### 🏆 Core Contributor
- Significant contributions
- Help review other PRs
- Influence project direction

### 👑 Maintainer
- Project leadership
- Final review authority
- Release management

## 📚 Resources

### Learning Materials
- [Qt 6 Documentation](https://doc.qt.io/qt-6/)
- [Modern C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Git Workflow](https://guides.github.com/introduction/flow/)

### Development Tools
- **IDE**: Qt Creator, CLion, VS Code
- **Debugger**: GDB, LLDB
- **Profiler**: Valgrind, Intel VTune
- **Static Analysis**: Clang-Tidy, PVS-Studio

### Project-Specific Documentation
- [Architecture Overview](docs/architecture.md)
- [API Reference](docs/api-reference.md)
- [Build Instructions](docs/building.md)
- [User Guide](docs/user-guide.md)

---

## 🎉 Thank You!

Every contribution, no matter how small, makes LDM better for everyone. Whether you're:

- 🐛 **Fixing a typo** in documentation
- ✨ **Adding a major feature** 
- 🎨 **Improving the UI**
- 📝 **Writing tests**
- 🔍 **Reporting bugs**
- 💡 **Suggesting improvements**

**You are making a difference!** 

The LDM project thrives because of contributors like you. Thank you for being part of this journey to create the best download manager possible.

---

<div align="center">
  <h3>🚀 Ready to contribute?</h3>
  <p>
    <a href="https://github.com/aerab243/ldm/issues">Find an Issue</a> • 
    <a href="https://github.com/aerab243/ldm/fork">Fork the Repo</a> • 
    <a href="https://github.com/aerab243/ldm/discussions">Join Discussion</a>
  </p>
  <p><em>Made with ❤️ by the LDM Community</em></p>
</div>