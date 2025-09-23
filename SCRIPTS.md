# LDM Scripts Documentation

This document describes all available scripts in the LDM project after cleanup.

## 🚀 Essential Scripts

### `start_ldm.sh` ⭐ **MAIN LAUNCHER**
**Primary entry point for LDM**
```bash
./start_ldm.sh [options]
```
- Automatically detects and launches the best available LDM version
- Tries GUI version first, fallback to CLI
- Auto-builds if needed
- **Recommended for all users**

### `install.sh` 
**System installation script**
```bash
./install.sh
```
- Installs LDM system-wide
- Creates desktop entries
- Sets up system integration
- Requires sudo privileges

### `uninstall.sh` 
**Complete removal script**
```bash
./uninstall.sh
```
- Removes ALL LDM files and configurations
- Cleans user data and system installations
- Interactive confirmation prompts
- Comprehensive cleanup

## 🔧 Build Scripts

### `build_working.sh`
**Build the working GUI version**
```bash
./build_working.sh
```
- Compiles `ldm-working` (GUI without system tray issues)
- Creates all necessary launcher scripts
- **Most reliable GUI build**

### `build_cli.sh`
**Build the CLI version**
```bash
./build_cli.sh
```
- Compiles `ldm-cli` (command-line interface)
- Always works regardless of GUI environment
- **Fallback option**

## 🧪 Testing & Diagnostics

### `test-deps.sh`
**Dependency checker**
```bash
./test-deps.sh
```
- Checks all required dependencies
- Validates Qt6 installation
- Reports missing packages
- **Run before building**

### `diagnose_ldm.sh`
**Comprehensive diagnostic tool**
```bash
./diagnose_ldm.sh
```
- Complete system analysis
- Build environment validation
- Runtime testing
- **Troubleshooting tool**

### `validate.sh`
**Project validation**
```bash
./validate.sh
```
- Validates project structure
- Checks file integrity
- Development tool

## 📁 Script Categories

### User Scripts (Ready to use)
- `start_ldm.sh` - Main launcher ⭐
- `install.sh` - System installation
- `uninstall.sh` - Complete removal

### Developer Scripts (Build & test)
- `build_working.sh` - Build GUI version
- `build_cli.sh` - Build CLI version
- `test-deps.sh` - Check dependencies
- `diagnose_ldm.sh` - Diagnose issues
- `validate.sh` - Validate project

## 🎯 Usage Scenarios

### First Time Setup
```bash
# 1. Check dependencies
./test-deps.sh

# 2. Build LDM
./build_working.sh

# 3. Launch
./start_ldm.sh
```

### Regular Usage
```bash
# Just launch (simplest)
./start_ldm.sh
```

### Installation
```bash
# System-wide install
sudo ./install.sh

# Launch from anywhere
ldm
```

### Troubleshooting
```bash
# Diagnose issues
./diagnose_ldm.sh

# Check dependencies
./test-deps.sh

# Rebuild if needed
./build_working.sh
```

### Complete Removal
```bash
# Remove everything
./uninstall.sh
```

## 🔄 Script Dependencies

```
start_ldm.sh
├── desktop/ldm-working (built by build_working.sh)
├── desktop/ldm-cli (built by build_cli.sh)
└── build_*.sh (auto-called if needed)

build_working.sh
├── Qt6 packages (checked by test-deps.sh)
└── C++ compiler

build_cli.sh
└── C++ compiler (minimal dependencies)

install.sh
├── Compiled executables
└── sudo privileges

uninstall.sh
└── sudo privileges (for system files)
```

## ⚠️ Important Notes

### Removed Scripts
The following scripts were removed during cleanup:
- All demo scripts (`demo_*.sh`)
- Multiple launcher variants (`launch_ldm_*.sh`)
- Duplicate test scripts (`test_ldm_*.sh`)
- Legacy build scripts (`run_ldm_*.sh`, `rebuild_*.sh`)
- Redundant documentation files

### Kept Scripts
Only essential, non-redundant scripts remain:
- **1 main launcher** (`start_ldm.sh`)
- **2 build scripts** (working + cli)
- **3 utility scripts** (install, uninstall, test-deps)
- **2 diagnostic scripts** (diagnose, validate)

### Script Permissions
All scripts are executable:
```bash
chmod +x *.sh
```

## 🆘 Support

If scripts don't work:
1. Run `./diagnose_ldm.sh` for analysis
2. Check `./test-deps.sh` for missing dependencies
3. Try `./build_working.sh` to rebuild
4. Contact developer: aerabenandrasana@gmail.com

## 📝 Development Notes

### Adding New Scripts
- Keep naming consistent (`action_target.sh`)
- Add documentation here
- Make executable: `chmod +x script.sh`
- Follow existing script patterns

### Script Standards
- Use `set -e` for error handling
- Include help/usage information
- Use colored output for clarity
- Provide meaningful error messages
- Clean up temporary files

**Project**: https://github.com/aerab243/ldm  
**Developer**: Anna-el Gerard RABENANDRASANA (aerab243)  
**Last Updated**: 2025-01-15