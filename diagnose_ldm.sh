#!/bin/bash

# LDM Diagnostic Script
# Comprehensive diagnostic tool for troubleshooting LDM issues

set -e

echo "=============================================="
echo "LDM - Like Download Manager Diagnostic Tool"
echo "=============================================="
echo
echo "Diagnosing system and LDM installation..."
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Counters
ISSUES=0
WARNINGS=0
SUCCESS=0

# Function to log results
log_success() {
    echo -e "${GREEN}✓${NC} $1"
    ((SUCCESS++))
}

log_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
    ((WARNINGS++))
}

log_error() {
    echo -e "${RED}✗${NC} $1"
    ((ISSUES++))
}

log_info() {
    echo -e "${CYAN}ℹ${NC} $1"
}

# Check if we're in the correct directory
if [ ! -d "desktop" ]; then
    log_error "Not in LDM root directory. Please run from ldm/ directory."
    exit 1
fi

echo -e "${WHITE}=== SYSTEM INFORMATION ===${NC}"
echo

# Basic system info
echo "System Information:"
echo "  OS: $(lsb_release -d -s 2>/dev/null || echo "Unknown Linux")"
echo "  Kernel: $(uname -r)"
echo "  Architecture: $(uname -m)"
echo "  Date: $(date)"
echo

# Display environment
echo "Environment:"
echo "  USER: $USER"
echo "  HOME: $HOME"
echo "  DISPLAY: ${DISPLAY:-"Not set"}"
echo "  XDG_SESSION_TYPE: ${XDG_SESSION_TYPE:-"Not set"}"
echo

echo -e "${WHITE}=== BUILD TOOLS CHECK ===${NC}"
echo

# Check essential build tools
if command -v gcc >/dev/null 2>&1; then
    log_success "GCC: $(gcc --version | head -n1)"
else
    log_error "GCC not found - required for compilation"
fi

if command -v g++ >/dev/null 2>&1; then
    log_success "G++: $(g++ --version | head -n1)"
else
    log_error "G++ not found - required for C++ compilation"
fi

if command -v make >/dev/null 2>&1; then
    log_success "Make: $(make --version | head -n1)"
else
    log_error "Make not found - required for building"
fi

if command -v cmake >/dev/null 2>&1; then
    log_success "CMake: $(cmake --version | head -n1)"
else
    log_warning "CMake not found - optional but recommended"
fi

if command -v pkg-config >/dev/null 2>&1; then
    log_success "pkg-config: $(pkg-config --version)"
else
    log_error "pkg-config not found - required for library detection"
fi

echo

echo -e "${WHITE}=== QT6 FRAMEWORK CHECK ===${NC}"
echo

# Check Qt6 components
QT6_ISSUES=0

if pkg-config --exists Qt6Core 2>/dev/null; then
    QT6_VERSION=$(pkg-config --modversion Qt6Core)
    log_success "Qt6Core: version $QT6_VERSION"
else
    log_error "Qt6Core not found - install qt6-base-dev"
    ((QT6_ISSUES++))
fi

if pkg-config --exists Qt6Widgets 2>/dev/null; then
    log_success "Qt6Widgets: available"
else
    log_error "Qt6Widgets not found - install qt6-base-dev"
    ((QT6_ISSUES++))
fi

if pkg-config --exists Qt6Network 2>/dev/null; then
    log_success "Qt6Network: available"
else
    log_error "Qt6Network not found - install qt6-base-dev"
    ((QT6_ISSUES++))
fi

if pkg-config --exists Qt6Gui 2>/dev/null; then
    log_success "Qt6Gui: available"
else
    log_error "Qt6Gui not found - install qt6-base-dev"
    ((QT6_ISSUES++))
fi

# Check for Qt6 MOC
MOC_FOUND=false
for moc_path in "/usr/lib64/qt6/libexec/moc" "/usr/lib/qt6/libexec/moc" "moc6" "moc-qt6"; do
    if [ -f "$moc_path" ] || command -v "$moc_path" >/dev/null 2>&1; then
        log_success "Qt6 MOC found at: $moc_path"
        MOC_FOUND=true
        break
    fi
done

if [ "$MOC_FOUND" = false ]; then
    log_error "Qt6 MOC not found - install qt6-tools-dev"
    ((QT6_ISSUES++))
fi

if [ $QT6_ISSUES -eq 0 ]; then
    log_success "Qt6 framework complete"
else
    log_error "Qt6 framework incomplete ($QT6_ISSUES issues)"
fi

echo

echo -e "${WHITE}=== ADDITIONAL LIBRARIES ===${NC}"
echo

# Check additional libraries
if pkg-config --exists libcurl 2>/dev/null; then
    log_success "libcurl: $(pkg-config --modversion libcurl)"
else
    log_warning "libcurl not found - install libcurl4-openssl-dev"
fi

if pkg-config --exists sqlite3 2>/dev/null; then
    log_success "sqlite3: $(pkg-config --modversion sqlite3)"
else
    log_warning "sqlite3 not found - install libsqlite3-dev"
fi

if pkg-config --exists openssl 2>/dev/null; then
    log_success "OpenSSL: $(pkg-config --modversion openssl)"
else
    log_warning "OpenSSL not found - install libssl-dev"
fi

echo

echo -e "${WHITE}=== LDM FILES CHECK ===${NC}"
echo

# Check LDM files and directories
if [ -d "desktop" ]; then
    log_success "Desktop directory exists"
else
    log_error "Desktop directory missing"
fi

if [ -d "desktop/src" ]; then
    log_success "Source directory exists"
else
    log_error "Source directory missing"
fi

# Check for source files
SOURCE_FILES=("main.cpp" "main_complete.cpp" "main_idm_complete.cpp" "main_working.cpp" "main_cli_simple.cpp")
FOUND_SOURCES=0

for source in "${SOURCE_FILES[@]}"; do
    if [ -f "desktop/src/$source" ]; then
        log_success "Source file: $source"
        ((FOUND_SOURCES++))
    fi
done

if [ $FOUND_SOURCES -eq 0 ]; then
    log_error "No source files found"
else
    log_info "Found $FOUND_SOURCES source files"
fi

# Check for executables
echo
echo "Executable Status:"

EXECUTABLES=("ldm-working" "ldm-cli" "ldm-complete" "ldm-complete-fixed")
WORKING_EXECUTABLES=0

for exe in "${EXECUTABLES[@]}"; do
    if [ -f "desktop/$exe" ]; then
        if [ -x "desktop/$exe" ]; then
            log_success "Executable: $exe ($(stat -c%s desktop/$exe) bytes)"
            ((WORKING_EXECUTABLES++))
        else
            log_warning "File exists but not executable: $exe"
        fi
    else
        log_info "Not found: $exe"
    fi
done

echo

echo -e "${WHITE}=== LAUNCH SCRIPTS CHECK ===${NC}"
echo

# Check launch scripts
SCRIPTS=("launch_ldm_working.sh" "launch_ldm_cli.sh" "build_working.sh" "build_cli.sh")
for script in "${SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        if [ -x "$script" ]; then
            log_success "Launch script: $script"
        else
            log_warning "Script exists but not executable: $script"
        fi
    else
        log_info "Script not found: $script"
    fi
done

echo

echo -e "${WHITE}=== RUNTIME TESTS ===${NC}"
echo

# Test executables if they exist
if [ -f "desktop/ldm-cli" ] && [ -x "desktop/ldm-cli" ]; then
    echo "Testing CLI version..."
    if timeout 5s ./desktop/ldm-cli --version >/dev/null 2>&1; then
        log_success "CLI version runs correctly"
    else
        log_error "CLI version fails to run"
    fi
fi

if [ -f "desktop/ldm-working" ] && [ -x "desktop/ldm-working" ]; then
    echo "Testing working GUI version..."
    if timeout 5s ./desktop/ldm-working --version >/dev/null 2>&1; then
        log_success "Working GUI version runs correctly"
    else
        log_warning "Working GUI version has issues (may need display)"
    fi
fi

if [ -f "desktop/ldm-complete" ] && [ -x "desktop/ldm-complete" ]; then
    echo "Testing complete version..."
    if timeout 5s ./desktop/ldm-complete --version >/dev/null 2>&1; then
        log_success "Complete version runs"
    else
        log_warning "Complete version has startup issues"
    fi
fi

echo

echo -e "${WHITE}=== GRAPHICS ENVIRONMENT ===${NC}"
echo

# Check graphics environment
if [ -n "$DISPLAY" ]; then
    log_success "DISPLAY variable set: $DISPLAY"

    if command -v xrandr >/dev/null 2>&1 && xrandr >/dev/null 2>&1; then
        log_success "X11 display accessible"
    else
        log_warning "X11 display may not be accessible"
    fi
else
    log_warning "DISPLAY variable not set (GUI apps may not work)"
fi

if [ -n "$WAYLAND_DISPLAY" ]; then
    log_success "Wayland display available: $WAYLAND_DISPLAY"
fi

if command -v systemctl >/dev/null 2>&1; then
    if systemctl --user is-active --quiet graphical-session.target 2>/dev/null; then
        log_success "Graphical session active"
    else
        log_warning "Graphical session may not be active"
    fi
fi

echo

echo -e "${WHITE}=== RECOMMENDATIONS ===${NC}"
echo

# Provide recommendations based on findings
if [ $QT6_ISSUES -gt 0 ]; then
    echo -e "${YELLOW}Qt6 Issues Found:${NC}"
    echo "  Install Qt6 development packages:"
    echo "    sudo dnf install qt6-qtbase-devel qt6-qttools-devel  # Fedora/CentOS"
    echo "    sudo apt install qt6-base-dev qt6-tools-dev          # Ubuntu/Debian"
    echo
fi

if [ $WORKING_EXECUTABLES -eq 0 ]; then
    echo -e "${YELLOW}No working executables found:${NC}"
    echo "  Build LDM executables:"
    echo "    ./build_working.sh    # Build working GUI version"
    echo "    ./build_cli.sh        # Build CLI version"
    echo
fi

if [ $WORKING_EXECUTABLES -gt 0 ]; then
    echo -e "${GREEN}Working executables available:${NC}"
    if [ -f "desktop/ldm-working" ]; then
        echo "  Recommended: ./launch_ldm_working.sh"
    elif [ -f "desktop/ldm-cli" ]; then
        echo "  Available: ./launch_ldm_cli.sh"
    fi
    echo
fi

# Display package installation commands
echo -e "${CYAN}Package Installation Commands:${NC}"
echo
echo "Fedora/CentOS/RHEL:"
echo "  sudo dnf groupinstall \"Development Tools\""
echo "  sudo dnf install qt6-qtbase-devel qt6-qttools-devel libcurl-devel sqlite-devel"
echo
echo "Ubuntu/Debian:"
echo "  sudo apt update"
echo "  sudo apt install build-essential qt6-base-dev qt6-tools-dev libcurl4-openssl-dev libsqlite3-dev"
echo

echo -e "${WHITE}=== DIAGNOSTIC SUMMARY ===${NC}"
echo

echo "Results:"
echo "  ✓ Successful checks: $SUCCESS"
echo "  ⚠ Warnings: $WARNINGS"
echo "  ✗ Issues found: $ISSUES"
echo

if [ $ISSUES -eq 0 ] && [ $WORKING_EXECUTABLES -gt 0 ]; then
    echo -e "${GREEN}✓ LDM should work correctly!${NC}"
    echo "  Run: ./test_all_versions.sh for available options"
elif [ $ISSUES -eq 0 ]; then
    echo -e "${YELLOW}⚠ System is ready, but LDM needs to be built${NC}"
    echo "  Run: ./build_working.sh to build working version"
elif [ $QT6_ISSUES -gt 0 ]; then
    echo -e "${RED}✗ Qt6 framework issues prevent LDM from working${NC}"
    echo "  Install Qt6 development packages first"
else
    echo -e "${RED}✗ Multiple issues found${NC}"
    echo "  Follow recommendations above to resolve issues"
fi

echo
echo "For support, contact:"
echo "  Developer: Anna-el Gerard RABENANDRASANA (aerab243)"
echo "  Email: aerabenandrasana@gmail.com"
echo "  Project: https://github.com/aerab243/ldm"
echo
echo "Diagnostic completed: $(date)"
