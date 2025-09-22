# Release Guide - Linux Download Manager

This guide explains how to create releases for Linux Download Manager (LDM) using the automated GitHub workflows.

**Developer:** Anna-el Gerard RABENANDRASANA (aerab243)  
**Project:** https://github.com/aerab243/ldm  
**Contact:** aerabenandrasana@gmail.com

## Overview

Linux Download Manager uses GitHub Actions to automatically build and distribute packages for multiple platforms:

- **Linux**: DEB packages (Ubuntu/Debian), RPM packages (CentOS/RHEL), AppImage (universal)
- **Windows**: Installer (.exe) and Portable (.zip)
- **macOS**: DMG and portable packages (planned)

## Automated Workflows

### 1. Continuous Integration (`ci.yml`)

Runs automatically on every push to `main`/`develop` branches and pull requests:

- **Code Quality**: Formatting, static analysis, and linting
- **Multi-platform builds**: Linux, Windows, and macOS
- **Testing**: Runs test suites if available
- **Security**: Dependency vulnerability scanning
- **Documentation**: Checks for required documentation files

### 2. Release Workflow (`release.yml`)

Triggered when creating a new release tag:

- Builds packages for all supported platforms
- Uploads packages to GitHub Releases
- Creates comprehensive release notes
- Publishes draft release for review

### 3. Platform-specific Workflows

Individual workflows for testing specific platforms:
- `build-linux.yml`: Linux packages (DEB, RPM, AppImage)
- `build-windows.yml`: Windows installer and portable

## Creating a Release

### Method 1: Using the Release Script (Recommended)

The project includes a release script that automates version management:

```bash
# Navigate to project directory
cd ldm

# Create a patch release (1.0.0 → 1.0.1)
./scripts/release.sh patch

# Create a minor release (1.0.1 → 1.1.0)
./scripts/release.sh minor

# Create a major release (1.1.0 → 2.0.0)
./scripts/release.sh major

# Create a prerelease (1.0.0 → 1.0.1-rc.1)
./scripts/release.sh patch --prerelease

# Create a custom version
./scripts/release.sh custom --version 2.0.0

# Preview changes without making them
./scripts/release.sh minor --dry-run
```

The script will:
1. Update version files (`VERSION`, `CMakeLists.txt`, etc.)
2. Update the changelog
3. Create a git commit and tag
4. Push changes to GitHub
5. Trigger the automated build workflows

### Method 2: Manual Release

If you prefer manual control:

1. **Update version files:**
   ```bash
   # Update VERSION file
   echo "1.0.1" > VERSION
   
   # Update CMakeLists.txt
   sed -i 's/project(LDM VERSION [0-9.]*/project(LDM VERSION 1.0.1/' CMakeLists.txt
   ```

2. **Update changelog:**
   - Edit `CHANGELOG.md` to add release notes
   - Follow the [Keep a Changelog](https://keepachangelog.com/) format

3. **Commit and tag:**
   ```bash
   git add VERSION CMakeLists.txt CHANGELOG.md
   git commit -m "Release version 1.0.1"
   git tag -a v1.0.1 -m "Release 1.0.1"
   git push origin main
   git push origin v1.0.1
   ```

### Method 3: GitHub Web Interface

1. Go to your repository on GitHub
2. Click "Releases" → "Create a new release"
3. Choose "Create new tag" and enter version (e.g., `v1.0.1`)
4. Select target branch (usually `main`)
5. Fill in release title and description
6. Choose "Set as pre-release" if it's a beta/RC version
7. Click "Publish release"

## Package Information

### Linux Packages

#### DEB Package (Ubuntu/Debian)
- **File**: `ldm-{version}-amd64.deb`
- **Installation**: `sudo dpkg -i ldm-{version}-amd64.deb`
- **Dependencies**: Automatically resolved on supported systems
- **Removes cleanly**: `sudo apt remove ldm`

#### RPM Package (CentOS/RHEL/Fedora)
- **File**: `ldm-{version}-x86_64.rpm`
- **Installation**: `sudo rpm -i ldm-{version}-x86_64.rpm` or `sudo dnf install ldm-{version}-x86_64.rpm`
- **Dependencies**: May require EPEL repository for some dependencies

#### AppImage (Universal Linux)
- **File**: `LDM-{version}-x86_64.AppImage`
- **Installation**: Download, make executable, and run
- **No dependencies**: Self-contained with all libraries
- **Portable**: Can run from any location

### Windows Packages

#### Installer
- **File**: `LDM-Setup-{version}-Windows.exe`
- **Installation**: Run the installer as administrator
- **Features**: 
  - Start menu shortcuts
  - Desktop shortcut
  - Uninstaller registration
  - File associations (planned)

#### Portable
- **File**: `LDM-Portable-{version}-Windows.zip`
- **Installation**: Extract and run `ldm.exe`
- **No installation required**: Runs from any folder
- **Settings**: Stored in application directory

## Build Requirements

### Linux Dependencies
- Qt 6.5+
- CMake 3.16+
- C++20 compatible compiler
- System libraries: curl, OpenSSL, SQLite, FFmpeg, ClamAV, 7-Zip, Protocol Buffers

### Windows Dependencies
- Visual Studio 2022
- Qt 6.5+ for Windows
- vcpkg for dependency management
- NSIS for installer creation

## Troubleshooting

### Common Issues

#### Build Failures
1. **Missing dependencies**: Check if all required libraries are available
2. **Qt version mismatch**: Ensure Qt 6.5+ is installed
3. **Compiler issues**: Verify C++20 support

#### Package Issues
1. **DEB dependencies**: Missing packages can be installed with `apt-get install -f`
2. **RPM dependencies**: May need to enable additional repositories
3. **AppImage**: Requires FUSE on older systems

#### Release Process
1. **Permission denied**: Ensure `GITHUB_TOKEN` has appropriate permissions
2. **Tag already exists**: Delete tag locally and remotely before recreating
3. **Workflow failures**: Check GitHub Actions logs for specific error messages

### Getting Help

- **Issues**: Report problems on [GitHub Issues](https://github.com/aerab243/ldm/issues)
- **Discussions**: Use [GitHub Discussions](https://github.com/aerab243/ldm/discussions) for questions
- **Documentation**: Check the [project wiki](https://github.com/aerab243/ldm/wiki)
- **Contact**: Email the developer at aerabenandrasana@gmail.com

## Release Checklist

Before creating a release:

- [ ] All tests pass locally
- [ ] Version number follows semantic versioning
- [ ] Changelog is updated with all changes
- [ ] Documentation is up to date
- [ ] No known critical bugs
- [ ] Security vulnerabilities addressed
- [ ] Performance testing completed (for major releases)
- [ ] Browser extension compatibility verified
- [ ] Backup configurations tested

After creating a release:

- [ ] Verify all packages build successfully
- [ ] Test installation on target platforms
- [ ] Update documentation websites
- [ ] Announce release on social media/forums
- [ ] Monitor for bug reports
- [ ] Update package repositories (if maintaining custom repos)

## Version Numbering

Linux Download Manager follows [Semantic Versioning](https://semver.org/):

- **MAJOR** (X.0.0): Breaking changes, major feature additions
- **MINOR** (x.Y.0): New features, backward compatible
- **PATCH** (x.y.Z): Bug fixes, backward compatible

### Prerelease Versions
- **Release Candidates**: `1.0.0-rc.1`, `1.0.0-rc.2`
- **Beta**: `1.0.0-beta.1` (for major testing phases)
- **Alpha**: `1.0.0-alpha.1` (for early development builds)

## Security Considerations

- Release artifacts are signed with GitHub's signing key
- All dependencies are scanned for vulnerabilities
- Windows binaries may trigger antivirus warnings initially
- Linux packages are built in clean, reproducible environments
- Source code is available for security auditing at https://github.com/aerab243/ldm
- Developed by Anna-el Gerard RABENANDRASANA with focus on security and privacy

## Future Improvements

Planned enhancements to the release process:

- [ ] Code signing for Windows executables
- [ ] Notarization for macOS packages
- [ ] Automated security scanning
- [ ] Performance benchmarking in CI
- [ ] Automated screenshot generation
- [ ] Translation updates integration
- [ ] Package repository hosting
- [ ] Delta updates for faster downloads