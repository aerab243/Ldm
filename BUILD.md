# Building Linux Download Manager (LDM)

This document provides detailed instructions for building LDM from source.

## Quick Start

```bash
# Test dependencies
./test-deps.sh

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install
sudo make install
```

## System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu 20.04+, CentOS 8+, or equivalent)
- **Compiler**: GCC 9+ or Clang 10+ with C++20 support
- **CMake**: 3.16 or newer
- **Qt**: 6.2 or newer
- **RAM**: 2GB available during compilation
- **Disk**: 1GB free space for build

### Recommended Requirements
- **OS**: Ubuntu 22.04 LTS or newer
- **Compiler**: GCC 11+ or Clang 13+
- **CMake**: 3.20 or newer
- **Qt**: 6.5 or newer
- **RAM**: 4GB available during compilation
- **Disk**: 2GB free space for build and dependencies

## Dependencies

### Required Dependencies

#### Build Tools
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git pkg-config

# CentOS/RHEL/Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git pkg-config
```

#### Core Libraries
```bash
# Ubuntu/Debian
sudo apt-get install \
    libssl-dev \
    libsqlite3-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libcurl4-openssl-dev

# CentOS/RHEL/Fedora
sudo dnf install \
    openssl-devel \
    sqlite-devel \
    protobuf-devel \
    protobuf-compiler \
    libcurl-devel
```

#### Media Libraries (FFmpeg)
```bash
# Ubuntu/Debian
sudo apt-get install \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libswscale-dev

# CentOS/RHEL/Fedora
sudo dnf install ffmpeg-devel
```

#### Qt6 Framework

##### Ubuntu 22.04+
```bash
# Add Qt6 PPA for better Qt6 support
sudo add-apt-repository ppa:okirby/qt6-backports
sudo apt-get update

sudo apt-get install \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-l10n-tools \
    qt6-multimedia-dev \
    libqt6sql6-sqlite \
    libqt6opengl6-dev \
    libqt6svg6-dev
```

##### Ubuntu 20.04
Qt6 is not available in Ubuntu 20.04 repositories. You have two options:

1. **Upgrade to Ubuntu 22.04** (recommended)
2. **Install Qt6 manually**:
```bash
# Download Qt6 installer
wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run
chmod +x qt-unified-linux-x64-online.run
./qt-unified-linux-x64-online.run

# Set Qt6 path
export Qt6_DIR=/opt/Qt/6.5.0/gcc_64/lib/cmake/Qt6
```

##### CentOS/RHEL/Fedora
```bash
# Enable EPEL and CRB repositories
sudo dnf install epel-release
sudo dnf config-manager --set-enabled crb

sudo dnf install \
    qt6-qtbase-devel \
    qt6-qttools-devel \
    qt6-qtmultimedia-devel
```

#### Graphics Libraries
```bash
# Ubuntu/Debian
sudo apt-get install \
    libgl1-mesa-dev \
    libxkbcommon-dev \
    libxcb-xinerama0-dev \
    libxcb-cursor-dev

# CentOS/RHEL/Fedora
sudo dnf install \
    mesa-libGL-devel \
    libxkbcommon-devel \
    xcb-util-cursor-devel
```

### Optional Dependencies

#### Compression Support
```bash
# Ubuntu/Debian
sudo apt-get install p7zip-full

# CentOS/RHEL/Fedora
sudo dnf install p7zip p7zip-plugins
```

#### Virus Scanning
```bash
# Ubuntu/Debian
sudo apt-get install libclamav-dev

# CentOS/RHEL/Fedora
sudo dnf install clamav-devel
```

#### Qt6 Charts (Optional)
```bash
# Ubuntu/Debian (if available)
sudo apt-get install qt6-charts-dev

# If not available, LDM will build without charts functionality
```

## Building

### 1. Check Dependencies
Run the dependency checker to ensure all required components are available:
```bash
./test-deps.sh
```

### 2. Configure Build
```bash
mkdir build
cd build

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Custom installation prefix
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/ldm

# Disable optional features
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CHARTS=OFF
```

### 3. Compile
```bash
# Use all available cores
make -j$(nproc)

# Or specify number of jobs
make -j4

# Verbose output for debugging
make VERBOSE=1
```

### 4. Install
```bash
# System-wide installation (requires sudo)
sudo make install

# Or create packages
make package
```

## Build Options

### CMake Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type: `Debug`, `Release`, `RelWithDebInfo` |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Installation directory |
| `ENABLE_CHARTS` | `AUTO` | Enable Qt6 Charts (auto-detected) |
| `ENABLE_BROWSER_INTEGRATION` | `ON` | Enable browser integration |
| `ENABLE_VIRUS_SCANNING` | `AUTO` | Enable ClamAV integration |
| `ENABLE_COMPRESSION` | `AUTO` | Enable 7-zip support |
| `ENABLE_TESTS` | `ON` | Build unit tests |

### Example Configurations

#### Minimal Build
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_CHARTS=OFF \
    -DENABLE_VIRUS_SCANNING=OFF \
    -DENABLE_COMPRESSION=OFF \
    -DENABLE_TESTS=OFF
```

#### Development Build
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_TESTS=ON \
    -DENABLE_DEBUG_LOGGING=ON
```

#### Portable Build
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/opt/ldm \
    -DENABLE_PORTABLE=ON
```

## Packaging

### DEB Package (Ubuntu/Debian)
```bash
cd build
make package
# Creates: LDM-1.0.0-Linux.deb
```

### RPM Package (CentOS/RHEL/Fedora)
```bash
cd build
cpack -G RPM
# Creates: LDM-1.0.0-Linux.rpm
```

### AppImage (Universal Linux)
```bash
cd build
cpack -G TGZ
# Then use linuxdeploy to create AppImage
```

## Troubleshooting

### Common Issues

#### Qt6 Not Found
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solutions:**
1. Install Qt6 development packages
2. Set `Qt6_DIR` environment variable:
   ```bash
   export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
   ```
3. Use Qt6 from PPA (Ubuntu) or manual installation

#### Missing p7zip-dev
```
E: Unable to locate package p7zip-dev
```

**Solution:** Use `p7zip-full` instead:
```bash
sudo apt-get install p7zip-full
```

#### FFmpeg Not Found
```
Package 'libavformat' not found
```

**Solutions:**
1. Enable additional repositories:
   ```bash
   # Ubuntu
   sudo add-apt-repository universe
   sudo apt-get update
   
   # CentOS/RHEL
   sudo dnf install epel-release
   sudo dnf config-manager --set-enabled powertools  # CentOS 8
   sudo dnf config-manager --set-enabled crb        # CentOS 9
   ```

#### Compilation Errors with C++20
```
error: 'std::format' is not a member of 'std'
```

**Solution:** Update to a newer compiler:
```bash
# Ubuntu
sudo apt-get install gcc-11 g++-11
export CC=gcc-11
export CXX=g++-11

# Or use Clang
sudo apt-get install clang-13
export CC=clang-13
export CXX=clang++-13
```

#### Out of Memory During Compilation
```
g++: internal compiler error: Killed (program cc1plus)
```

**Solutions:**
1. Reduce parallel jobs: `make -j2` instead of `make -j$(nproc)`
2. Add swap space:
   ```bash
   sudo fallocate -l 2G /swapfile
   sudo chmod 600 /swapfile
   sudo mkswap /swapfile
   sudo swapon /swapfile
   ```
3. Use a machine with more RAM

### Debug Build Issues

#### Debug Information Too Large
**Solution:** Use `RelWithDebInfo` instead of `Debug`:
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

#### Slow Debug Build
**Solution:** Disable some features in debug mode:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_PERFORMANCE_MONITORING=OFF
```

### Platform-Specific Issues

#### Ubuntu 20.04
- Qt6 not available in repositories → Use manual installation or upgrade to 22.04
- Some packages have different names → Check with `apt search`

#### CentOS 7
- Default GCC too old → Install devtoolset:
  ```bash
  sudo yum install centos-release-scl
  sudo yum install devtoolset-11-gcc-c++
  scl enable devtoolset-11 bash
  ```

#### Arch Linux
```bash
sudo pacman -S base-devel cmake git pkg-config qt6-base qt6-multimedia qt6-charts curl sqlite protobuf ffmpeg openssl p7zip
```

## Testing the Build

### Run Unit Tests
```bash
cd build
make test
# Or
ctest --verbose
```

### Manual Testing
```bash
cd build
./desktop/ldm-desktop --version
./desktop/ldm-cli --help
```

### Integration Testing
```bash
# Test basic functionality
./desktop/ldm-cli --add-url "https://example.com/file.zip" --output "/tmp/"

# Test GUI
./desktop/ldm-desktop
```

## Development Setup

### IDE Configuration

#### VS Code
Install extensions:
- C/C++
- CMake
- CMake Tools
- Qt tools

#### Qt Creator
1. Open `CMakeLists.txt` as project
2. Configure with Qt6 kit
3. Set build directory to `build`

### Debugging
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Run with GDB
gdb ./desktop/ldm-desktop

# Run with Valgrind
valgrind --leak-check=full ./desktop/ldm-desktop
```

## Contributing

### Code Style
- C++20 standard
- Qt coding conventions
- Use `clang-format` with provided `.clang-format`
- Follow RAII principles

### Before Submitting
1. Run tests: `make test`
2. Check code style: `clang-format -i src/**/*.cpp src/**/*.h`
3. Update documentation if needed
4. Test on multiple distributions if possible

## Getting Help

- **GitHub Issues**: https://github.com/aerab243/ldm/issues
- **Wiki**: https://github.com/aerab243/ldm/wiki
- **Email**: aerabenandrasana@gmail.com

For build-specific issues, please include:
- Operating system and version
- Compiler version
- CMake version
- Qt version
- Full error output
- Output of `./test-deps.sh`
