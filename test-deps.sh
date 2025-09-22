#!/bin/bash

# Test Dependencies Script for Linux Download Manager (LDM)
# This script tests if all required dependencies are available for building LDM

set -e

echo "=== Linux Download Manager - Dependency Test ==="
echo "Testing build dependencies for LDM..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
MISSING=0
AVAILABLE=0

# Function to check if a command exists
check_command() {
    if command -v "$1" >/dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} $1 is available"
        ((AVAILABLE++))
        return 0
    else
        echo -e "${RED}✗${NC} $1 is missing"
        ((MISSING++))
        return 1
    fi
}

# Function to check if a package is installed (Ubuntu/Debian)
check_package() {
    if dpkg -l | grep -q "^ii  $1 "; then
        echo -e "${GREEN}✓${NC} Package $1 is installed"
        ((AVAILABLE++))
        return 0
    else
        echo -e "${RED}✗${NC} Package $1 is missing"
        ((MISSING++))
        return 1
    fi
}

# Function to check if a library is available via pkg-config
check_pkgconfig() {
    if pkg-config --exists "$1" 2>/dev/null; then
        version=$(pkg-config --modversion "$1" 2>/dev/null || echo "unknown")
        echo -e "${GREEN}✓${NC} $1 is available (version: $version)"
        ((AVAILABLE++))
        return 0
    else
        echo -e "${RED}✗${NC} $1 is missing or not found via pkg-config"
        ((MISSING++))
        return 1
    fi
}

# Function to check Qt6 components
check_qt6() {
    local component="$1"
    local display_name="$2"

    # Try different methods to detect Qt6
    if command -v qmake6 >/dev/null 2>&1; then
        qt_version=$(qmake6 -query QT_VERSION 2>/dev/null || echo "unknown")
        if [[ $qt_version == 6.* ]]; then
            echo -e "${GREEN}✓${NC} Qt6 detected (version: $qt_version)"
            ((AVAILABLE++))
            return 0
        fi
    fi

    # Check via pkg-config
    if pkg-config --exists "Qt6$component" 2>/dev/null; then
        version=$(pkg-config --modversion "Qt6$component" 2>/dev/null || echo "unknown")
        echo -e "${GREEN}✓${NC} $display_name is available (version: $version)"
        ((AVAILABLE++))
        return 0
    fi

    # Check for installed packages
    if dpkg -l | grep -q "libqt6.*$component"; then
        echo -e "${GREEN}✓${NC} $display_name appears to be installed"
        ((AVAILABLE++))
        return 0
    fi

    echo -e "${RED}✗${NC} $display_name is missing"
    ((MISSING++))
    return 1
}

echo
echo "=== Build Tools ==="
check_command "gcc"
check_command "g++"
check_command "make"
check_command "cmake"
check_command "git"
check_command "pkg-config"

echo
echo "=== Core Libraries ==="
check_pkgconfig "libcurl"
check_pkgconfig "sqlite3"
check_pkgconfig "openssl"
check_pkgconfig "protobuf"

echo
echo "=== Media Libraries ==="
check_pkgconfig "libavformat"
check_pkgconfig "libavcodec"
check_pkgconfig "libavutil"
check_pkgconfig "libswscale"

echo
echo "=== Qt6 Framework ==="
check_qt6 "Core" "Qt6 Core"
check_qt6 "Widgets" "Qt6 Widgets"
check_qt6 "Network" "Qt6 Network"
check_qt6 "Sql" "Qt6 SQL"
check_qt6 "Multimedia" "Qt6 Multimedia"

echo
echo "=== Optional Qt6 Components ==="
if check_qt6 "Charts" "Qt6 Charts"; then
    echo -e "${GREEN}  Charts functionality will be enabled${NC}"
else
    echo -e "${YELLOW}  Charts functionality will be disabled${NC}"
fi

echo
echo "=== Additional Tools ==="
check_command "7z" || check_command "7za" || echo -e "${YELLOW}⚠${NC} 7-zip not found (p7zip-full package)"

# Check for ClamAV (optional)
if check_pkgconfig "libclamav"; then
    echo -e "${GREEN}  Virus scanning will be enabled${NC}"
else
    echo -e "${YELLOW}  Virus scanning will be disabled${NC}"
fi

echo
echo "=== System Information ==="
echo "OS: $(lsb_release -d -s 2>/dev/null || echo "Unknown Linux")"
echo "Kernel: $(uname -r)"
echo "Architecture: $(uname -m)"

if command -v cmake >/dev/null 2>&1; then
    echo "CMake version: $(cmake --version | head -n1 | cut -d' ' -f3)"
fi

if command -v gcc >/dev/null 2>&1; then
    echo "GCC version: $(gcc --version | head -n1 | cut -d' ' -f4)"
fi

echo
echo "=== Summary ==="
echo -e "Available dependencies: ${GREEN}$AVAILABLE${NC}"
echo -e "Missing dependencies: ${RED}$MISSING${NC}"

if [ $MISSING -eq 0 ]; then
    echo -e "${GREEN}✓ All dependencies are available! You can build LDM.${NC}"
    exit 0
elif [ $MISSING -le 3 ]; then
    echo -e "${YELLOW}⚠ Some optional dependencies are missing, but you should be able to build LDM with reduced functionality.${NC}"
    exit 0
else
    echo -e "${RED}✗ Too many dependencies are missing. Please install the required packages.${NC}"
    echo
    echo "For Ubuntu/Debian, try:"
    echo "sudo apt-get update"
    echo "sudo apt-get install build-essential cmake git pkg-config libssl-dev libsqlite3-dev libprotobuf-dev protobuf-compiler libavformat-dev libavcodec-dev libavutil-dev libswscale-dev p7zip-full libcurl4-openssl-dev libgl1-mesa-dev libxkbcommon-dev"
    echo "sudo add-apt-repository ppa:okirby/qt6-backports"
    echo "sudo apt-get update"
    echo "sudo apt-get install qt6-base-dev qt6-tools-dev qt6-multimedia-dev libqt6sql6-sqlite"
    exit 1
fi
