# LDM Installation Guide

## Overview
This guide will walk you through installing LDM (Like Download Manager) on your system. LDM is available for Windows, macOS, and Linux platforms.

## System Requirements

### Minimum Requirements
- **Operating System**: 
  - Windows 10 or later
  - macOS 10.15 (Catalina) or later  
  - Ubuntu 18.04 LTS or equivalent Linux distribution
- **Memory (RAM)**: 4 GB
- **Storage**: 100 MB free disk space
- **Network**: Internet connection for downloads

### Recommended Requirements
- **Memory (RAM)**: 8 GB or more
- **Storage**: 1 GB free disk space (for download cache)
- **Processor**: Multi-core CPU for better performance

## Downloading LDM

1. Visit the official LDM website at [github.com/aerab243/ldm](https://github.com/aerab243/ldm)
2. Navigate to the Downloads section
3. Select your operating system
4. Download the appropriate installer package:
   - **Windows**: `ldm-setup-x.x.x.exe` (NSIS installer)
   - **macOS**: `ldm-x.x.x.dmg` (Disk Image)
   - **Linux**: 
     - Ubuntu/Debian: `ldm_x.x.x_amd64.deb`
     - Fedora/CentOS: `ldm-x.x.x.x86_64.rpm`

## Installation Instructions

### Windows Installation

1. **Download the Installer**
   - Download `ldm-setup-x.x.x.exe` from the official website

2. **Run the Installer**
   - Locate the downloaded file and double-click it
   - If prompted by User Account Control, click "Yes" to allow the installer to run

3. **Follow the Installation Wizard**
   - Welcome screen: Click "Next"
   - License Agreement: Read and accept the terms, click "Next"
   - Installation Location: Choose install directory or use default, click "Next"
   - Additional Tasks: Select options like desktop shortcut, start menu entry
   - Ready to Install: Click "Install"
   - Wait for installation to complete
   - Completion: Click "Finish"

4. **First Launch**
   - LDM should be available in Start Menu or desktop shortcut
   - Double-click to launch

### macOS Installation

1. **Download the Disk Image**
   - Download `ldm-x.x.x.dmg` from the official website

2. **Mount the Disk Image**
   - Double-click the downloaded `.dmg` file
   - The disk image will mount and open a Finder window

3. **Install LDM**
   - Drag the LDM icon into the Applications folder
   - Wait for the copy to complete

4. **Eject the Disk Image**
   - Right-click the mounted disk icon on desktop
   - Select "Eject" or drag to Trash

5. **First Launch**
   - Open Finder and navigate to Applications
   - Double-click LDM to launch
   - You may see a security warning - click "Open" to proceed

### Linux Installation

#### Ubuntu/Debian (.deb package)

1. **Download the Package**
   - Download `ldm_x.x.x_amd64.deb`

2. **Install via Package Manager**
   ```bash
   sudo dpkg -i ldm_x.x.x_amd64.deb
   sudo apt-get install -f  # Fix any dependencies
   ```

3. **Alternative: Install via Software Center**
   - Double-click the `.deb` file
   - Use Ubuntu Software Center or GDebi to install

#### Fedora/CentOS (.rpm package)

1. **Download the Package**
   - Download `ldm-x.x.x.x86_64.rpm`

2. **Install via Package Manager**
   ```bash
   sudo dnf install ldm-x.x.x.x86_64.rpm  # Fedora
   sudo yum install ldm-x.x.x.x86_64.rpm  # CentOS/RHEL
   ```

3. **Alternative: Install via GUI**
   - Use your distribution's software manager

#### Arch Linux (AUR)

1. **Install from AUR**
   ```bash
   yay -S ldm  # Using yay
   # or
   git clone https://aur.archlinux.org/ldm.git
   cd ldm
   makepkg -si
   ```

## Post-Installation Setup

### Launch LDM
- **Windows**: Use Start Menu or desktop shortcut
- **macOS**: Use Launchpad or Applications folder
- **Linux**: Use application menu or run `ldm` from terminal

### Initial Configuration
1. **First Run Wizard**
   - Set default download directory
   - Configure proxy settings (if needed)
   - Choose theme (light/dark)

2. **Browser Extension Setup**
   - LDM will prompt to install browser extensions
   - Supported browsers: Chrome, Firefox, Edge, Safari
   - Follow on-screen instructions for each browser

### Verify Installation
- Check that LDM appears in system tray/notification area
- Test with a small download to ensure functionality
- Verify browser extension is working

## Troubleshooting Installation

### Common Issues

#### Windows
- **"Access Denied" during installation**: Run installer as Administrator
- **Installation fails**: Disable antivirus temporarily, ensure sufficient disk space

#### macOS
- **"App is damaged" error**: 
  ```bash
  xattr -rd com.apple.quarantine /Applications/LDM.app
  ```
- **Gatekeeper blocks app**: Go to System Preferences > Security & Privacy > General tab, click "Open Anyway"

#### Linux
- **Missing dependencies**: Install required packages:
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libqt6network6 libqt6widgets6 libssl3
  
  # Fedora
  sudo dnf install qt6-qtnetwork qt6-qtbase openssl
  ```
- **Permission denied**: Ensure user has access to download directories

### Uninstalling LDM

#### Windows
- Go to Settings > Apps > LDM > Uninstall
- Or use the uninstaller in the installation directory

#### macOS
- Drag LDM from Applications to Trash
- Empty Trash

#### Linux
```bash
# Debian/Ubuntu
sudo apt-get remove ldm

# Fedora/CentOS
sudo dnf remove ldm
```

## Getting Help

If you encounter issues during installation:
- Check the [Troubleshooting Guide](troubleshooting.md)
- Visit the [GitHub Issues](https://github.com/aerab243/ldm/issues)
- Contact support at aerabenandrasana@gmail.com

## Next Steps

After successful installation:
1. Read the [Getting Started Guide](getting-started.md)
2. Learn about [Advanced Features](advanced-features.md)
3. Explore the [API Documentation](../api/rest-api.md) for developers

---

*Last updated: 2025-01-15*